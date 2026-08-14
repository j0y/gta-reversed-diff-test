// game_tests.cpp — Test runner for gta-reversed scenario tests.
//
// Compiled into gta_reversed.asi via build-time patching.
// Triggered by GAME_TEST_ENABLE=1 environment variable.
//
// Tests are defined in headless_stubs/tests/*.cpp using GAME_TEST/GAME_DIFF_TEST macros.
// They register via static initialization and are discovered automatically.
//
// Entry point: GameTestRunnerOnFrame() — called from SoakTestOnFrame().

#include "StdInc.h"
#include "TestFramework.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ===================================================================
// Test runner
// ===================================================================

static FILE* s_resultFile = nullptr;



static void TestLog(const char* fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    OutputDebugStringA(buf);
    OutputDebugStringA("\n");

    if (!s_resultFile) {
        const char* path = getenv("GAME_TEST_RESULTS_FILE");
        s_resultFile = fopen(path ? path : "game_test_results.txt", "w");
    }
    if (s_resultFile) {
        fprintf(s_resultFile, "%s\n", buf);
        fflush(s_resultFile);
    }
}

// ===================================================================
// Model pre-loading (WARMUP phase only)
// ===================================================================
//
// GAME_TEST_REQUEST_MODELS=481,461,...  -- ask the streamer for these model IDs
// at the START of warmup, then let the warmup frames drive the load.
//
// This exists because tests run inside SuspendOtherThreads (see RUN_TESTS
// below), which parks CdStreamThread. A blocking LoadAllRequestedModels() from
// within a test therefore never returns -- the loader waits for streaming
// channels that nothing is pumping. Requesting here instead costs nothing: the
// frames were already going to elapse.
//
// Deliberately does NOT block or verify. A model that fails to arrive is the
// test's problem to notice and report, not this hook's to force.
static void RequestTestModels() {
    const char* env = getenv("GAME_TEST_REQUEST_MODELS");
    if (!env || !env[0]) {
        return;
    }
    char buf[512];
    strncpy(buf, env, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    int requested = 0;
    for (char* tok = strtok(buf, ","); tok; tok = strtok(nullptr, ",")) {
        while (*tok == ' ') tok++;
        const int id = atoi(tok);
        if (id <= 0) {
            continue;
        }
        if (!CStreaming::IsModelLoaded(id)) {
            // GAME_REQUIRED only. KEEP_IN_MEMORY would pin these for the whole
            // run and perturb later tests' streaming budget.
            CStreaming::RequestModel(id, STREAMING_GAME_REQUIRED);
            requested++;
        }
    }
    TestLog("GAME_TEST_REQUEST_MODELS: requested %d model(s)", requested);
}

// SEH wrapper — must be in a separate function from C++ destructors (MSVC limitation)
static bool RunSingleTest(void (*fn)()) {
    __try {
        fn();
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        GetTestContext().RecordFailure("(unknown)", 0, "EXCEPTION during test execution");
        return true; // crashed
    }
    return false;
}

enum class RunnerPhase {
    WAIT_GAME,
    WARMUP,
    RUN_TESTS,
    DONE
};

static RunnerPhase s_phase = RunnerPhase::WAIT_GAME;
static int s_warmupFrames = 0;

void GameTestRunnerOnFrame() {
    int gameState = *(int*)0xC8D4C0; // gGameState

    switch (s_phase) {
    case RunnerPhase::WAIT_GAME:
        if (gameState >= GAME_STATE_IDLE) {
            // Get warmup frame count from env (default 100 — enough for population to spawn)
            const char* envFrames = getenv("GAME_TEST_WARMUP_FRAMES");
            s_warmupFrames = envFrames ? atoi(envFrames) : 100;
            // Ask for any test-required models NOW, so the warmup frames below
            // stream them in while CdStreamThread is still running.
            RequestTestModels();
            s_phase = RunnerPhase::WARMUP;
        }
        break;

    case RunnerPhase::WARMUP:
        // Let the game run to populate world with ambient peds/vehicles
        if (s_warmupFrames > 0) {
            s_warmupFrames--;
            // Pump the streamer explicitly. The headless frame loop does not
            // drive CStreaming's on-demand queue (the log shows only Init /
            // InitImageList, never Update / LoadRequestedModels), so models
            // asked for by RequestTestModels() sit requested-but-unloaded no
            // matter how many warmup frames elapse. Servicing it here is safe:
            // CdStreamThread is still running during WARMUP -- the deadlock the
            // RequestTestModels comment warns about only occurs INSIDE tests,
            // after SuspendOtherThreads parks that thread. Same call the game
            // itself uses (Pools.cpp:224, Game.cpp:459).
            CStreaming::LoadRequestedModels();
            return; // let game process this frame normally
        }
        // Final flush: block until everything still queued is resident, so the
        // tests see a fully-loaded set rather than a partially-streamed one.
        CStreaming::LoadAllRequestedModels(false);
        s_phase = RunnerPhase::RUN_TESTS;
        break;

    case RunnerPhase::RUN_TESTS: {
        auto& tests = GameTestRegistry::Tests();
        int total = (int)tests.size();
        int passed = 0;
        int failed = 0;

        // Suspend background threads (CdStream, etc.) to prevent state changes
        // between original and reversed calls in differential tests.
        SuspendOtherThreads threadGuard;

        TestLog("GAME_TEST_VERSION=1");
        TestLog("TOTAL_TESTS=%d", total);
        TestLog("");

        // GAME_TEST_FILTER: comma-separated list of substrings to match against "Class/Name".
        // A test runs if ANY token matches (OR logic). Single token works as before.
        // Examples:
        //   GAME_TEST_FILTER=CVector                       (single class)
        //   GAME_TEST_FILTER=CVector,CGeneral,CPed2        (multiple classes)
        //   GAME_TEST_FILTER=CVector/Magnitude             (specific test)
        const char* filter = getenv("GAME_TEST_FILTER");
        int skipped = 0;

        // Pre-parse comma-separated filter tokens (max 64)
        const char* filterTokens[64] = {};
        char filterBuf[1024] = {};
        int numTokens = 0;
        if (filter && filter[0]) {
            strncpy(filterBuf, filter, sizeof(filterBuf) - 1);
            char* tok = strtok(filterBuf, ",");
            while (tok && numTokens < 64) {
                // Trim leading spaces
                while (*tok == ' ') tok++;
                if (*tok) filterTokens[numTokens++] = tok;
                tok = strtok(nullptr, ",");
            }
        }

        for (auto& test : tests) {
            // Apply filter — test runs if any token matches
            if (numTokens > 0) {
                char fullName[256];
                _snprintf(fullName, sizeof(fullName), "%s/%s", test.className, test.testName);
                fullName[sizeof(fullName) - 1] = '\0';
                bool matched = false;
                for (int t = 0; t < numTokens; t++) {
                    if (strstr(fullName, filterTokens[t])) { matched = true; break; }
                }
                if (!matched) {
                    skipped++;
                    continue;
                }
            }

            auto& ctx = GetTestContext();
            ctx.Reset(test.className, test.testName);

            // Run test in separate function to allow SEH (__try can't coexist
            // with C++ destructors like SuspendOtherThreads in the same function)
            bool crashed = RunSingleTest(test.fn);

            if (ctx.failures == 0 && !crashed) {
                passed++;
                TestLog("  PASS  %s/%s (%d assertions)", test.className, test.testName, ctx.assertions);
            } else {
                failed++;
                TestLog("  FAIL  %s/%s: %s", test.className, test.testName, ctx.firstFailure);
            }
        }
        if (skipped > 0) {
            TestLog("SKIPPED=%d (filter: %s)", skipped, filter);
        }

        TestLog("");
        TestLog("PASSED=%d", passed);
        TestLog("FAILED=%d", failed);
        TestLog("STATUS=%s", failed == 0 ? "PASS" : "FAIL");

        if (s_resultFile) { fclose(s_resultFile); s_resultFile = nullptr; }
        s_phase = RunnerPhase::DONE;
        ExitProcess(failed == 0 ? 0 : 1);
        break;
    }

    case RunnerPhase::DONE:
        break;
    }
}
