// test_CMaths.cpp — Tests for CMaths sin/cos lookup table.
//
// BUG: The reversed code's static initializer (Maths.cpp line 9) overwrites
// the original game's precomputed sin LUT at 0xBB3E00 with std::sin-generated
// values BEFORE hooks are installed. The original InitMathsTables (0x59AC90)
// never gets a chance to run with the game's original computation. Difference
// is up to ~0.02 per entry — affects every inlined GetSinFast/GetCosFast call.
//
// The GetSinFast/GetCosFast hooks are registered but not findable at runtime
// (functions are inlined everywhere, no callers exist in the original binary).

#include "StdInc.h"
#include "TestFramework.h"
#include "Maths.h"

// --- Differential: reversed LUT vs original game LUT ---

GAME_DIFF_TEST(CMaths, SinLUT_MatchesOriginal) {
    // The original LUT at 0xBB3E00 has been overwritten by the reversed code's
    // static initializer. We can restore it by calling the original InitMathsTables
    // via its address. The hook trampoline at 0x59AC90 should still contain the
    // original code bytes when the hook is disabled.
    //
    // Save current (reversed) LUT
    constexpr int LUT_SIZE = 256;
    auto& lut = StaticRef<std::array<float, LUT_SIZE>>(0xBB3E00);
    std::array<float, LUT_SIZE> reversedLUT = lut;

    // Call original InitMathsTables directly at its address.
    // This restores the original game's sin table.
    using InitFn = void(*)();
    auto origInit = (InitFn)0x59AC90;

    // The hook at 0x59AC90 jumps to the reversed no-op. We need to check if
    // calling it actually changes the LUT. If not, the hook is active and we
    // can't get original values — test is inconclusive.
    origInit();
    bool lutChanged = (lut != reversedLUT);

    if (!lutChanged) {
        // Hook is active — reversed no-op ran. Can't compare.
        // This itself is the bug: the reversed static init overwrote the LUT,
        // and InitMathsTables is a no-op, so original values are lost forever.
        // Flag as failure to document the issue.
        GetTestContext().RecordFailure(__FILE__, __LINE__,
            "CMaths LUT: reversed static init overwrites original sin table at 0xBB3E00; "
            "InitMathsTables is a no-op so original values cannot be restored");
    } else {
        // Original InitMathsTables actually wrote different values — compare them
        std::array<float, LUT_SIZE> origLUT = lut;
        for (int i = 0; i < LUT_SIZE; i++) {
            // The reversed code generates the LUT from std::sin at static init
            // (MSVC C++23 constexpr). The original game computed it at runtime
            // (MSVC 2004 CRT). Differences up to ~3e-6 (last significant digit
            // of float32) are acceptable — just different sin() rounding.
            EXPECT_NEAR(reversedLUT[i], origLUT[i], 1e-5f);
        }
        // Restore reversed LUT
        lut = reversedLUT;
    }
}

// --- Behavioral sanity checks ---

GAME_TEST(CMaths, SinFast_Zero) {
    EXPECT_NEAR(CMaths::GetSinFast(0.0f), 0.0f, 1e-5f);
}

GAME_TEST(CMaths, CosFast_Zero) {
    EXPECT_NEAR(CMaths::GetCosFast(0.0f), 1.0f, 0.025f);
}

GAME_TEST(CMaths, SinCos_Identity) {
    float angles[] = { 0.0f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f };
    for (float a : angles) {
        float s = CMaths::GetSinFast(a);
        float c = CMaths::GetCosFast(a);
        EXPECT_NEAR(s * s + c * c, 1.0f, 0.1f);
    }
}

GAME_TEST(CMaths, SinFast_Periodicity) {
    float angles[] = { 0.0f, 1.0f, 2.0f, 3.0f };
    for (float a : angles) {
        EXPECT_NEAR(CMaths::GetSinFast(a), CMaths::GetSinFast(a + TWO_PI), 1e-5f);
    }
}

GAME_TEST(CMaths, CosFromSin_Consistency) {
    for (int i = 0; i < 256; i++) {
        float rad = (float)i * TWO_PI / 256.0f;
        EXPECT_NEAR(CMaths::GetCosFast(rad), CMaths::GetSinFast(rad + PI / 2.0f), 1e-6f);
    }
}
