// soak_test.cpp — Runs the game for N frames headlessly and reports results.
// Compiled into gta_reversed.asi via build-time patching.
//
// Injection point: Render2dStuff (0x53E230) is already stubbed to no-op by
// headless_render_stubs.cpp. We replace that stub with our frame counter,
// which runs once per frame after CGame::Process().

#include "StdInc.h"
#include <cstdio>
#include <ctime>

// Log to both file and OutputDebugString (visible via Wine debug channels)
static FILE* s_logFile = nullptr;
static void SoakLog(const char* fmt, ...) {
    char buf[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    OutputDebugStringA(buf);
    OutputDebugStringA("\n");

    if (!s_logFile) {
        // Use absolute path so it's findable regardless of CWD
        s_logFile = fopen("C:\\soak_test.log", "w");
    }
    if (s_logFile) {
        fprintf(s_logFile, "%s\n", buf);
        fflush(s_logFile);
    }
}

// ---------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------

static constexpr int    DEFAULT_TARGET_FRAMES = 1000;
static constexpr int    PROGRESS_INTERVAL     = 100;
// Write results to game directory (current working dir when launched)
static constexpr auto   RESULTS_FILE          = "C:\\soak_test_results.txt";

// Game state address (gGameState)
static constexpr auto   ADDR_GAME_STATE       = 0xC8D4C0;
// Game states we consider "running" for the soak test
static constexpr int    SOAK_GAME_STATE_IDLE           = 9;  // In-game
static constexpr int    SOAK_GAME_STATE_FRONTEND_IDLE  = 7;  // Main menu

// CTimer addresses
static constexpr auto   ADDR_TIME_MS          = 0xB7CB84; // m_snTimeInMilliseconds
static constexpr auto   ADDR_TIME_STEP        = 0xB7CB5C; // ms_fTimeStep
static constexpr auto   ADDR_FRAME_COUNTER    = 0xB7CB4C; // m_FrameCounter

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

static int     s_targetFrames = DEFAULT_TARGET_FRAMES;
static int     s_frameCount   = 0;
static bool    s_gameReady    = false;
static clock_t s_wallStart    = 0;

// ---------------------------------------------------------------------------
// Frame callback — called once per frame from Render2dStuff hook
// ---------------------------------------------------------------------------

// Forward declarations
extern void DiffTestOnFrame();         // differential_test.cpp
extern void GameTestRunnerOnFrame();   // game_tests.cpp

void SoakTestOnFrame() {
    // Dispatch to test harnesses based on environment variables
    static bool s_dispatchChecked = false;
    static bool s_isDiffTest = false;
    static bool s_isGameTest = false;
    if (!s_dispatchChecked) {
        s_dispatchChecked = true;
        s_isDiffTest = (getenv("DIFF_TEST_ENABLE") != nullptr);
        s_isGameTest = (getenv("GAME_TEST_ENABLE") != nullptr);
        if (s_isDiffTest) {
            OutputDebugStringA("[SoakTest] DIFF_TEST_ENABLE set, dispatching to DiffTestOnFrame\n");
        } else if (s_isGameTest) {
            OutputDebugStringA("[SoakTest] GAME_TEST_ENABLE set, dispatching to GameTestRunnerOnFrame\n");
        }
    }
    if (s_isDiffTest) {
        DiffTestOnFrame();
        return;
    }
    if (s_isGameTest) {
        GameTestRunnerOnFrame();
        return;
    }

    // Log game state periodically for debugging
    int gameState = *(int*)ADDR_GAME_STATE;
    static int s_debugCount = 0;
    if (s_debugCount < 20 || (s_debugCount % 500 == 0)) {
        char dbg[128];
        _snprintf(dbg, sizeof(dbg), "[SoakTest] frame_call=%d gGameState=%d", s_debugCount, gameState);
        SoakLog(dbg);
    }
    s_debugCount++;

    // Accept any state >= FRONTEND_IDLE (7) — game is initialized enough
    if (gameState < SOAK_GAME_STATE_FRONTEND_IDLE) {
        return;
    }

    // First gameplay frame
    if (!s_gameReady) {
        s_gameReady = true;
        s_wallStart = clock();

        // Read target frames from environment (set via gta-reversed.ini or hardcoded)
        const char* envFrames = getenv("SOAK_TEST_FRAMES");
        if (envFrames) {
            int n = atoi(envFrames);
            if (n > 0) s_targetFrames = n;
        }

        SoakLog("[SoakTest] Game reached state %d. Running %d frames...", gameState, s_targetFrames);
    }

    s_frameCount++;

    // Read game timing
    uint32_t gameTimeMs = *(uint32_t*)ADDR_TIME_MS;
    float    timeStep   = *(float*)ADDR_TIME_STEP;
    uint32_t engineFrame = *(uint32_t*)ADDR_FRAME_COUNTER;

    // Periodic progress
    if (s_frameCount % PROGRESS_INTERVAL == 0) {
        double wallSec = (double)(clock() - s_wallStart) / CLOCKS_PER_SEC;
        SoakLog("[SoakTest] Frame %d/%d | GameTime=%ums | TimeStep=%.3f | EngineFrame=%u | Wall=%.1fs",
               s_frameCount, s_targetFrames, gameTimeMs, timeStep, engineFrame, wallSec);
    }

    // Target reached
    if (s_frameCount >= s_targetFrames) {
        double wallSec = (double)(clock() - s_wallStart) / CLOCKS_PER_SEC;
        uint32_t wallMs = (uint32_t)(wallSec * 1000.0);

        SoakLog("[SoakTest] PASSED: %d frames in %.1fs (%.0f fps) | GameTime=%ums",
               s_frameCount, wallSec,
               wallSec > 0 ? s_frameCount / wallSec : 0,
               gameTimeMs);

        // Write results file
        FILE* f = fopen(RESULTS_FILE, "w");
        if (f) {
            fprintf(f, "SOAK_TEST_RESULT=PASS\n");
            fprintf(f, "FRAMES=%d\n", s_frameCount);
            fprintf(f, "GAME_TIME_MS=%u\n", gameTimeMs);
            fprintf(f, "WALL_TIME_MS=%u\n", wallMs);
            fprintf(f, "FPS=%.1f\n", wallSec > 0 ? s_frameCount / wallSec : 0);
            fclose(f);
        }

        // Clean exit
        ExitProcess(0);
    }
}
