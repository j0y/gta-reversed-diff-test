#!/bin/bash
set -euo pipefail

# run-differential-test.sh — Orchestrates differential testing of gta-reversed.
#
# Runs the game multiple times with different hook configurations and compares
# game state hashes to detect behavioral differences between reversed and
# original code.
#
# Usage (inside Docker):
#   /scripts/run-differential-test.sh
#
# Usage (from host):
#   docker run --rm \
#       -v ./GTASA:/game:ro \
#       -v ./build-output:/build:ro \
#       -v ./gta_sa_compact.exe:/gamebin/gta_sa_compact.exe:ro \
#       -v ./scripts:/scripts:ro \
#       -v ./configs:/configs:ro \
#       gta-reversed-build /scripts/run-differential-test.sh

WINEPREFIX="${WINEPREFIX:-/opt/wine-gtasa}"
GAMEDIR="$WINEPREFIX/drive_c/Games/GTASA"
RESULTS_DIR="/tmp/diff_results"
# Wine C:\ maps to $WINEPREFIX/drive_c — use Windows paths for game process
WINE_RESULTS_DIR="C:\\diff_results"
WINE_RESULTS_UNIX="$WINEPREFIX/drive_c/diff_results"
GAME_TIMEOUT="${GAME_TIMEOUT:-120}"
WARMUP_FRAMES="${WARMUP_FRAMES:-300}"
MEASURE_FRAMES="${MEASURE_FRAMES:-100}"
BASELINE_RUNS="${BASELINE_RUNS:-3}"

export WINEPREFIX
export WINEARCH=win32
export WINEDLLOVERRIDES="d3d9=n"
export WINEDEBUG=-all

# ---------------------------------------------------------------------------
# Setup
# ---------------------------------------------------------------------------

setup_wine_prefix() {
    if [ -f /scripts/setup-wine-prefix.sh ]; then
        /scripts/setup-wine-prefix.sh
    fi
}

setup_game_dir() {
    mkdir -p "$GAMEDIR"
    rsync -a /game/ "$GAMEDIR/" 2>/dev/null || true
    cp /gamebin/gta_sa_compact.exe "$GAMEDIR/gta_sa.exe" 2>/dev/null || true
    cp /build/gta_reversed.asi "$GAMEDIR/" 2>/dev/null || true
    cp /build/d3d9.dll "$GAMEDIR/" 2>/dev/null || true
    cp /configs/gta-reversed.ini "$GAMEDIR/" 2>/dev/null || true

    # Install MSVC runtime DLLs (same as run-headless.sh)
    local VCRT_DIR="/opt/msvc/VC/Redist/MSVC/14.50.35710/x86/Microsoft.VC145.CRT"
    local UCRT_DIR="/opt/msvc/Windows Kits/10/Redist/10.0.26100.0/ucrt/DLLs/x86"
    local SYS32="$WINEPREFIX/drive_c/windows/system32"
    for dll in msvcp140.dll msvcp140_1.dll msvcp140_2.dll vcruntime140.dll concrt140.dll; do
        [ -f "${VCRT_DIR}/${dll}" ] && cp -f "${VCRT_DIR}/${dll}" "$GAMEDIR/" && cp -f "${VCRT_DIR}/${dll}" "$SYS32/"
    done
    [ -f "${UCRT_DIR}/ucrtbase.dll" ] && cp -f "${UCRT_DIR}/ucrtbase.dll" "$GAMEDIR/" && cp -f "${UCRT_DIR}/ucrtbase.dll" "$SYS32/"
}

start_xvfb() {
    if ! pgrep -x Xvfb > /dev/null 2>&1; then
        Xvfb :99 -screen 0 800x600x24 &>/dev/null &
        sleep 1
    fi
    export DISPLAY=:99
}

# ---------------------------------------------------------------------------
# Run a single game instance with given config
# ---------------------------------------------------------------------------

