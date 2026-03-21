#!/bin/bash
set -euo pipefail

# Run headless GTA:SA in Docker
# Usage:
#   ./scripts/run.sh              # Run with default 90s timeout
#   ./scripts/run.sh 120          # Run with custom timeout
#   ./scripts/run.sh logs         # Just dump logs from last run

cd "$(dirname "$0")/.."
PROJECT_ROOT="$(pwd)"

IMAGE_NAME="gta-reversed-build"
LOGS_DIR="/tmp/wine-logs"

if [ "${1:-}" = "logs" ]; then
    echo "=== Wine log files ==="
    for f in "${LOGS_DIR}"/*.log; do
        [ -f "$f" ] || continue
        echo "--- $(basename "$f") ---"
        cat "$f"
        echo ""
    done
    exit 0
fi

TIMEOUT="${1:-90}"
mkdir -p "${LOGS_DIR}"

echo "=== Running headless GTA:SA (timeout=${TIMEOUT}s) ==="
docker run --rm \
    -v "${PROJECT_ROOT}/GTASA:/game:ro" \
    -v "${PROJECT_ROOT}/build-output:/build:ro" \
    -v "${PROJECT_ROOT}/gta_sa_compact.exe:/gamebin/gta_sa_compact.exe:ro" \
    -v "${PROJECT_ROOT}/scripts:/scripts:ro" \
    -v "${PROJECT_ROOT}/configs:/configs:ro" \
    -v "${LOGS_DIR}:/tmp/wine-logs" \
    -e TIMEOUT="${TIMEOUT}" \
    "${IMAGE_NAME}" bash -c '/scripts/run-headless.sh 2>&1; cp /opt/wine-gtasa/drive_c/*.log /tmp/wine-logs/ 2>/dev/null; true'

echo ""
echo "=== Key logs ==="
for log in scripts.log state5.log pgl.log; do
    if [ -f "${LOGS_DIR}/${log}" ]; then
        echo "--- ${log} ---"
        cat "${LOGS_DIR}/${log}"
        echo ""
    fi
done
