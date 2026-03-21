// differential_test.cpp — Differential test harness for gta-reversed.
// Compiled into gta_reversed.asi via build-time patching.
//
// Compares game behavior with reversed hooks enabled vs disabled by hashing
// observable game state over multiple frames. Controlled via environment variables.
//
// Entry point: DiffTestOnFrame() — called from SoakTestOnFrame() when
// DIFF_TEST_ENABLE=1 is set.

#include "StdInc.h"
#include "reversiblehooks/RootHookCategory.h"
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <random>

// ---------------------------------------------------------------------------
// FNV-1a hash (fast, non-cryptographic)
// ---------------------------------------------------------------------------

static uint64_t fnv1a_init() { return 0xcbf29ce484222325ULL; }

static uint64_t fnv1a_feed(uint64_t hash, const void* data, size_t len) {
    auto* p = (const uint8_t*)data;
    for (size_t i = 0; i < len; i++) {
        hash ^= p[i];
        hash *= 0x100000001b3ULL;
    }
    return hash;
}

// ---------------------------------------------------------------------------
// Game state addresses (same as soak_test.cpp)
// ---------------------------------------------------------------------------

static constexpr uintptr_t ADDR_GAME_STATE    = 0xC8D4C0;
static constexpr int       GAME_STATE_PLAYING  = 9;
static constexpr int       GAME_STATE_FRONTEND = 7;

// Regions to hash each frame (~900 bytes total)
struct HashRegion {
    uintptr_t addr;
    size_t    size;
    const char* name;
};

static constexpr HashRegion HASH_REGIONS[] = {
    { 0xB7CB4C, 64,  "CTimer" },          // frame counter, time, timestep
    { 0xC81320, 256, "CWeather" },         // weather state
    { 0xB70148, 16,  "CClock" },           // game hours/minutes/days
    { 0xB6F028, 64,  "TheCamera" },        // camera matrix
    { 0xB7CD98, 512, "CPlayerInfo" },      // player info (money, wanted, etc.)
};

// ---------------------------------------------------------------------------
// Configuration (from environment variables)
// ---------------------------------------------------------------------------

static int         s_warmupFrames  = 300;
static int         s_measureFrames = 100;
static const char* s_resultsFile   = "diff_test_results.txt";
static const char* s_disableHooks  = nullptr;
static bool        s_disableAll    = false;

static void ReadConfig() {
    if (auto* v = getenv("DIFF_TEST_WARMUP_FRAMES"))  s_warmupFrames  = atoi(v);
    if (auto* v = getenv("DIFF_TEST_MEASURE_FRAMES")) s_measureFrames = atoi(v);
    if (auto* v = getenv("DIFF_TEST_RESULTS_FILE"))    s_resultsFile   = v;
    if (auto* v = getenv("DIFF_TEST_DISABLE_ALL"))     s_disableAll    = (strcmp(v, "1") == 0);
    s_disableHooks = getenv("DIFF_TEST_DISABLE_HOOKS");
}

// ---------------------------------------------------------------------------
// Logging (same pattern as soak_test.cpp)
// ---------------------------------------------------------------------------

static FILE* s_logFile = nullptr;