run_game() {
    local run_name="$1"
    local disable_hooks="${2:-}"
    local disable_all="${3:-0}"

    local result_file="${RESULTS_DIR}/${run_name}.txt"
    # Use Windows path for the game process (fopen in MSVC CRT)
    local wine_result_file="${WINE_RESULTS_DIR}\\${run_name}.txt"
    mkdir -p "$WINE_RESULTS_UNIX"

    export DIFF_TEST_ENABLE=1
    export DIFF_TEST_WARMUP_FRAMES="$WARMUP_FRAMES"
    export DIFF_TEST_MEASURE_FRAMES="$MEASURE_FRAMES"
    export DIFF_TEST_RESULTS_FILE="$wine_result_file"
    export DIFF_TEST_DISABLE_HOOKS="$disable_hooks"
    export DIFF_TEST_DISABLE_ALL="$disable_all"

    # --unhook-except keeps only FastLoader (CLoadingScreen) hooks active so
    # the game gets past startup dialogs.  All other reversed hooks are disabled
    # by default; the diff test harness selectively re-enables categories to test.
    cd "$GAMEDIR"
    timeout "$GAME_TIMEOUT" wine gta_sa.exe \
        --unhook-except=Global/CLoadingScreen 2>/dev/null || true
    wineserver -k 2>/dev/null || true

    # Copy result from Wine filesystem to Unix results dir
    local wine_unix_file="${WINE_RESULTS_UNIX}/${run_name}.txt"
    if [ -f "$wine_unix_file" ]; then
        cp "$wine_unix_file" "$result_file"
    fi

    # Also check if it wrote to CWD (game directory) with default filename
    if [ ! -f "$result_file" ] && [ -f "$GAMEDIR/diff_test_results.txt" ]; then
        cp "$GAMEDIR/diff_test_results.txt" "$result_file"
        rm -f "$GAMEDIR/diff_test_results.txt"
    fi

    # Check result
    if [ -f "$result_file" ]; then
        local hash=$(grep "^FINAL_HASH=" "$result_file" 2>/dev/null | cut -d= -f2)
        local status=$(grep "^STATUS=" "$result_file" 2>/dev/null | cut -d= -f2)
        echo "  ${run_name}: ${status} hash=${hash}"
    else
        echo "  ${run_name}: NO RESULT (no results file)"
        # Check for diff_test.log in game dir for debugging
        if [ -f "$GAMEDIR/diff_test.log" ]; then
            echo "  diff_test.log:"
            cat "$GAMEDIR/diff_test.log"
        fi
        if [ -f "$WINEPREFIX/drive_c/soak_test.log" ]; then
            echo "  soak_test.log:"
            tail -5 "$WINEPREFIX/drive_c/soak_test.log"
        fi
        echo "STATUS=CRASH" > "$result_file"
        echo "FINAL_HASH=0000000000000000" >> "$result_file"
    fi
}

get_hash() {
    local file="$1"
    grep "^FINAL_HASH=" "$file" 2>/dev/null | cut -d= -f2
}

get_status() {
    local file="$1"
    grep "^STATUS=" "$file" 2>/dev/null | cut -d= -f2
}

# ---------------------------------------------------------------------------
# Main test flow
# ---------------------------------------------------------------------------

echo "=== Phase 4: Differential Test Harness ==="
echo "Warmup: ${WARMUP_FRAMES} frames, Measure: ${MEASURE_FRAMES} frames"
echo "Game timeout: ${GAME_TIMEOUT}s"
echo ""

setup_wine_prefix
setup_game_dir
start_xvfb

mkdir -p "$RESULTS_DIR"
mkdir -p "$WINE_RESULTS_UNIX"

# --- Phase 0: Warmup runs (primes Wine prefix, filesystem cache, gta_sa.set) ---
WARMUP_RUNS="${WARMUP_RUNS:-3}"
echo "--- Phase 0: Warmup runs (${WARMUP_RUNS}x cold-start priming) ---"
for i in $(seq 1 "$WARMUP_RUNS"); do
    run_game "warmup_${i}"
