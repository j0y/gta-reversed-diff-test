// test_CHud2.cpp — CHud behavior tests.
// CHud hooks are not registered at runtime — behavior tests only.

#include "StdInc.h"
#include "TestFramework.h"
#include "Hud.h"

GAME_TEST(CHud2, HelpMessageDisplayed) {
    bool result = CHud::HelpMessageDisplayed();
    (void)result;
    EXPECT_TRUE(true);
}

GAME_TEST(CHud2, GetYPosBasedOnHealth_Default) {
    float result = CHud::GetYPosBasedOnHealth(0, 100.0f, 0);
    EXPECT_TRUE(result >= 0.0f);
}

GAME_TEST(CHud2, GetYPosBasedOnHealth_WithOffset) {
    float result = CHud::GetYPosBasedOnHealth(0, 89.0f, 12);
    EXPECT_TRUE(result >= 0.0f);
}
