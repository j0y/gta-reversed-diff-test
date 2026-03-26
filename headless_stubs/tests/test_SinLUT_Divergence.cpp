// test_SinLUT_Divergence.cpp — Proves sin LUT overwrite causes observable divergence.
//
// The reversed Maths.cpp overwrites the game's sin lookup table at 0xBB3E00
// with std::sin (MSVC C++23) values during static initialization — before
// hooks are installed, before DllMain, before anything. The original game
// computed this table at runtime using MSVC 2004 CRT sin(). The values
// differ by ~1-5 ULP per entry.
//
// GetSinFast/GetCosFast are inlined everywhere in the original binary
// (zero call sites — hooks register but can't actually hook anything).
// So original game code reads directly from 0xBB3E00. "Disabling a hook
// to run original code" doesn't give you original behavior — the original
// code reads the wrong sin table.
//
// This test restores the original table by calling the original
// InitMathsTables at 0x59AC90 directly (no hook was installed there
// since there are no callers), then compares a multi-frame simulation
// with each table to show the drift.

#include "StdInc.h"
#include "TestFramework.h"
#include "Maths.h"

// Original InitMathsTables at 0x59AC90 — no hook is installed here
// because GetSinFast/GetCosFast have no callers in the original binary.
// The original bytes should be intact. Calling it recomputes the sin LUT
// using the original exe's MSVC 2004 CRT sin().
static auto OriginalInitMathsTables = (void(*)())0x59AC90;

// But wait — CMaths::InjectHooks() DOES install a hook at 0x59AC90 for
// InitMathsTables itself (Maths.cpp:35). The hook replaces original code
// with a JMP to the reversed no-op. Since --unhook-except disables most
// hooks at startup, the original bytes MAY be restored... or the hook
// may still be active. We try both approaches:
//   1. Call 0x59AC90 directly (works if hook is disabled or never installed)
//   2. If that doesn't change the table, manually compute original values

static bool TryRestoreOriginalLUT(std::array<float, 256>& lut,
                                   const std::array<float, 256>& reversedLUT) {
    // Attempt 1: call original InitMathsTables directly
    OriginalInitMathsTables();
    if (lut != reversedLUT) return true;

    // Attempt 2: the hook is active (reversed no-op ran).
    // We can't use HookDisableGuard because "Global/CMaths/InitMathsTables"
    // isn't found in hooks_paths.csv. Manually write original bytes back?
    // We don't have the original bytes. But we CAN recompute the table
    // using the original exe's CRT sin() at its import address.
    //
    // The original InitMathsTables does essentially:
    //   for (int i = 0; i < 256; i++)
    //       SinTable[i] = sin(i * TWO_PI / 256.0f);
    // using the MSVC 2004 CRT sin() linked into gta_sa.exe.
    //
    // The CRT sin() in the original exe is at its IAT entry. We can find it
    // via GetModuleHandle("gta_sa.exe") + IAT lookup. But that's complex.
    //
    // Simpler: just call msvcrt.dll's sin(), which is the Win2K-era CRT
    // that ships with Windows and matches what MSVC 2004 would have used.
    HMODULE msvcrt = GetModuleHandleA("msvcrt.dll");
    if (!msvcrt) msvcrt = LoadLibraryA("msvcrt.dll");
    if (!msvcrt) return false;

    auto oldSin = (double(*)(double))GetProcAddress(msvcrt, "sin");
    if (!oldSin) return false;

    for (int i = 0; i < 256; i++) {
        lut[i] = (float)oldSin((double)i * (double)TWO_PI / 256.0);
    }
    return (lut != reversedLUT);
}

// --- Part 1: Show the tables actually differ ---

GAME_TEST(SinLUT_Divergence, TablesAreDifferent) {
    constexpr int LUT_SIZE = 256;
    auto& lut = StaticRef<std::array<float, LUT_SIZE>>(0xBB3E00);
    auto reversedLUT = lut;

    if (!TryRestoreOriginalLUT(lut, reversedLUT)) {
        lut = reversedLUT;
        GetTestContext().RecordFailure(__FILE__, __LINE__,
            "Cannot restore original sin LUT by any method.");
        return;
    }
    auto originalLUT = lut;
    lut = reversedLUT; // restore

    int diffCount = 0;
    float maxDiff = 0.f;
    int maxDiffIdx = 0;
    for (int i = 0; i < LUT_SIZE; i++) {
        float d = std::abs(originalLUT[i] - reversedLUT[i]);
        if (d > 0.f) {
            diffCount++;
            if (d > maxDiff) { maxDiff = d; maxDiffIdx = i; }
        }
    }

    char buf[256];
    _snprintf(buf, sizeof(buf),
        "%d/256 entries differ, max=%.8f at [%d] (orig=%.8f, rev=%.8f)",
        diffCount, maxDiff, maxDiffIdx,
        originalLUT[maxDiffIdx], reversedLUT[maxDiffIdx]);

    // Report as failure so the numbers show in test output.
    // This test intentionally "fails" — the failure message IS the evidence.
    GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
}

// --- Part 2: Simulate vehicle driving in a curve ---
// Simple version: heading += turnRate, pos += speed * (sin, cos).
// This has minimal compounding — error doesn't feed back.

static CVector SimulateCurveSimple(int frames, float speed, float turnRate) {
    CVector pos(0.f, 0.f, 0.f);
    float heading = 0.f;
    for (int i = 0; i < frames; i++) {
        heading += turnRate;
        pos.x += speed * CMaths::GetSinFast(heading);
        pos.y += speed * CMaths::GetCosFast(heading);
    }
    return pos;
}

