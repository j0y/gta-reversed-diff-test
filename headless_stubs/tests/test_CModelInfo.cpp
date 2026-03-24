// test_CModelInfo.cpp — Differential tests for CModelInfo.
// Hook paths: Models/CModelInfo/<fn>
// Vehicle type classification functions — all static, take model index.

#include "StdInc.h"
#include "TestFramework.h"
#include "ModelInfo.h"

// Vehicle model IDs: 400-611. Test representative set + boundary/invalid values.
static const int32 TEST_IDS[] = {
    0, 1, 100, 399,                                  // non-vehicle
    400, 401, 402, 403, 404, 405, 410, 415, 420,     // cars
    425, 430, 435, 440, 445, 448, 449, 450, 451,     // various vehicles
    460, 461, 462, 463, 464, 465, 466, 467, 468, 469, // bikes
    470, 471, 472, 473,                               // boats
    476, 487, 488, 497, 501, 511, 512, 513, 519, 520, // planes, helis
    530, 531, 532, 539, 548, 556, 557, 570, 571, 572, // train, quad, etc.
    573, 574, 575, 576, 577, 578, 580, 590, 596, 600, 611,
    612, 1000, 5000, 19999                            // out of vehicle range
};
static const int NUM_TEST_IDS = sizeof(TEST_IDS) / sizeof(TEST_IDS[0]);

#define DIFF_MODEL_CHECK(FnName) \
    GAME_DIFF_TEST(CModelInfo, FnName) { \
        for (int i = 0; i < NUM_TEST_IDS; i++) { \
            int32 id = TEST_IDS[i]; \
            auto orig = CallOriginal("Models/CModelInfo/" #FnName, CModelInfo::FnName, id); \
            auto rev = CModelInfo::FnName(id); \
            EXPECT_EQ(orig, rev); \
        } \
    }

DIFF_MODEL_CHECK(IsBoatModel)
DIFF_MODEL_CHECK(IsCarModel)
DIFF_MODEL_CHECK(IsTrainModel)
DIFF_MODEL_CHECK(IsHeliModel)
DIFF_MODEL_CHECK(IsPlaneModel)
DIFF_MODEL_CHECK(IsBikeModel)
DIFF_MODEL_CHECK(IsFakePlaneModel)
DIFF_MODEL_CHECK(IsMonsterTruckModel)
DIFF_MODEL_CHECK(IsQuadBikeModel)
DIFF_MODEL_CHECK(IsBmxModel)
DIFF_MODEL_CHECK(IsTrailerModel)

GAME_DIFF_TEST(CModelInfo, IsVehicleModelType) {
    for (int i = 0; i < NUM_TEST_IDS; i++) {
        int32 id = TEST_IDS[i];
        int32 orig, rev;
        { HookDisableGuard guard("Models/CModelInfo/IsVehicleModelType"); orig = CModelInfo::IsVehicleModelType(id); }
        rev = CModelInfo::IsVehicleModelType(id);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetModelInfoFromHashKey sweep ---
// Tests hash-to-model-info lookup across known model name hashes.

GAME_DIFF_TEST(CModelInfo, GetModelInfoFromHashKey_Sweep) {
    // Hash a bunch of known model names and look them up
    const char* names[] = {
        "landstal", "bravura", "buffalo", "linerun", "perennial",
        "sentinel", "dumper", "firetruck", "trashmaster", "stretch",
        "manana", "infernus", "voodoo", "pony", "mule",
        "cheetah", "ambulance", "fbiranch", "moonbeam", "esperanto",
        "taxi", "washington", "bobcat", "mrwhoop", "bfinject",
        "police", "enforcer", "securicar", "banshee", "predator",
    };
    for (auto* name : names) {
        uint32 hash = CKeyGen::GetKey(name);
        CBaseModelInfo* orig;
        CBaseModelInfo* rev;
        { HookDisableGuard guard("Models/CModelInfo/GetModelInfoFromHashKey");
          orig = CModelInfo::GetModelInfoFromHashKey(hash, nullptr); }
        rev = CModelInfo::GetModelInfoFromHashKey(hash, nullptr);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetModelInfoUInt16 (takes name + out uint16*) ---

GAME_DIFF_TEST(CModelInfo, GetModelInfoUInt16_ByName) {
    const char* names[] = { "infernus", "cheetah", "buffalo", "police", "taxi" };
    for (auto* name : names) {
        uint16 origId = 0, revId = 0;
        CBaseModelInfo* orig;
        CBaseModelInfo* rev;
        { HookDisableGuard guard("Models/CModelInfo/GetModelInfoUInt16");
          orig = CModelInfo::GetModelInfoUInt16(name, &origId); }
        rev = CModelInfo::GetModelInfoUInt16(name, &revId);
        EXPECT_EQ(orig, rev);
        EXPECT_EQ(origId, revId);
    }
}
