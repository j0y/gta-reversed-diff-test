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
            s_phase = RunnerPhase::WARMUP;
        }
        break;

    case RunnerPhase::WARMUP:
        // Let the game run to populate world with ambient peds/vehicles
        if (s_warmupFrames > 0) {
            s_warmupFrames--;
            return; // let game process this frame normally
        }
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

        // GAME_TEST_FILTER: if set, only run tests whose "Class/Name" contains this substring
        const char* filter = getenv("GAME_TEST_FILTER");
        int skipped = 0;

        for (auto& test : tests) {
            // Apply filter
            if (filter && filter[0]) {
                char fullName[256];
                _snprintf(fullName, sizeof(fullName), "%s/%s", test.className, test.testName);
                fullName[sizeof(fullName) - 1] = '\0';
                if (!strstr(fullName, filter)) {
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