done

# Collect hooks.csv — generated by ReversibleHooks::OnInjectionEnd() during DllMain.
# Written to the game's CWD (game directory) by std::ofstream.
HOOKS_CSV="$GAMEDIR/hooks.csv"
if [ -f "$HOOKS_CSV" ]; then
    cp "$HOOKS_CSV" "$RESULTS_DIR/hooks.csv"
    HOOK_COUNT=$(tail -n +2 "$HOOKS_CSV" | wc -l)
    CATEGORY_COUNT=$(tail -n +2 "$HOOKS_CSV" | cut -d, -f1 | sort -u | wc -l)
    echo "  hooks.csv collected: ${HOOK_COUNT} hooks in ${CATEGORY_COUNT} categories"
else
    echo "  WARNING: hooks.csv not found in $GAMEDIR"
    # Also check Wine C:\ drive
    if [ -f "$WINEPREFIX/drive_c/hooks.csv" ]; then
        cp "$WINEPREFIX/drive_c/hooks.csv" "$RESULTS_DIR/hooks.csv"
        HOOKS_CSV="$RESULTS_DIR/hooks.csv"
        echo "  Found hooks.csv in Wine C:\\ — collected"
    fi
fi

# Collect hooks_paths.csv — generated by differential_test.cpp during APPLY_CONFIG.
# Contains full category paths needed for SetCategoryOrItemStateByPath.
HOOKS_PATHS="$GAMEDIR/hooks_paths.csv"
if [ -f "$HOOKS_PATHS" ]; then
    cp "$HOOKS_PATHS" "$RESULTS_DIR/hooks_paths.csv"
    PATH_COUNT=$(tail -n +2 "$HOOKS_PATHS" | cut -d, -f1 | sort -u | wc -l)
    echo "  hooks_paths.csv collected: ${PATH_COUNT} unique category paths"
else
    echo "  WARNING: hooks_paths.csv not found — Phase 3 may be limited"
fi
echo ""

# --- Phase 1: Baseline runs (all hooks enabled) ---
echo "--- Phase 1: Baseline runs (${BASELINE_RUNS}x) ---"
BASELINE_HASHES=()
for i in $(seq 1 "$BASELINE_RUNS"); do
    run_game "baseline_${i}"
    BASELINE_HASHES+=("$(get_hash "${RESULTS_DIR}/baseline_${i}.txt")")
done

# Check baseline consistency
BASELINE_HASH="${BASELINE_HASHES[0]}"
BASELINE_CONSISTENT=true
for h in "${BASELINE_HASHES[@]}"; do
    if [ "$h" != "$BASELINE_HASH" ]; then
        BASELINE_CONSISTENT=false
        break
    fi
done

if [ "$BASELINE_CONSISTENT" = true ]; then
    echo "  Baseline consistent: hash=$BASELINE_HASH"
else
    echo "  WARNING: Baseline hashes differ (non-determinism detected)"
    echo "  Hashes: ${BASELINE_HASHES[*]}"
    echo "  Using first run as reference"
fi
echo ""

# --- Phase 2: All-disabled run (original game code) ---
echo "--- Phase 2: All hooks disabled (original code) ---"
run_game "all_disabled" "" "1"
ALL_DISABLED_HASH=$(get_hash "${RESULTS_DIR}/all_disabled.txt")

if [ "$ALL_DISABLED_HASH" = "$BASELINE_HASH" ]; then
    echo "  WARNING: All-disabled hash matches baseline!"
    echo "  This means the test can't distinguish reversed from original code."
    echo "  Possible causes: hashed regions not affected, or non-determinism."
else
    echo "  All-disabled diverges from baseline (good — test is sensitive)"
fi
echo ""