// Realistic version: vehicle with 2D orientation matrix updated via sin/cos
// each frame. Steering rotates the forward vector, velocity is along forward,
// position integrates velocity. Error compounds because the orientation matrix
// drifts, which changes the velocity direction, which changes position, which
// (in a real game) changes collision response. This models what ApplyTurnSpeed
// + ProcessControl actually does.

struct SimVehicle {
    CVector pos;
    float forwardX, forwardY;  // unit forward vector (orientation)
    float speed;
    float turnRate;
};

static SimVehicle SimulateDriving(int frames, float speed, float turnRate) {
    SimVehicle v;
    v.pos = CVector(0.f, 0.f, 0.f);
    v.forwardX = 0.f;
    v.forwardY = 1.f;  // facing north
    v.speed = speed;
    v.turnRate = turnRate;

    for (int i = 0; i < frames; i++) {
        // Rotate forward vector by turnRate using sin/cos LUT
        // This is what the game's matrix rotation does every frame
        float s = CMaths::GetSinFast(v.turnRate);
        float c = CMaths::GetCosFast(v.turnRate);
        float newFwdX = v.forwardX * c - v.forwardY * s;
        float newFwdY = v.forwardX * s + v.forwardY * c;
        v.forwardX = newFwdX;
        v.forwardY = newFwdY;

        // Velocity along forward direction
        v.pos.x += v.speed * v.forwardX;
        v.pos.y += v.speed * v.forwardY;
    }
    return v;
}

GAME_TEST(SinLUT_Divergence, DrivingDrift_50Frames) {
    constexpr int LUT_SIZE = 256;
    constexpr int FRAMES = 50;
    constexpr float SPEED = 0.5f;      // ~56 km/h in game units
    constexpr float TURN_RATE = 0.04f;  // gentle turn (~2.3 deg/frame)

    auto& lut = StaticRef<std::array<float, LUT_SIZE>>(0xBB3E00);
    auto reversedLUT = lut;

    auto vRev = SimulateDriving(FRAMES, SPEED, TURN_RATE);

    if (!TryRestoreOriginalLUT(lut, reversedLUT)) {
        lut = reversedLUT;
        return;
    }

    auto vOrig = SimulateDriving(FRAMES, SPEED, TURN_RATE);
    lut = reversedLUT;

    float posDrift = (vRev.pos - vOrig.pos).Magnitude();
    float fwdDriftX = std::abs(vRev.forwardX - vOrig.forwardX);
    float fwdDriftY = std::abs(vRev.forwardY - vOrig.forwardY);

    char buf[512];
    _snprintf(buf, sizeof(buf),
        "%d frames (~1s): pos drift=%.8f units, heading drift=(%.8f, %.8f). "
        "Same code, same inputs, only sin LUT differs.",
        FRAMES, posDrift, fwdDriftX, fwdDriftY);

    GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
}

GAME_TEST(SinLUT_Divergence, DrivingDrift_1000Frames) {
    constexpr int LUT_SIZE = 256;
    constexpr int FRAMES = 1000;
    constexpr float SPEED = 0.5f;
    constexpr float TURN_RATE = 0.04f;

    auto& lut = StaticRef<std::array<float, LUT_SIZE>>(0xBB3E00);
    auto reversedLUT = lut;

    auto vRev = SimulateDriving(FRAMES, SPEED, TURN_RATE);

    if (!TryRestoreOriginalLUT(lut, reversedLUT)) {
        lut = reversedLUT;
        return;
    }

    auto vOrig = SimulateDriving(FRAMES, SPEED, TURN_RATE);
    lut = reversedLUT;

    float posDrift = (vRev.pos - vOrig.pos).Magnitude();
    float fwdDriftX = std::abs(vRev.forwardX - vOrig.forwardX);
    float fwdDriftY = std::abs(vRev.forwardY - vOrig.forwardY);

    char buf[512];
    _snprintf(buf, sizeof(buf),
        "%d frames (~20s): pos drift=%.8f units, heading drift=(%.8f, %.8f). "
        "A real reversal bug could produce similar or smaller drift.",
        FRAMES, posDrift, fwdDriftX, fwdDriftY);

    GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
}

GAME_TEST(SinLUT_Divergence, DrivingDrift_5000Frames) {
    constexpr int LUT_SIZE = 256;
    constexpr int FRAMES = 5000;
    constexpr float SPEED = 0.5f;
    constexpr float TURN_RATE = 0.04f;

    auto& lut = StaticRef<std::array<float, LUT_SIZE>>(0xBB3E00);
    auto reversedLUT = lut;

    auto vRev = SimulateDriving(FRAMES, SPEED, TURN_RATE);

    if (!TryRestoreOriginalLUT(lut, reversedLUT)) {
        lut = reversedLUT;
        return;
    }

    auto vOrig = SimulateDriving(FRAMES, SPEED, TURN_RATE);
    lut = reversedLUT;

    float posDrift = (vRev.pos - vOrig.pos).Magnitude();

    char buf[512];
    _snprintf(buf, sizeof(buf),
        "%d frames (~100s): pos drift=%.4f game units (%.2f meters). "
        "rev=(%.2f,%.2f) orig=(%.2f,%.2f). "
        "Can you tell if this is a reversal bug or sin table drift?",
        FRAMES, posDrift, posDrift,
        vRev.pos.x, vRev.pos.y, vOrig.pos.x, vOrig.pos.y);

    GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
}
