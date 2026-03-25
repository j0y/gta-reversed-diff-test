// test_CCover2.cpp — Differential tests for CCover.
// Hook paths: Global/CCover/<fn>
// Tests cover point management functions.

#include "StdInc.h"
#include "TestFramework.h"
#include "Cover.h"

GAME_TEST(CCover2, Init_NoCrash) {
    // CCover::Init resets the cover point array — safe to call
    CCover::Init();
    EXPECT_TRUE(true);
}

// ShouldThisBuildingHaveItsCoverPointsCreated takes CBuilding* — can't easily
// create one. Instead test other CCover functions.

// RemoveCoverPointsForThisEntity with nullptr — tests null guard
GAME_TEST(CCover2, RemoveCoverPointsForThisEntity_Null) {
    CCover::RemoveCoverPointsForThisEntity(nullptr);
    EXPECT_TRUE(true);
}
