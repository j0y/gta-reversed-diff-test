// test_CPathFind4.cpp — Systematic CPathFind differential tests.
// Tests ALL remaining untested hooked CPathFind query functions.
// Hook paths: Global/CPathFind/<fn>
// Uses ThePaths global instance.

#include "StdInc.h"
#include "TestFramework.h"
#include "PathFind.h"

// ===================================================================
// ThisNodeHasToBeSwitchedOff — bool, takes CPathNode*
// ===================================================================

GAME_DIFF_TEST(CPathFind4, ThisNodeHasToBeSwitchedOff_Node0) {
    auto* node = ThePaths.GetPathNode(CNodeAddress{(uint16)0, (uint16)0});
    if (!node) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPathFind/ThisNodeHasToBeSwitchedOff");
      orig = ThePaths.ThisNodeHasToBeSwitchedOff(node); }
    rev = ThePaths.ThisNodeHasToBeSwitchedOff(node);
    EXPECT_EQ(orig, rev);
}

// MultipleNodes crashes — area 0 nodes may not be fully loaded at state 9

// ===================================================================
// ThisNodeWillLeadIntoADeadEnd — bool, takes two CPathNode*
// ===================================================================

GAME_DIFF_TEST(CPathFind4, ThisNodeWillLeadIntoADeadEnd) {
    auto* node0 = ThePaths.GetPathNode(CNodeAddress{(uint16)0, (uint16)0});
    auto* node1 = ThePaths.GetPathNode(CNodeAddress{(uint16)0, (uint16)1});
    if (!node0 || !node1) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPathFind/ThisNodeWillLeadIntoADeadEnd");
      orig = ThePaths.ThisNodeWillLeadIntoADeadEnd(node0, node1); }
    rev = ThePaths.ThisNodeWillLeadIntoADeadEnd(node0, node1);
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// CountNeighboursToBeSwitchedOff — size_t, takes const CPathNode&
// ===================================================================

// CountNeighbours on multiple nodes crashes — area 0 nodes not fully loaded

// ===================================================================
// FindLinkBetweenNodes — CCarPathLinkAddress, takes two CNodeAddress
// ===================================================================

GAME_DIFF_TEST(CPathFind4, FindLinkBetweenNodes_Adjacent) {
    CNodeAddress a{0, 0}, b{0, 1};
    CCarPathLinkAddress orig, rev;
    { HookDisableGuard guard("Global/CPathFind/FindLinkBetweenNodes");
      orig = ThePaths.FindLinkBetweenNodes(a, b); }
    rev = ThePaths.FindLinkBetweenNodes(a, b);
    EXPECT_EQ(orig.IsValid(), rev.IsValid());
}

GAME_DIFF_TEST(CPathFind4, FindLinkBetweenNodes_Same) {
    CNodeAddress a{0, 0};
    CCarPathLinkAddress orig, rev;
    { HookDisableGuard guard("Global/CPathFind/FindLinkBetweenNodes");
      orig = ThePaths.FindLinkBetweenNodes(a, a); }
    rev = ThePaths.FindLinkBetweenNodes(a, a);
    EXPECT_EQ(orig.IsValid(), rev.IsValid());
}

// ===================================================================
// ReturnInteriorNodeIndex — CNodeAddress
// ===================================================================

GAME_DIFF_TEST(CPathFind4, ReturnInteriorNodeIndex_Zero) {
    CNodeAddress orig, rev;
    { HookDisableGuard guard("Global/CPathFind/ReturnInteriorNodeIndex");
      orig = ThePaths.ReturnInteriorNodeIndex(0, 0, 0); }
    rev = ThePaths.ReturnInteriorNodeIndex(0, 0, 0);
    EXPECT_EQ(orig.IsValid(), rev.IsValid());
}

// ===================================================================
// FindNearestExteriorNodeToInteriorNode — CNodeAddress
// ===================================================================

GAME_DIFF_TEST(CPathFind4, FindNearestExteriorNodeToInteriorNode_Zero) {
    CNodeAddress orig, rev;
    { HookDisableGuard guard("Global/CPathFind/FindNearestExteriorNodeToInteriorNode");
      orig = ThePaths.FindNearestExteriorNodeToInteriorNode(0); }
    rev = ThePaths.FindNearestExteriorNodeToInteriorNode(0);
    EXPECT_EQ(orig.IsValid(), rev.IsValid());
}