static void DiffLog(const char* fmt, ...) {
    char buf[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    OutputDebugStringA(buf);
    OutputDebugStringA("\n");

    if (!s_logFile) {
        s_logFile = fopen("diff_test.log", "w");
    }
    if (s_logFile) {
        fprintf(s_logFile, "%s\n", buf);
        fflush(s_logFile);
    }
}

// ---------------------------------------------------------------------------
// Deterministic state reset
// ---------------------------------------------------------------------------

// CTimer addresses (from Timer.h StaticRef declarations)
static constexpr uintptr_t ADDR_TIMER_FRAME_COUNTER     = 0xB7CB4C;
static constexpr uintptr_t ADDR_TIMER_OLD_TIMESTEP      = 0xB7CB54;
static constexpr uintptr_t ADDR_TIMER_TIMESTEP_NC       = 0xB7CB58;
static constexpr uintptr_t ADDR_TIMER_TIMESTEP          = 0xB7CB5C;
static constexpr uintptr_t ADDR_TIMER_TIMESCALE         = 0xB7CB64;
static constexpr uintptr_t ADDR_TIMER_PPPP_TIME         = 0xB7CB6C;
static constexpr uintptr_t ADDR_TIMER_PPP_TIME          = 0xB7CB70;
static constexpr uintptr_t ADDR_TIMER_PP_TIME           = 0xB7CB74;
static constexpr uintptr_t ADDR_TIMER_P_TIME            = 0xB7CB78;
static constexpr uintptr_t ADDR_TIMER_PAUSE_TIME        = 0xB7CB7C;
static constexpr uintptr_t ADDR_TIMER_TIME_NC           = 0xB7CB80;
static constexpr uintptr_t ADDR_TIMER_TIME              = 0xB7CB84;
static constexpr uintptr_t ADDR_TIMER_P_TIME_NC         = 0xB7CB68;
static constexpr uintptr_t ADDR_TIMER_FPS               = 0xB7CB50;
static constexpr uintptr_t ADDR_TIMER_ENABLE_DEBUG      = 0xB7CB40;

static void ResetTimerState() {
    // Reset all timer state to known values so measurement starts identically
    // regardless of how long init took (wall-clock varies between runs).
    constexpr uint32_t BASE_TIME = 10000; // 10 seconds into game

    *(uint32_t*)ADDR_TIMER_FRAME_COUNTER = 500;
    *(uint32_t*)ADDR_TIMER_TIME          = BASE_TIME;
    *(uint32_t*)ADDR_TIMER_TIME_NC       = BASE_TIME;
    *(uint32_t*)ADDR_TIMER_PAUSE_TIME    = BASE_TIME;
    *(uint32_t*)ADDR_TIMER_P_TIME        = BASE_TIME - 20;
    *(uint32_t*)ADDR_TIMER_PP_TIME       = BASE_TIME - 40;
    *(uint32_t*)ADDR_TIMER_PPP_TIME      = BASE_TIME - 60;
    *(uint32_t*)ADDR_TIMER_PPPP_TIME     = BASE_TIME - 80;
    *(uint32_t*)ADDR_TIMER_P_TIME_NC     = BASE_TIME - 20;

    *(float*)ADDR_TIMER_TIMESTEP         = 1.0f;
    *(float*)ADDR_TIMER_TIMESTEP_NC      = 1.0f;
    *(float*)ADDR_TIMER_OLD_TIMESTEP     = 1.0f;
    *(float*)ADDR_TIMER_TIMESCALE        = 1.0f;
    *(float*)ADDR_TIMER_FPS              = 50.0f;
    *(bool*)ADDR_TIMER_ENABLE_DEBUG      = true;

    DiffLog("[DiffTest] Timer state reset (BaseTime=%u, Frame=500)", BASE_TIME);
}

static void SeedRNGDeterministic() {
    // C library rand() — used by CGeneral::GetRandomNumber()
    srand(42);

    DiffLog("[DiffTest] RNG seeded deterministically (srand(42))");
}

static void ResetDeterministicState() {
    ResetTimerState();
    SeedRNGDeterministic();
}

// ---------------------------------------------------------------------------
// Hook configuration
// ---------------------------------------------------------------------------

// Hooks that must stay enabled for headless operation.
// These contain #ifdef NOTSA_HEADLESS stubs that prevent crashes with the null
// D3D9 device and Wine's missing audio. Without them, original game code runs
// and calls into D3D9/DirectSound which are not available headless.
static const char* HEADLESS_WHITELIST[] = {
    "Global/CGame",                   // InitialiseEssentialsAfterRW/InitialiseCoreDataAfterRW — skip renderer check, force audio success
    "Global/CLoadingScreen",          // NewChunkLoaded/LoadingScreen — called by CStreaming during gameplay
    "Audio/CAudioEngine",             // Service/InitialisePostLoading — early returns under headless
    "Audio/Hardware/CAEAudioHardware", // Initialise/Service/IsSoundBankLoaded — headless audio stubs
    "Shadows/CRealTimeShadowManager", // Init — skips shadow RW camera creation under headless
};

static void ReEnableHeadlessWhitelist() {
    for (const char* path : HEADLESS_WHITELIST) {
        auto result = ReversibleHooks::SetCategoryOrItemStateByPath(path, true);
        const char* resultStr = "???";
        switch (result) {
            case ReversibleHooks::SetCatOrItemStateResult::Done:     resultStr = "OK"; break;
            case ReversibleHooks::SetCatOrItemStateResult::NotFound: resultStr = "NOT_FOUND"; break;
            case ReversibleHooks::SetCatOrItemStateResult::Locked:   resultStr = "LOCKED"; break;
        }
        DiffLog("[DiffTest] Re-enable headless whitelist '%s' -> %s", path, resultStr);
    }
}

static void ApplyHookConfig() {
    if (s_disableAll) {
        DiffLog("[DiffTest] Disabling ALL hooks (original code baseline)");
        ReversibleHooks::GetRootCategory().SetAllItemsEnabled(false);
        ReEnableHeadlessWhitelist();
        return;
    }

    if (s_disableHooks && *s_disableHooks) {
        // Parse comma-separated hook paths
        char buf[4096];
        strncpy(buf, s_disableHooks, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        char* tok = strtok(buf, ",");
        while (tok) {
            // Trim whitespace
            while (*tok == ' ') tok++;
            char* end = tok + strlen(tok) - 1;
            while (end > tok && *end == ' ') *end-- = '\0';

            if (*tok) {
                auto result = ReversibleHooks::SetCategoryOrItemStateByPath(tok, false);
                const char* resultStr = "???";
                switch (result) {
                    case ReversibleHooks::SetCatOrItemStateResult::Done:     resultStr = "OK"; break;
                    case ReversibleHooks::SetCatOrItemStateResult::NotFound: resultStr = "NOT_FOUND"; break;
                    case ReversibleHooks::SetCatOrItemStateResult::Locked:   resultStr = "LOCKED"; break;
                }
                DiffLog("[DiffTest] Disable hook '%s' -> %s", tok, resultStr);
            }
            tok = strtok(nullptr, ",");
        }
    }
}

// ---------------------------------------------------------------------------
// Game state hashing
// ---------------------------------------------------------------------------

static uint64_t HashGameState() {
    uint64_t h = fnv1a_init();
    for (const auto& region : HASH_REGIONS) {
        h = fnv1a_feed(h, (const void*)region.addr, region.size);
    }
    return h;
}

// ---------------------------------------------------------------------------
// State machine
// ---------------------------------------------------------------------------

enum class Phase {
    WAIT_GAME,    // Wait for gGameState >= GAME_STATE_FRONTEND
    APPLY_CONFIG, // Read env vars, disable hooks
    WARMUP,       // Run warmup frames (streaming stabilization)
    MEASURE,      // Hash game state each frame
    DONE          // Write results and exit
};

static Phase    s_phase      = Phase::WAIT_GAME;
static int      s_frameCount = 0;
static clock_t  s_wallStart  = 0;

// Frame hash storage (heap-allocated to avoid large static array)
static uint64_t* s_frameHashes = nullptr;
static int       s_hashCount   = 0;

// ---------------------------------------------------------------------------
// Results output
// ---------------------------------------------------------------------------

static void WriteResults(const char* status) {
    FILE* f = fopen(s_resultsFile, "w");
    if (!f) {
        DiffLog("[DiffTest] ERROR: Cannot open results file '%s'", s_resultsFile);
        return;
    }

    uint32_t wallMs = (uint32_t)((double)(clock() - s_wallStart) / CLOCKS_PER_SEC * 1000.0);

    fprintf(f, "DIFF_TEST_VERSION=1\n");
    fprintf(f, "STATUS=%s\n", status);
    fprintf(f, "WARMUP_FRAMES=%d\n", s_warmupFrames);
    fprintf(f, "MEASURE_FRAMES=%d\n", s_measureFrames);
    fprintf(f, "DISABLED_HOOKS=%s\n", s_disableHooks ? s_disableHooks : "");
    fprintf(f, "DISABLE_ALL=%d\n", s_disableAll ? 1 : 0);
    fprintf(f, "WALL_TIME_MS=%u\n", wallMs);

    // Compute combined hash from all frame hashes
    uint64_t combined = fnv1a_init();
    for (int i = 0; i < s_hashCount; i++) {
        combined = fnv1a_feed(combined, &s_frameHashes[i], sizeof(uint64_t));
    }
    fprintf(f, "FINAL_HASH=%016llx\n", (unsigned long long)combined);

    // Per-frame hashes
    fprintf(f, "FRAME_HASHES=");
    for (int i = 0; i < s_hashCount; i++) {
        if (i > 0) fprintf(f, ",");
        fprintf(f, "%016llx", (unsigned long long)s_frameHashes[i]);
    }
    fprintf(f, "\n");

    fclose(f);
    DiffLog("[DiffTest] Results written to '%s' (FINAL_HASH=%016llx)", s_resultsFile, (unsigned long long)combined);
}

// ---------------------------------------------------------------------------
// Generate hooks_paths.csv — full category paths for Phase 3 testing
// ---------------------------------------------------------------------------

static void WriteCategoryPaths(FILE* f, const ReversibleHooks::HookCategory& cat, const char* parentPath) {
    char fullPath[512];
    if (parentPath[0]) {
        snprintf(fullPath, sizeof(fullPath), "%s/%s", parentPath, cat.Name().c_str());
    } else {
        snprintf(fullPath, sizeof(fullPath), "%s", cat.Name().c_str());
    }

    // Write items in this category with full path
    for (const auto& item : cat.Items()) {
        fprintf(f, "%s,%s,%s,%d,%d\n",
            fullPath,
            cat.Name().c_str(),
            item->Name().c_str(),
            (int)item->Reversed(),
            (int)item->Locked());
    }

    // Recurse into subcategories
    for (const auto& sub : cat.SubCategories()) {
        WriteCategoryPaths(f, sub, fullPath);
    }
}

static void GenerateHooksPathsCsv() {
    FILE* f = fopen("hooks_paths.csv", "w");
    if (!f) {
        DiffLog("[DiffTest] WARNING: Cannot write hooks_paths.csv");
        return;
    }
    fprintf(f, "path,class,fn_name,reversed,locked\n");
    const auto& root = ReversibleHooks::GetRootCategory();
    for (const auto& sub : root.SubCategories()) {
        WriteCategoryPaths(f, sub, "");
    }
    fclose(f);
    DiffLog("[DiffTest] hooks_paths.csv written");
}

// ---------------------------------------------------------------------------
// Entry point — called from SoakTestOnFrame() when DIFF_TEST_ENABLE is set
// ---------------------------------------------------------------------------

void DiffTestOnFrame() {
    int gameState = *(int*)ADDR_GAME_STATE;

    switch (s_phase) {
    case Phase::WAIT_GAME:
        if (gameState >= GAME_STATE_FRONTEND) {
            DiffLog("[DiffTest] Game reached state %d, configuring...", gameState);
            s_phase = Phase::APPLY_CONFIG;
        }
        break;

    case Phase::APPLY_CONFIG:
        ReadConfig();

        // Generate hooks_paths.csv with full category paths (for Phase 3 testing)
        GenerateHooksPathsCsv();

        // Allocate frame hash storage
        s_frameHashes = (uint64_t*)malloc(s_measureFrames * sizeof(uint64_t));
        if (!s_frameHashes) {
            DiffLog("[DiffTest] ERROR: malloc failed for %d frame hashes", s_measureFrames);
            WriteResults("ERROR");
            ExitProcess(1);
            return;
        }

        ApplyHookConfig();

        // Reset timer + RNG to known state so warmup+measure is deterministic.
        // Timer state accumulated non-deterministic wall-clock values during init
        // (states 0-8). This reset ensures identical starting conditions.
        ResetDeterministicState();

        s_wallStart = clock();
        s_frameCount = 0;
        s_phase = Phase::WARMUP;
        DiffLog("[DiffTest] Warming up (%d frames)...", s_warmupFrames);
        break;

    case Phase::WARMUP:
        s_frameCount++;
        if (s_frameCount >= s_warmupFrames) {
            // Re-reset at measurement boundary so any streaming-order variance
            // during warmup doesn't affect the measurement phase
            ResetDeterministicState();

            s_frameCount = 0;
            s_hashCount = 0;
            s_phase = Phase::MEASURE;
            DiffLog("[DiffTest] Measuring (%d frames)...", s_measureFrames);
        }
        break;

    case Phase::MEASURE:
        s_frameHashes[s_hashCount++] = HashGameState();
        s_frameCount++;
        if (s_frameCount >= s_measureFrames) {
            s_phase = Phase::DONE;
        }
        break;

    case Phase::DONE:
        WriteResults("PASS");
        if (s_frameHashes) { free(s_frameHashes); s_frameHashes = nullptr; }
        if (s_logFile) { fclose(s_logFile); s_logFile = nullptr; }
        ExitProcess(0);
        break;
    }
}
