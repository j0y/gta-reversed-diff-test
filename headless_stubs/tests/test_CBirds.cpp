// test_CBirds.cpp — Differential + behavior tests for CBirds.
// Hook paths: Global/CBirds/<fn>
//
// Bird flock lifecycle. Render() uses RenderWare (skip headlessly).
// Init, Shutdown, CreateNumberOfBirds, HandleGunShot are testable.
// Update reads camera state but doesn't render — testable too.

#include "StdInc.h"
#include "TestFramework.h"
#include "Birds.h"

// Save/restore bird state around tests that modify it
struct BirdStateSaver {
    std::array<CBird, 6> savedBirds;
    uint32 savedCount;
    bool savedShot;
    CVector savedShotPos;

    BirdStateSaver() {
        savedBirds = CBirds::aBirds;
        savedCount = CBirds::uiNumberOfBirds;
        savedShot = CBirds::bHasBirdBeenShot;
        savedShotPos = CBirds::vecBirdShotAt;
    }
    ~BirdStateSaver() {
        CBirds::aBirds = savedBirds;
        CBirds::uiNumberOfBirds = savedCount;
        CBirds::bHasBirdBeenShot = savedShot;
        CBirds::vecBirdShotAt = savedShotPos;
    }
};

// --- Differential tests ---

GAME_DIFF_TEST(CBirds, Init) {
    BirdStateSaver saver;

    // Pre-populate some birds
    CBirds::aBirds[0].m_bCreated = true;
    CBirds::aBirds[1].m_bCreated = true;
    CBirds::uiNumberOfBirds = 2;
    CBirds::bHasBirdBeenShot = true;

    // Reversed Init
    CBirds::Init();
    bool revCreated0 = CBirds::aBirds[0].m_bCreated;
    bool revCreated1 = CBirds::aBirds[1].m_bCreated;
    uint32 revCount = CBirds::uiNumberOfBirds;
    bool revShot = CBirds::bHasBirdBeenShot;

    // Restore and run original
    CBirds::aBirds[0].m_bCreated = true;
    CBirds::aBirds[1].m_bCreated = true;
    CBirds::uiNumberOfBirds = 2;
    CBirds::bHasBirdBeenShot = true;
    { HookDisableGuard guard("Global/CBirds/Init");
      CBirds::Init(); }

    EXPECT_EQ(revCreated0, CBirds::aBirds[0].m_bCreated);
    EXPECT_EQ(revCreated1, CBirds::aBirds[1].m_bCreated);
    EXPECT_EQ(revCount, CBirds::uiNumberOfBirds);
    EXPECT_EQ(revShot, CBirds::bHasBirdBeenShot);
}

GAME_DIFF_TEST(CBirds, Shutdown) {
    BirdStateSaver saver;

    CBirds::aBirds[0].m_bCreated = true;
    CBirds::aBirds[2].m_bCreated = true;
    CBirds::uiNumberOfBirds = 2;

    CBirds::Shutdown();
    bool revCreated0 = CBirds::aBirds[0].m_bCreated;
    bool revCreated2 = CBirds::aBirds[2].m_bCreated;
    uint32 revCount = CBirds::uiNumberOfBirds;

    CBirds::aBirds[0].m_bCreated = true;
    CBirds::aBirds[2].m_bCreated = true;
    CBirds::uiNumberOfBirds = 2;
    { HookDisableGuard guard("Global/CBirds/Shutdown");
      CBirds::Shutdown(); }

    EXPECT_EQ(revCreated0, CBirds::aBirds[0].m_bCreated);
    EXPECT_EQ(revCreated2, CBirds::aBirds[2].m_bCreated);
    EXPECT_EQ(revCount, CBirds::uiNumberOfBirds);
}

GAME_DIFF_TEST(CBirds, CreateNumberOfBirds) {
    BirdStateSaver saver;

    // Seed RNG for deterministic results
    srand(42);
    CBirds::Init();

    CVector start = TheCamera.GetPosition() + CVector(20.0f, 0.0f, 15.0f);
    CVector target = TheCamera.GetPosition() + CVector(0.0f, 0.0f, 15.0f);

    // Reversed
    CBirds::CreateNumberOfBirds(start, target, 3, eBirdsBiome::BIOME_NORMAL, false);
    uint32 revCount = CBirds::uiNumberOfBirds;
    bool revCreated[6];
    CVector revPos[6];
    for (int i = 0; i < 6; i++) {
        revCreated[i] = CBirds::aBirds[i].m_bCreated;
        revPos[i] = CBirds::aBirds[i].m_vecPosn;
    }

    // Reset and run original with same seed
    srand(42);
    CBirds::Init();
    { HookDisableGuard guard("Global/CBirds/CreateNumberOfBirds");
      CBirds::CreateNumberOfBirds(start, target, 3, eBirdsBiome::BIOME_NORMAL, false); }

    EXPECT_EQ(revCount, CBirds::uiNumberOfBirds);
    for (int i = 0; i < 6; i++) {
        EXPECT_EQ(revCreated[i], CBirds::aBirds[i].m_bCreated);
        if (revCreated[i]) {
            EXPECT_NEAR(revPos[i].x, CBirds::aBirds[i].m_vecPosn.x, 1e-3f);
            EXPECT_NEAR(revPos[i].y, CBirds::aBirds[i].m_vecPosn.y, 1e-3f);
            EXPECT_NEAR(revPos[i].z, CBirds::aBirds[i].m_vecPosn.z, 1e-3f);
        }
    }
}