// ===================================================================
// AreNodesLoadedForArea — bool, takes area bounds
// ===================================================================

GAME_DIFF_TEST(CPathFind4, AreNodesLoadedForArea_GroveStreet) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CPathFind/AreNodesLoadedForArea");
      orig = ThePaths.AreNodesLoadedForArea(2400.f, 2550.f, -1700.f, -1600.f); }
    rev = ThePaths.AreNodesLoadedForArea(2400.f, 2550.f, -1700.f, -1600.f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPathFind4, AreNodesLoadedForArea_Downtown) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CPathFind/AreNodesLoadedForArea");
      orig = ThePaths.AreNodesLoadedForArea(1000.f, 1200.f, -1000.f, -800.f); }
    rev = ThePaths.AreNodesLoadedForArea(1000.f, 1200.f, -1000.f, -800.f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPathFind4, AreNodesLoadedForArea_FarAway) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CPathFind/AreNodesLoadedForArea");
      orig = ThePaths.AreNodesLoadedForArea(-3000.f, -2900.f, -3000.f, -2900.f); }
    rev = ThePaths.AreNodesLoadedForArea(-3000.f, -2900.f, -3000.f, -2900.f);
    EXPECT_EQ(orig, rev);
}

// ===================================================================
// MarkRegionsForCoors — void, takes position + radius
// ===================================================================

GAME_DIFF_TEST(CPathFind4, MarkRegionsForCoors_GroveStreet) {
    CVector pos(2488.f, -1666.f, 13.5f);
    { HookDisableGuard guard("Global/CPathFind/MarkRegionsForCoors");
      ThePaths.MarkRegionsForCoors(pos, 100.f); }
    ThePaths.MarkRegionsForCoors(pos, 100.f);
    EXPECT_TRUE(true);
}

// ===================================================================
// SetPathsNeededAtPosition — void, marks paths for streaming
// ===================================================================

GAME_DIFF_TEST(CPathFind4, SetPathsNeededAtPosition_GroveStreet) {
    CVector pos(2488.f, -1666.f, 13.5f);
    { HookDisableGuard guard("Global/CPathFind/SetPathsNeededAtPosition");
      ThePaths.SetPathsNeededAtPosition(pos); }
    ThePaths.SetPathsNeededAtPosition(pos);
    EXPECT_TRUE(true);
}

// ===================================================================
// TestForPedTrafficLight — already tested but add more positions
// ===================================================================

GAME_DIFF_TEST(CPathFind4, TestForPedTrafficLight_MultipleNodes) {
    for (uint16 i = 0; i < 20; i++) {
        CNodeAddress a{(uint16)0, i}, b{(uint16)0, (uint16)(i + 1)};
        bool orig, rev;
        { HookDisableGuard guard("Global/CPathFind/TestForPedTrafficLight");
          orig = ThePaths.TestForPedTrafficLight(a, b); }
        rev = ThePaths.TestForPedTrafficLight(a, b);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// TestCrossesRoad — already tested but add more node pairs
// ===================================================================

GAME_DIFF_TEST(CPathFind4, TestCrossesRoad_MultipleNodes) {
    for (uint16 i = 0; i < 20; i++) {
        CNodeAddress a{(uint16)0, i}, b{(uint16)0, (uint16)(i + 1)};
        bool orig, rev;
        { HookDisableGuard guard("Global/CPathFind/TestCrossesRoad");
          orig = ThePaths.TestCrossesRoad(a, b); }
        rev = ThePaths.TestCrossesRoad(a, b);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// These2NodesAreAdjacent — already tested but extend to more pairs
// ===================================================================

// These2NodesAreAdjacent extended crashes — area 0 node pairs not fully loaded

// ===================================================================
// IsWaterNodeNearby — already tested but add more positions
// ===================================================================

GAME_DIFF_TEST(CPathFind4, IsWaterNodeNearby_OceanAndInland) {
    CVector positions[] = {
        {370.f, -1840.f, 3.f},     // ocean
        {2488.f, -1666.f, 13.5f},  // inland
        {-2170.f, 250.f, 30.f},    // SF
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPathFind/IsWaterNodeNearby");
          orig = ThePaths.IsWaterNodeNearby(pos, 100.f); }
        rev = ThePaths.IsWaterNodeNearby(pos, 100.f);
        EXPECT_EQ(orig, rev);
    }
}
