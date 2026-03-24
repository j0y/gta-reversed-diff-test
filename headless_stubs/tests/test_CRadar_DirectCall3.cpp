// test_CRadar_DirectCall3.cpp — Direct-call test for TransformRealWorldToTexCoordSpace.

#include "StdInc.h"
#include "TestFramework.h"
#include "Radar.h"

GAME_TEST(CRadar_DirectCall3, TransformRealWorldToTexCoordSpace) {
    struct TestCase { CVector2D pos; int32 x, y; };
    TestCase cases[] = {
        { {2495.0f, -1665.0f}, 0, 0 },
        { {2495.0f, -1665.0f}, 6, 6 },
        { {0.0f, 0.0f}, 0, 0 },
        { {0.0f, 0.0f}, 5, 5 },
        { {-2170.0f, 250.0f}, 3, 9 },
        { {2027.0f, 1008.0f}, 11, 3 },
        { {3000.0f, -3000.0f}, 11, 0 },
    };
    for (auto& tc : cases) {
        CVector2D orig = CallOriginal_CVector2D(0x583600, tc.pos, tc.x, tc.y);
        CVector2D rev = CRadar::TransformRealWorldToTexCoordSpace(tc.pos, tc.x, tc.y);
        EXPECT_NEAR(orig.x, rev.x, 1e-3f);
        EXPECT_NEAR(orig.y, rev.y, 1e-3f);
    }
}
