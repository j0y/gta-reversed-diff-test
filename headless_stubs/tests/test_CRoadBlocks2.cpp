// test_CRoadBlocks2.cpp — Extended CRoadBlocks differential tests.
// Hook paths: Global/CRoadBlocks/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "RoadBlocks.h"

GAME_TEST(CRoadBlocks2, Init_NoCrash) {
    CRoadBlocks::Init();
    EXPECT_TRUE(true);
}

GAME_TEST(CRoadBlocks2, ClearScriptRoadBlocks_NoCrash) {
    CRoadBlocks::ClearScriptRoadBlocks();
    EXPECT_TRUE(true);
}
