// test_CPathFind.cpp — Differential tests for CPathFind.
// Hook paths: Global/CPathFind/<fn>
// ThePaths is a global instance at 0x96F050, initialized at state 9.

#include "StdInc.h"
#include "TestFramework.h"
#include "PathFind.h"

// --- Region coordinate mapping (pure math, no node data needed) ---

GAME_DIFF_TEST(CPathFind, FindXRegionForCoors) {
    float xs[] = { -3000.0f, -2000.0f, -1000.0f, 0.0f, 1000.0f, 2000.0f, 3000.0f };
    for (float x : xs) {
        size_t orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindXRegionForCoors"); orig = ThePaths.FindXRegionForCoors(x); }
        rev = ThePaths.FindXRegionForCoors(x);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPathFind, FindYRegionForCoors) {
    float ys[] = { -3000.0f, -2000.0f, -1000.0f, 0.0f, 1000.0f, 2000.0f, 3000.0f };
    for (float y : ys) {
        size_t orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindYRegionForCoors"); orig = ThePaths.FindYRegionForCoors(y); }
        rev = ThePaths.FindYRegionForCoors(y);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPathFind, FindRegionForCoors) {
    CVector2D positions[] = {
        { 0.0f, 0.0f }, { 1000.0f, 1000.0f }, { -2000.0f, 500.0f },
        { 2488.0f, -1666.0f }, { -1500.0f, -1500.0f }, { 2000.0f, 2000.0f }
    };
    for (auto& pos : positions) {
        size_t orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindRegionForCoors"); orig = ThePaths.FindRegionForCoors(pos); }
        rev = ThePaths.FindRegionForCoors(pos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPathFind, FindXCoorsForRegion) {
    for (size_t r = 0; r < 8; r++) {
        float orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindXCoorsForRegion"); orig = ThePaths.FindXCoorsForRegion(r); }
        rev = ThePaths.FindXCoorsForRegion(r);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

GAME_DIFF_TEST(CPathFind, FindYCoorsForRegion) {
    for (size_t r = 0; r < 8; r++) {
        float orig, rev;
        { HookDisableGuard guard("Global/CPathFind/FindYCoorsForRegion"); orig = ThePaths.FindYCoorsForRegion(r); }
        rev = ThePaths.FindYCoorsForRegion(r);
        EXPECT_NEAR(orig, rev, 1e-3f);
    }
}

// --- Node data queries (paths loaded at init) ---

GAME_DIFF_TEST(CPathFind, IsAreaNodesAvailable) {
    // Test with a few node addresses — valid and invalid
    CNodeAddress addrs[] = {
        { 0, 0 }, { 0, 1 }, { 1, 0 }, { 5, 10 },
        { (uint16)-1, (uint16)-1 }, { 0, 100 }
    };
    for (auto& addr : addrs) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPathFind/IsAreaNodesAvailable"); orig = ThePaths.IsAreaNodesAvailable(addr); }
        rev = ThePaths.IsAreaNodesAvailable(addr);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPathFind, HaveRequestedNodesBeenLoaded) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CPathFind/HaveRequestedNodesBeenLoaded"); orig = ThePaths.HaveRequestedNodesBeenLoaded(); }
    rev = ThePaths.HaveRequestedNodesBeenLoaded();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPathFind, AreNodesLoadedForArea) {
    // Test known areas in Los Santos
    struct { float minX, maxX, minY, maxY; } areas[] = {
        { 2400.0f, 2500.0f, -1700.0f, -1600.0f },  // LS downtown
        { 0.0f, 100.0f, 0.0f, 100.0f },              // near origin
        { -2000.0f, -1900.0f, -1000.0f, -900.0f },   // SF area
        { -3000.0f, 3000.0f, -3000.0f, 3000.0f },     // whole map
    };
    for (auto& a : areas) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPathFind/AreNodesLoadedForArea"); orig = ThePaths.AreNodesLoadedForArea(a.minX, a.maxX, a.minY, a.maxY); }
        rev = ThePaths.AreNodesLoadedForArea(a.minX, a.maxX, a.minY, a.maxY);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPathFind, GetPathNode) {
    CNodeAddress addrs[] = {
        { 0, 0 }, { 0, 1 }, { 0, 5 }, { 1, 0 }, { 5, 0 }
    };
    for (auto& addr : addrs) {
        CPathNode* orig;
        CPathNode* rev;
        { HookDisableGuard guard("Global/CPathFind/GetPathNode"); orig = ThePaths.GetPathNode(addr); }
        rev = ThePaths.GetPathNode(addr);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPathFind, These2NodesAreAdjacent) {
    // Only test with nodes known to be in loaded areas
    // First verify area 0 is loaded, then test
    if (!ThePaths.IsAreaNodesAvailable(CNodeAddress(0, 0))) return;
    auto* node0 = ThePaths.GetPathNode(CNodeAddress(0, 0));
    auto* node1 = ThePaths.GetPathNode(CNodeAddress(0, 1));
    if (!node0 || !node1) return;
    CNodeAddress a1(0, 0), a2(0, 1);
    bool orig, rev;
    { HookDisableGuard guard("Global/CPathFind/These2NodesAreAdjacent"); orig = ThePaths.These2NodesAreAdjacent(a1, a2); }
    rev = ThePaths.These2NodesAreAdjacent(a1, a2);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPathFind, TestCrossesRoad) {
    CNodeAddress a1(0, 0), a2(0, 1), a3(0, 5);
    struct { CNodeAddress n1, n2; } pairs[] = {
        { a1, a2 }, { a1, a3 }, { a2, a3 }
    };
    for (auto& p : pairs) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPathFind/TestCrossesRoad"); orig = ThePaths.TestCrossesRoad(p.n1, p.n2); }
        rev = ThePaths.TestCrossesRoad(p.n1, p.n2);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPathFind, TestForPedTrafficLight) {
    CNodeAddress a1(0, 0), a2(0, 1), a3(0, 5);
    struct { CNodeAddress n1, n2; } pairs[] = {
        { a1, a2 }, { a1, a3 }, { a2, a3 }
    };
    for (auto& p : pairs) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPathFind/TestForPedTrafficLight"); orig = ThePaths.TestForPedTrafficLight(p.n1, p.n2); }
        rev = ThePaths.TestForPedTrafficLight(p.n1, p.n2);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPathFind, IsWaterNodeNearby) {
    CVector positions[] = {
        { 2488.0f, -1666.0f, 13.0f },   // LS
        { 0.0f, 0.0f, 0.0f },            // origin
        { -1500.0f, 500.0f, 10.0f },     // ocean area
    };
    float radii[] = { 50.0f, 200.0f };
    for (auto& pos : positions) {
        for (float r : radii) {
            bool orig, rev;
            { HookDisableGuard guard("Global/CPathFind/IsWaterNodeNearby"); orig = ThePaths.IsWaterNodeNearby(pos, r); }
            rev = ThePaths.IsWaterNodeNearby(pos, r);
            EXPECT_EQ(orig, rev);
        }
    }
}
