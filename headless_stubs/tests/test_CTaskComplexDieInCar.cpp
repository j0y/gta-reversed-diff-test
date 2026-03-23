// test_CTaskComplexDieInCar.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Tasks/TaskTypes/TaskComplexDieInCar.h"

GAME_TEST(CTaskComplexDieInCar, GetTaskType) {
    CTaskComplexDieInCar task(WEAPON_UNARMED);
    EXPECT_EQ((int32)task.GetTaskType(), (int32)TASK_COMPLEX_DIE_IN_CAR);
}

GAME_TEST(CTaskComplexDieInCar, IsComplex) {
    CTaskComplexDieInCar task(WEAPON_UNARMED);
    EXPECT_TRUE(task.IsComplex());
}

GAME_TEST(CTaskComplexDieInCar, Constructor_WeaponType) {
    CTaskComplexDieInCar task(WEAPON_AK47);
    EXPECT_EQ((int32)task.m_nWeaponType, (int32)WEAPON_AK47);
}

GAME_TEST(CTaskComplexDieInCar, Clone_PreservesWeapon) {
    CTaskComplexDieInCar original(WEAPON_EXPLOSION);
    CTask* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetTaskType(), (int32)TASK_COMPLEX_DIE_IN_CAR);
    auto* c = static_cast<CTaskComplexDieInCar*>(cloned);
    EXPECT_EQ((int32)c->m_nWeaponType, (int32)WEAPON_EXPLOSION);
    delete cloned;
}

GAME_TEST(CTaskComplexDieInCar, DifferentWeapons) {
    eWeaponType weapons[] = { WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_EXPLOSION, WEAPON_DROWNING };
    for (auto wt : weapons) {
        CTaskComplexDieInCar task(wt);
        EXPECT_EQ((int32)task.m_nWeaponType, (int32)wt);
    }
}
