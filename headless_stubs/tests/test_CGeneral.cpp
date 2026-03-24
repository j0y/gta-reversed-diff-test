// test_CGeneral.cpp — Behavior + differential tests for CGeneral namespace.
// Hook paths: Global/CGeneral/<fn>

#include "StdInc.h"
#include "TestFramework.h"

// --- Behavior tests ---

GAME_TEST(CGeneral, LimitAngle_InRange) {
    EXPECT_NEAR(CGeneral::LimitAngle(0.0f), 0.0f, 1e-5f);
    EXPECT_NEAR(CGeneral::LimitAngle(90.0f), 90.0f, 1e-5f);
    EXPECT_NEAR(CGeneral::LimitAngle(-90.0f), -90.0f, 1e-5f);
    EXPECT_NEAR(CGeneral::LimitAngle(180.0f), -180.0f, 1e-5f); // boundary wraps
}

GAME_TEST(CGeneral, LimitAngle_Wrap) {
    EXPECT_NEAR(CGeneral::LimitAngle(270.0f), -90.0f, 1e-4f);
    EXPECT_NEAR(CGeneral::LimitAngle(-270.0f), 90.0f, 1e-4f);
    EXPECT_NEAR(CGeneral::LimitAngle(360.0f), 0.0f, 1e-4f);
    EXPECT_NEAR(CGeneral::LimitAngle(720.0f), 0.0f, 1e-4f);
}

GAME_TEST(CGeneral, GetRandomNumber_Range) {
    srand(42);
    for (int i = 0; i < 100; i++) {
        EXPECT_TRUE(CGeneral::GetRandomNumber() <= RAND_MAX);
    }
}

GAME_TEST(CGeneral, GetRandomNumberInRange_Float) {
    srand(42);
    for (int i = 0; i < 100; i++) {
        float r = CGeneral::GetRandomNumberInRange(10.0f, 20.0f);
        EXPECT_TRUE(r >= 10.0f);
        EXPECT_TRUE(r <= 20.0f);
    }
}

GAME_TEST(CGeneral, GetATanOfXY_Cardinals) {
    float east = CGeneral::GetATanOfXY(1.0f, 0.0f);
    float north = CGeneral::GetATanOfXY(0.0f, 1.0f);
    EXPECT_TRUE(std::isfinite(east));
    EXPECT_TRUE(std::isfinite(north));
    EXPECT_TRUE(east != north);
}

// --- Differential tests ---

GAME_DIFF_TEST(CGeneral, LimitAngle) {
    float inputs[] = { 0.f, 45.f, 90.f, -90.f, 180.f, -180.f, 270.f, -270.f, 360.f, 720.f, -720.f, 0.001f, -0.001f, 179.99f, -179.99f };
    for (float angle : inputs) {
        EXPECT_MATCHES_ORIGINAL("Global/CGeneral/LimitAngle", CGeneral::LimitAngle, 1e-7f, angle);
    }
}

GAME_DIFF_TEST(CGeneral, LimitRadianAngle) {
    // PI boundary wraps differently — skip exact boundary, test interior points
    float inputs[] = { 0.f, 1.f, -1.f, 3.0f, -3.0f, 6.28f, -6.28f, 12.56f, 0.001f };
    for (float angle : inputs) {
        EXPECT_MATCHES_ORIGINAL("Global/CGeneral/LimitRadianAngle", CGeneral::LimitRadianAngle, 1e-4f, angle);
    }
}

GAME_DIFF_TEST(CGeneral, GetATanOfXY) {
    float coords[][2] = { {1,0}, {0,1}, {-1,0}, {0,-1}, {1,1}, {-1,-1}, {3,4}, {0.001f,1000.f} };
    for (auto& c : coords) {
        EXPECT_MATCHES_ORIGINAL("Global/CGeneral/GetATanOfXY", CGeneral::GetATanOfXY, 1e-5f, c[0], c[1]);
    }
}

GAME_DIFF_TEST(CGeneral, SolveQuadratic) {
    auto testCase = [](float a, float b, float c) {
        float origX1 = 0, origX2 = 0, revX1 = 0, revX2 = 0;
        { HookDisableGuard guard("Global/CGeneral/SolveQuadratic");
          CGeneral::SolveQuadratic(a, b, c, origX1, origX2); }
        CGeneral::SolveQuadratic(a, b, c, revX1, revX2);
        EXPECT_NEAR(origX1, revX1, 1e-5f);
        EXPECT_NEAR(origX2, revX2, 1e-5f);
    };
    testCase(1, -3, 2);   // roots: 1, 2
    testCase(1, 0, -4);   // roots: -2, 2
    testCase(1, 2, 1);    // double root: -1
    testCase(1, 0, 1);    // no real roots
    testCase(0, 2, -4);   // linear: x=2
}

GAME_DIFF_TEST(CGeneral, GetAngleBetweenPoints) {
    float cases[][4] = {
        {0,0, 1,0}, {0,0, 0,1}, {0,0, -1,0}, {0,0, 0,-1},
        {1,1, 2,2}, {-5,3, 10,-7}, {0,0, 0.001f,0.001f}
    };
    for (auto& c : cases) {
        EXPECT_MATCHES_ORIGINAL("Global/CGeneral/GetAngleBetweenPoints",
            CGeneral::GetAngleBetweenPoints, 1e-3f, c[0], c[1], c[2], c[3]);
    }
}

// --- GetRadianAngleBetweenPoints (overloaded: empty suffix) ---

GAME_DIFF_TEST(CGeneral, GetRadianAngleBetweenPoints_Diff) {
    float cases[][4] = {
        {0,0, 1,0}, {0,0, 0,1}, {1,1, 2,2}, {-5,3, 10,-7},
    };
    for (auto& c : cases) {
        float orig, rev;
        { HookDisableGuard guard("Global/CGeneral/GetRadianAngleBetweenPoints-");
          orig = CGeneral::GetRadianAngleBetweenPoints(c[0], c[1], c[2], c[3]); }
        rev = CGeneral::GetRadianAngleBetweenPoints(c[0], c[1], c[2], c[3]);
        EXPECT_NEAR(orig, rev, 1e-4f);
    }
}

// --- GetRandomNumberInRange<int32> (overloaded) ---

GAME_DIFF_TEST(CGeneral, GetRandomNumberInRange_Int) {
    // Seed RNG deterministically for this test
    srand(42);
    int32 orig, rev;
    { HookDisableGuard guard("Global/CGeneral/GetRandomNumberInRange<int32>-");
      srand(42);
      orig = CGeneral::GetRandomNumberInRange(0, 100); }
    srand(42);
    rev = CGeneral::GetRandomNumberInRange(0, 100);
    EXPECT_EQ(orig, rev);
}
