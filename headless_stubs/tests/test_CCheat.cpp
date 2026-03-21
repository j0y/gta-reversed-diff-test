// test_CCheat.cpp — Differential tests for CCheat.
// Hook paths: Global/CCheat/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CCheat, AddToCheatString) {
    // AddToCheatString appends a char to the cheat input buffer.
    // Save and restore the buffer to avoid side effects.
    auto savedStr = std::string(CCheat::m_CheatString);

    char origStr[30]{}, revStr[30]{};

    CCheat::m_CheatString[0] = '\0';
    { HookDisableGuard guard("Global/CCheat/AddToCheatString"); CCheat::AddToCheatString('A'); }
    strncpy(origStr, CCheat::m_CheatString, sizeof(origStr) - 1);

    CCheat::m_CheatString[0] = '\0';
    CCheat::AddToCheatString('A');
    strncpy(revStr, CCheat::m_CheatString, sizeof(revStr) - 1);

    // Restore
    strncpy(CCheat::m_CheatString, savedStr.c_str(), 29);

    EXPECT_TRUE(strcmp(origStr, revStr) == 0);
}
