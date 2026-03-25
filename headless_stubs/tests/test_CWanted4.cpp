// test_CWanted4.cpp — Extended CWanted differential tests.
// Hook paths: Global/CWanted/<fn>
// Tests wanted-level query functions not covered by test_CWanted.cpp/CWanted2/CWanted3.

#include "StdInc.h"
#include "TestFramework.h"
#include "Wanted.h"

GAME_DIFF_TEST(CWanted4, AreMiamiViceRequired) {
    CWanted* wanted = FindPlayerWanted(0);
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreMiamiViceRequired"); orig = wanted->AreMiamiViceRequired(); }
    rev = wanted->AreMiamiViceRequired();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted4, AreSwatRequired) {
    CWanted* wanted = FindPlayerWanted(0);
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreSwatRequired"); orig = wanted->AreSwatRequired(); }
    rev = wanted->AreSwatRequired();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted4, AreFbiRequired) {
    CWanted* wanted = FindPlayerWanted(0);
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreFbiRequired"); orig = wanted->AreFbiRequired(); }
    rev = wanted->AreFbiRequired();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted4, AreArmyRequired) {
    CWanted* wanted = FindPlayerWanted(0);
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AreArmyRequired"); orig = wanted->AreArmyRequired(); }
    rev = wanted->AreArmyRequired();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted4, IsInPursuit) {
    CWanted* wanted = FindPlayerWanted(0);
    if (!wanted) return;
    // IsInPursuit takes a CCopPed* — test with nullptr
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/IsInPursuit"); orig = wanted->IsInPursuit(nullptr); }
    rev = wanted->IsInPursuit(nullptr);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted4, WorkOutPolicePresence) {
    CWanted* wanted = FindPlayerWanted(0);
    if (!wanted) return;
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos = player->GetPosition();
    float radius = 100.0f;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CWanted/WorkOutPolicePresence"); orig = wanted->WorkOutPolicePresence(pos, radius); }
    rev = wanted->WorkOutPolicePresence(pos, radius);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CWanted4, AddCrimeToQ) {
    CWanted* wanted = FindPlayerWanted(0);
    if (!wanted) return;
    // Test with a position — AddCrimeToQ queues a crime
    // Save and restore crime queue state
    // Use eCrimeType 1 (minor crime) at player pos
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos = player->GetPosition();
    // Just compare return values — both will add to queue
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/AddCrimeToQ"); orig = wanted->AddCrimeToQ((eCrimeType)1, 0, pos, false, false); }
    rev = wanted->AddCrimeToQ((eCrimeType)1, 0, pos, false, false);
    EXPECT_EQ(orig, rev);
}