# --- Phase 3: Category-level testing ---
echo "--- Phase 3: Category-level testing ---"
# Use hooks_paths.csv which has full category paths (needed by SetCategoryOrItemStateByPath).
# Test at top-level category granularity first (e.g., "Global", "Audio", "Entity").
# hooks_paths.csv format: path,class,fn_name,reversed,locked
HOOKS_PATHS_COLLECTED="$RESULTS_DIR/hooks_paths.csv"
if [ -f "$HOOKS_PATHS_COLLECTED" ]; then
    # Extract top-level categories (first component of the path)
    TOP_CATEGORIES=$(tail -n +2 "$HOOKS_PATHS_COLLECTED" | cut -d, -f1 | cut -d/ -f1 | sort -u)
    CAT_COUNT=$(echo "$TOP_CATEGORIES" | grep -c .)
    echo "  Found ${CAT_COUNT} top-level categories to test"

    for category in $TOP_CATEGORIES; do
        [ -z "$category" ] && continue
        # Sanitize category name for filename (replace / with _)
        safe_name=$(echo "$category" | tr '/' '_')
        run_game "cat_${safe_name}" "$category"
    done
else
    echo "  No hooks_paths.csv available, skipping category testing"
    echo "  (hooks_paths.csv is generated by differential_test.cpp during warmup)"
fi
echo ""

# --- Phase 4: Summary ---
echo "--- Results Summary ---"
PASS=0
FAIL=0
CRASH=0
DIVERGENT_CATEGORIES=""

for result_file in "${RESULTS_DIR}"/cat_*.txt; do
    [ -f "$result_file" ] || continue
    name=$(basename "$result_file" .txt | sed 's/^cat_//')
    status=$(get_status "$result_file")
    hash=$(get_hash "$result_file")

    if [ "$status" = "CRASH" ]; then
        echo "  CRASH:  $name"
        CRASH=$((CRASH + 1))
    elif [ "$hash" != "$BASELINE_HASH" ]; then
        echo "  DIFFER: $name (hash=$hash)"
        FAIL=$((FAIL + 1))
        DIVERGENT_CATEGORIES="$DIVERGENT_CATEGORIES $name"
    else
        PASS=$((PASS + 1))
    fi
done

echo ""
echo "Categories: ${PASS} match, ${FAIL} diverge, ${CRASH} crash"
if [ -n "$DIVERGENT_CATEGORIES" ]; then
    echo "Divergent:${DIVERGENT_CATEGORIES}"
fi

# --- Write JSON report ---
REPORT_FILE="${RESULTS_DIR}/differential_test_results.json"
cat > "$REPORT_FILE" << JSONEOF
{
    "version": 1,
    "baseline_hash": "${BASELINE_HASH}",
    "baseline_consistent": ${BASELINE_CONSISTENT},
    "all_disabled_hash": "${ALL_DISABLED_HASH}",
    "test_sensitive": $([ "$ALL_DISABLED_HASH" != "$BASELINE_HASH" ] && echo "true" || echo "false"),
    "categories_pass": ${PASS},
    "categories_fail": ${FAIL},
    "categories_crash": ${CRASH},
    "divergent_categories": [$(echo "$DIVERGENT_CATEGORIES" | sed 's/ /\n/g' | grep -v '^$' | sed 's/.*/"&"/' | paste -sd, -)]
}
JSONEOF

echo ""
echo "Report: $REPORT_FILE"
cat "$REPORT_FILE"

# Copy results to build output if available
if [ -d /build ]; then
    cp -r "$RESULTS_DIR" /build/diff_results 2>/dev/null || true
    # Also copy hook CSVs to build output for external tooling
    for csv in hooks.csv hooks_paths.csv; do
        [ -f "$RESULTS_DIR/$csv" ] && cp "$RESULTS_DIR/$csv" "/build/$csv" 2>/dev/null || true
    done
    echo "Results copied to /build/diff_results/"
fi

echo ""
echo "=== Differential test complete ==="