GAME_DIFF_TEST(CBirds, HandleGunShot) {
    BirdStateSaver saver;

    CBirds::Init();
    // Place a bird at a known location
    CBirds::aBirds[0].m_bCreated = true;
    CBirds::aBirds[0].m_vecPosn = CVector(100.0f, 100.0f, 50.0f);
    CBirds::uiNumberOfBirds = 1;

    // Shoot through the bird
    CVector shotA(100.0f, 100.0f, 49.0f);
    CVector shotB(100.0f, 100.0f, 51.0f);

    CBirds::HandleGunShot(&shotA, &shotB);
    bool revShot = CBirds::bHasBirdBeenShot;
    bool revCreated = CBirds::aBirds[0].m_bCreated;
    uint32 revCount = CBirds::uiNumberOfBirds;

    // Reset and run original
    CBirds::Init();
    CBirds::aBirds[0].m_bCreated = true;
    CBirds::aBirds[0].m_vecPosn = CVector(100.0f, 100.0f, 50.0f);
    CBirds::uiNumberOfBirds = 1;
    { HookDisableGuard guard("Global/CBirds/HandleGunShot");
      CBirds::HandleGunShot(&shotA, &shotB); }

    EXPECT_EQ(revShot, CBirds::bHasBirdBeenShot);
    EXPECT_EQ(revCreated, CBirds::aBirds[0].m_bCreated);
    EXPECT_EQ(revCount, CBirds::uiNumberOfBirds);
}

GAME_DIFF_TEST(CBirds, HandleGunShot_Miss) {
    BirdStateSaver saver;

    CBirds::Init();
    CBirds::aBirds[0].m_bCreated = true;
    CBirds::aBirds[0].m_vecPosn = CVector(100.0f, 100.0f, 50.0f);
    CBirds::uiNumberOfBirds = 1;

    // Shoot far from the bird
    CVector shotA(0.0f, 0.0f, 0.0f);
    CVector shotB(0.0f, 0.0f, 10.0f);

    CBirds::HandleGunShot(&shotA, &shotB);
    bool revShot = CBirds::bHasBirdBeenShot;
    bool revCreated = CBirds::aBirds[0].m_bCreated;

    CBirds::Init();
    CBirds::aBirds[0].m_bCreated = true;
    CBirds::aBirds[0].m_vecPosn = CVector(100.0f, 100.0f, 50.0f);
    CBirds::uiNumberOfBirds = 1;
    { HookDisableGuard guard("Global/CBirds/HandleGunShot");
      CBirds::HandleGunShot(&shotA, &shotB); }

    EXPECT_EQ(revShot, CBirds::bHasBirdBeenShot);
    EXPECT_EQ(revCreated, CBirds::aBirds[0].m_bCreated);
}

// --- Behavior tests ---

GAME_TEST(CBirds, ArraySizeIs6) {
    EXPECT_EQ(CBirds::aBirds.size(), (size_t)6);
}

GAME_TEST(CBirds, InitClearsAllBirds) {
    BirdStateSaver saver;

    CBirds::aBirds[0].m_bCreated = true;
    CBirds::uiNumberOfBirds = 1;
    CBirds::bHasBirdBeenShot = true;

    CBirds::Init();

    for (auto& bird : CBirds::aBirds) {
        EXPECT_FALSE(bird.m_bCreated);
    }
    EXPECT_EQ(CBirds::uiNumberOfBirds, 0u);
    EXPECT_FALSE(CBirds::bHasBirdBeenShot);
}

GAME_TEST(CBirds, CreateBirds_ZeroCount) {
    BirdStateSaver saver;

    CBirds::Init();
    CVector start(0, 0, 50), target(10, 0, 50);
    CBirds::CreateNumberOfBirds(start, target, 0, eBirdsBiome::BIOME_NORMAL, false);
    EXPECT_EQ(CBirds::uiNumberOfBirds, 0u);
}

GAME_TEST(CBirds, CreationCoords_FormationShape) {
    // Verify the formation offsets are reasonable
    EXPECT_NEAR(CBirds::faCreationCoorsX[0], 0.0f, 1e-5f);  // lead bird at center
    EXPECT_NEAR(CBirds::faCreationCoorsY[0], 0.0f, 1e-5f);
    EXPECT_NEAR(CBirds::faCreationCoorsZ[0], 0.0f, 1e-5f);
    // Other birds should be offset
    EXPECT_TRUE(CBirds::faCreationCoorsZ[1] > 0.0f);  // slightly above
}
