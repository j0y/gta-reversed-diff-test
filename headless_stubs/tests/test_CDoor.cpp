// test_CDoor.cpp — Differential tests for CDoor member functions.
// Hook paths: Global/CDoor/<fn>
//
// CDoor has member functions operating on angle/openAngle fields.
// Open, GetAngleOpenRatio, IsClosed, IsFullyOpen are pure math.

#include "StdInc.h"
#include "TestFramework.h"
#include "Door.h"

// --- GetAngleOpenRatio ---

GAME_DIFF_TEST(CDoor, GetAngleOpenRatio) {
    float openAngles[] = { 1.0f, 0.5f, 1.5707f, 3.14f, -1.0f, -0.5f };
    float currentAngles[] = { 0.0f, 0.25f, 0.5f, 1.0f, -0.25f, -0.5f };

    for (float open : openAngles) {
        for (float cur : currentAngles) {
            CDoor orig{}, rev{};
            orig.m_openAngle = open; orig.m_angle = cur;
            rev.m_openAngle = open;  rev.m_angle = cur;
            float o, r;
            { HookDisableGuard guard("Global/CDoor/GetAngleOpenRatio");
              o = orig.GetAngleOpenRatio(); }
            r = rev.GetAngleOpenRatio();
            EXPECT_NEAR(o, r, 1e-5f);
        }
    }
}

// --- IsClosed ---

GAME_DIFF_TEST(CDoor, IsClosed) {
    float testAngles[] = { 0.0f, 0.001f, -0.001f, 0.5f, -0.5f, 1.0f };
    float openAngles[] = { 1.0f, 0.5f, -1.0f };

    for (float open : openAngles) {
        for (float a : testAngles) {
            CDoor orig{}, rev{};
            orig.m_openAngle = open; orig.m_angle = a; orig.m_prevAngle = a;
            rev.m_openAngle = open;  rev.m_angle = a;  rev.m_prevAngle = a;
            bool o, r;
            { HookDisableGuard guard("Global/CDoor/IsClosed");
              o = orig.IsClosed(); }
            r = rev.IsClosed();
            EXPECT_EQ(o, r);
        }
    }
}

// --- IsFullyOpen ---

GAME_DIFF_TEST(CDoor, IsFullyOpen) {
    struct Case { float angle, openAngle; };
    Case cases[] = {
        { 1.0f, 1.0f },       // exactly open
        { 0.99f, 1.0f },      // near open
        { 0.4f, 1.0f },       // not open
        { 0.0f, 1.0f },       // closed
        { -1.0f, -1.0f },     // negative fully open
        { -0.6f, -1.0f },     // negative not open
        { 1.5f, 1.5f },       // large angle
        { 1.0f, 1.5f },       // not fully open
    };
    for (auto& c : cases) {
        CDoor orig{}, rev{};
        orig.m_angle = c.angle; orig.m_openAngle = c.openAngle;
        rev.m_angle = c.angle;  rev.m_openAngle = c.openAngle;
        bool o, r;
        { HookDisableGuard guard("Global/CDoor/IsFullyOpen");
          o = orig.IsFullyOpen(); }
        r = rev.IsFullyOpen();
        EXPECT_EQ(o, r);
    }
}

// --- Open ---

GAME_DIFF_TEST(CDoor, Open) {
    float openRatios[] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f, -0.5f };
    float openAngles[] = { 1.0f, 1.5707f, -1.0f, 0.5f };

    for (float oa : openAngles) {
        for (float ratio : openRatios) {
            CDoor orig{}, rev{};
            orig.m_openAngle = oa;
            rev.m_openAngle = oa;
            { HookDisableGuard guard("Global/CDoor/Open");
              orig.Open(ratio); }
            rev.Open(ratio);
            EXPECT_NEAR(orig.m_angle, rev.m_angle, 1e-5f);
        }
    }
}
