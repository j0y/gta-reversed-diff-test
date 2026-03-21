#!/bin/bash
set -euo pipefail

# Build the Docker image and optionally run a command in it
#
# Usage:
#   ./scripts/docker-build.sh              # Build Docker image only
#   ./scripts/docker-build.sh test         # Build image + run toolchain tests
#   ./scripts/docker-build.sh build        # Build image + full build gta-reversed
#   ./scripts/docker-build.sh build-tests  # Incremental rebuild (tests/stubs only, ~30s)
#   ./scripts/docker-build.sh shell        # Build image + interactive shell

cd "$(dirname "$0")/.."
PROJECT_ROOT="$(pwd)"

IMAGE_NAME="gta-reversed-build"
CONTAINER_BUILD_DIR="${PROJECT_ROOT}/build-output"
BUILD_TREE_VOLUME="gta-build-tree"

ACTION="${1:-}"

# Skip Docker image rebuild for incremental builds (saves ~5s)
if [ "${ACTION}" != "build-tests" ]; then
    echo "=== Building Docker image: ${IMAGE_NAME} ==="
    docker build -f docker/Dockerfile.build -t "${IMAGE_NAME}" .
fi

if [ -z "${ACTION}" ]; then
    echo "Docker image built. Run with: test | build | build-tests | shell"
    exit 0
fi

mkdir -p "${CONTAINER_BUILD_DIR}"

# Common mounts for all build commands
COMMON_MOUNTS="\
    -v ${PROJECT_ROOT}/gta-reversed:/src/gta-reversed:ro \
    -v ${PROJECT_ROOT}/headless_stubs:/src/headless_stubs:ro \
    -v ${PROJECT_ROOT}/patches:/src/patches:ro \
    -v ${CONTAINER_BUILD_DIR}:/build"

# Full builds persist the build tree for later incremental use
FULL_BUILD_MOUNTS="${COMMON_MOUNTS} \
    -v ${BUILD_TREE_VOLUME}:/tmp/gta-build"

case "${ACTION}" in
    test)
        echo ""
        echo "=== Running toolchain tests ==="
        docker run --rm ${COMMON_MOUNTS} ${IMAGE_NAME} /scripts/test-toolchain.sh
        ;;
    build)
        echo ""
        echo "=== Building gta-reversed (full) ==="
        docker run --rm ${FULL_BUILD_MOUNTS} \
            -v conan-cache:/root/.conan2 \
            ${IMAGE_NAME} /scripts/build.sh
        ;;
    build-tests)
        echo ""
        echo "=== Building gta-reversed (incremental — tests only) ==="
        docker run --rm ${FULL_BUILD_MOUNTS} \
            -v conan-cache:/root/.conan2 \
            -v ${PROJECT_ROOT}/scripts/build-tests.sh:/scripts/build-tests.sh:ro \
            ${IMAGE_NAME} /scripts/build-tests.sh
        ;;
    shell)
        echo ""
        echo "=== Interactive shell ==="
        docker run --rm -it ${FULL_BUILD_MOUNTS} \
            ${IMAGE_NAME} /bin/bash
        ;;
    *)
        echo "Unknown action: ${ACTION}"
        echo "Usage: $0 [test|build|build-tests|shell]"
        exit 1
        ;;
esac
