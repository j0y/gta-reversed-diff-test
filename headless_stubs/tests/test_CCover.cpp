// test_CCover.cpp — Deep differential tests for CCover.
// Hook paths: Global/CCover/<fn>
// Note: FindDirFromVector and FindVectorFromDir are locked — tested behavior only.

#include "StdInc.h"
#include "TestFramework.h"
#include "Cover.h"

// FindDirFromVector is locked but we can call it directly since it's a pure function
// at 0x698D40

GAME_TEST(CCover, FindDirFromVector_Cardinals) {
    // These are locked hooks — can't diff test, but verify no crash and reasonable output
    CVector dirs[] = {
        CVector(1.0f, 0.0f, 0.0f),   // east
        CVector(-1.0f, 0.0f, 0.0f),  // west
        CVector(0.0f, 1.0f, 0.0f),   // north
        CVector(0.0f, -1.0f, 0.0f),  // south
    };
    for (auto& d : dirs) {
        uint8 result = CCover::FindDirFromVector(d);
        EXPECT_TRUE(result < 8); // direction should be 0-7
    }
}

GAME_TEST(CCover, FindVectorFromDir_AllDirs) {
    for (uint8 dir = 0; dir < 8; dir++) {
        CVector result = CCover::FindVectorFromDir(dir);
        // Should be a unit-ish vector on XY plane
        float mag = result.x * result.x + result.y * result.y;
        EXPECT_TRUE(mag > 0.5f);
        EXPECT_TRUE(mag < 1.5f);
    }
}

// ShouldThisBuildingHaveItsCoverPointsCreated(nullptr) crashes — needs valid building
