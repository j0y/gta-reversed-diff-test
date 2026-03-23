// test_CNodeAddress.cpp — Tests for CNodeAddress.
// Path node addressing used by the navigation/pathfind system.

#include "StdInc.h"
#include "TestFramework.h"
#include "NodeAddress.h"

GAME_TEST(CNodeAddress, DefaultConstructor) {
    CNodeAddress addr;
    // Default: invalid area/node
    EXPECT_FALSE(addr.IsValid());
}

GAME_TEST(CNodeAddress, ParameterizedConstructor) {
    CNodeAddress addr(5, 10);
    EXPECT_EQ(addr.m_wAreaId, (uint16)5);
    EXPECT_EQ(addr.m_wNodeId, (uint16)10);
}

GAME_TEST(CNodeAddress, IsValid_ValidAddress) {
    CNodeAddress addr(1, 1);
    EXPECT_TRUE(addr.IsValid());
}

GAME_TEST(CNodeAddress, IsAreaValid) {
    CNodeAddress addr(3, 0);
    EXPECT_TRUE(addr.IsAreaValid());
}

GAME_TEST(CNodeAddress, ResetAreaId) {
    CNodeAddress addr(5, 10);
    addr.ResetAreaId();
    EXPECT_FALSE(addr.IsAreaValid());
    EXPECT_EQ(addr.m_wNodeId, (uint16)10); // node unchanged
}

GAME_TEST(CNodeAddress, ResetNodeId) {
    CNodeAddress addr(5, 10);
    addr.ResetNodeId();
    EXPECT_EQ(addr.m_wAreaId, (uint16)5); // area unchanged
}

GAME_TEST(CNodeAddress, Equality) {
    CNodeAddress a(1, 2);
    CNodeAddress b(1, 2);
    CNodeAddress c(1, 3);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
    EXPECT_FALSE(a != b);
}

GAME_TEST(CNodeAddress, BoolOperator) {
    CNodeAddress valid(1, 1);
    CNodeAddress invalid;
    EXPECT_TRUE((bool)valid);
    EXPECT_FALSE((bool)invalid);
}
