// test_COverloaded2.cpp — More differential tests for overloaded hooks.
// All use the dash-separated hook path: Category/Class/Function-Suffix

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Entity.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Object/Object.h"
#include "Entity/Physical.h"
#include "Entity/Placeable.h"
#include "PedType.h"
#include "KeyGen.h"
#include "ModelInfo.h"
#include "WeaponInfo.h"
#include "Weapon.h"
#include "Wanted.h"
#include "ScenarioHelpers.h"

// ============================================================
// CEntity::GetBoundCentre (overloaded: -ref, -vec)
// ============================================================

GAME_DIFF_TEST(COverloaded2, GetBoundCentre_Ref_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector origCentre{}, revCentre{};
    { HookDisableGuard guard("Entity/CEntity/GetBoundCentre-ref");
      player->GetBoundCentre(origCentre); }
    player->GetBoundCentre(revCentre);
    EXPECT_NEAR(origCentre.x, revCentre.x, 1e-3f);
    EXPECT_NEAR(origCentre.y, revCentre.y, 1e-3f);
    EXPECT_NEAR(origCentre.z, revCentre.z, 1e-3f);
}

// ============================================================
// CEntity::GetIsTouching (overloaded: -ent, -vec)
// ============================================================

static Scenario::TestVehicle* s_ov2Veh = nullptr;
static bool s_triedOv2Veh = false;

static CVehicle* GetOv2Veh() {
    if (!s_triedOv2Veh) {
        s_triedOv2Veh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_ov2Veh = new Scenario::TestVehicle(model, CVector(2384.f, -1652.f, 13.5f));
    }
    return s_ov2Veh ? s_ov2Veh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(COverloaded2, GetIsTouching_Entity) {
    auto* player = FindPlayerPed(0);
    auto* veh = GetOv2Veh();
    if (!player || !veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetIsTouching-ent");
      orig = player->GetIsTouching(veh); }
    rev = player->GetIsTouching(veh);
    EXPECT_EQ(orig, rev);
}

// ============================================================
// CPlaceable::IsWithinArea (overloaded: -xy, -xyz)
// ============================================================

GAME_DIFF_TEST(COverloaded2, IsWithinArea_XY_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos = player->GetPosition();
    bool orig, rev;
    // Test within a box around player
    { HookDisableGuard guard("Entity/CPlaceable/IsWithinArea-xy");
      orig = player->IsWithinArea(pos.x - 10.f, pos.y - 10.f, pos.x + 10.f, pos.y + 10.f); }
    rev = player->IsWithinArea(pos.x - 10.f, pos.y - 10.f, pos.x + 10.f, pos.y + 10.f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(COverloaded2, IsWithinArea_XY_Player_Outside) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    // Test far from player
    { HookDisableGuard guard("Entity/CPlaceable/IsWithinArea-xy");
      orig = player->IsWithinArea(0.f, 0.f, 1.f, 1.f); }
    rev = player->IsWithinArea(0.f, 0.f, 1.f, 1.f);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(COverloaded2, IsWithinArea_XYZ_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos = player->GetPosition();
    bool orig, rev;
    { HookDisableGuard guard("Entity/CPlaceable/IsWithinArea-xyz");
      orig = player->IsWithinArea(pos.x - 10.f, pos.y - 10.f, pos.z - 10.f,
                                    pos.x + 10.f, pos.y + 10.f, pos.z + 10.f); }
    rev = player->IsWithinArea(pos.x - 10.f, pos.y - 10.f, pos.z - 10.f,
                                 pos.x + 10.f, pos.y + 10.f, pos.z + 10.f);
    EXPECT_EQ(orig, rev);
}

// ============================================================
// CKeyGen::GetKey (overloaded: -null-terminated, -sized)
// ============================================================

GAME_DIFF_TEST(COverloaded2, GetKey_NullTerminated) {
    const char* strings[] = { "test", "PLAYER", "police", "GANTON", "" };
    for (auto* s : strings) {
        uint32 orig, rev;
        { HookDisableGuard guard("Core/CKeyGen/GetKey-null-terminated");
          orig = CKeyGen::GetKey(s); }
        rev = CKeyGen::GetKey(s);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(COverloaded2, GetKey_Sized) {
    const char* str = "teststring";
    for (size_t len = 0; len <= 10; len++) {
        uint32 orig, rev;
        { HookDisableGuard guard("Core/CKeyGen/GetKey-sized");
          orig = CKeyGen::GetKey(str, len); }
        rev = CKeyGen::GetKey(str, len);
        EXPECT_EQ(orig, rev);
    }
}

// ============================================================
// CModelInfo::GetModelInfo (overloaded: -full, -minmax)
// ============================================================

GAME_DIFF_TEST(COverloaded2, GetModelInfo_Full) {
    const char* names[] = { "infernus", "cheetah", "buffalo" };
    for (auto* name : names) {
        CBaseModelInfo* orig;
        CBaseModelInfo* rev;
        { HookDisableGuard guard("Models/CModelInfo/GetModelInfo-full");
          orig = CModelInfo::GetModelInfo(name, nullptr); }
        rev = CModelInfo::GetModelInfo(name, nullptr);
        EXPECT_EQ(orig, rev);
    }
}

// ============================================================
// CPed::GetWeaponSkill (overloaded: -Current, -WeaponType)
// ============================================================

GAME_DIFF_TEST(COverloaded2, GetWeaponSkill_Current) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    uint32 orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPed/GetWeaponSkill-Current");
      orig = (uint32)player->GetWeaponSkill(); }
    rev = (uint32)player->GetWeaponSkill();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(COverloaded2, GetWeaponSkill_WeaponType) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    eWeaponType weapons[] = { WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_AK47, WEAPON_M4 };
    for (auto wt : weapons) {
        uint32 orig, rev;
        { HookDisableGuard guard("Entity/Ped/CPed/GetWeaponSkill-WeaponType");
          orig = (uint32)player->GetWeaponSkill(wt); }
        rev = (uint32)player->GetWeaponSkill(wt);
        EXPECT_EQ(orig, rev);
    }
}

// ============================================================
// CPhysical::ApplyMoveForce-vec
// ============================================================

GAME_DIFF_TEST(COverloaded2, ApplyMoveForce_Vec) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector savedMove = player->m_vecMoveSpeed;
    CVector force(0.1f, 0.05f, 0.0f);

    player->m_vecMoveSpeed = savedMove;
    { HookDisableGuard guard("Entity/CPhysical/ApplyMoveForce-vec");
      player->ApplyMoveForce(force); }
    CVector origSpeed = player->m_vecMoveSpeed;

    player->m_vecMoveSpeed = savedMove;
    player->ApplyMoveForce(force);
    CVector revSpeed = player->m_vecMoveSpeed;

    EXPECT_NEAR(origSpeed.x, revSpeed.x, 1e-5f);
    EXPECT_NEAR(origSpeed.y, revSpeed.y, 1e-5f);
    EXPECT_NEAR(origSpeed.z, revSpeed.z, 1e-5f);
    player->m_vecMoveSpeed = savedMove;
}

// ============================================================
// CWeaponInfo::GetWeaponInfo-original (takes eWeaponType + eWeaponSkill)
// ============================================================

GAME_DIFF_TEST(COverloaded2, GetWeaponInfo_Original) {
    eWeaponType weapons[] = { WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_SHOTGUN, WEAPON_AK47, WEAPON_KNIFE };
    for (auto wt : weapons) {
        CWeaponInfo* orig;
        CWeaponInfo* rev;
        { HookDisableGuard guard("Global/CWeaponInfo/GetWeaponInfo-original");
          orig = CWeaponInfo::GetWeaponInfo(wt, eWeaponSkill::STD); }
        rev = CWeaponInfo::GetWeaponInfo(wt, eWeaponSkill::STD);
        EXPECT_EQ(orig, rev);
    }
}

// ============================================================
// CPedType::GetPedTypeAcquaintances (overloaded: -uint32 takes AcquaintanceId + ePedType)
// ============================================================

GAME_DIFF_TEST(COverloaded2, GetPedTypeAcquaintances_Uint32) {
    for (int32 type = 0; type < 8; type++) {
        uint32 orig, rev;
        { HookDisableGuard guard("Global/CPedType/GetPedTypeAcquaintances-uint32");
          orig = CPedType::GetPedTypeAcquaintances(0, static_cast<ePedType>(type)); }
        rev = CPedType::GetPedTypeAcquaintances(0, static_cast<ePedType>(type));
        EXPECT_EQ(orig, rev);
    }
}

// ============================================================
// CWanted::CanCopJoinPursuit-method
// ============================================================

GAME_DIFF_TEST(COverloaded2, CanCopJoinPursuit_Method) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* wanted = player->GetWanted();
    if (!wanted) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CWanted/CanCopJoinPursuit-method");
      orig = wanted->CanCopJoinPursuit(nullptr); }
    rev = wanted->CanCopJoinPursuit(nullptr);
    EXPECT_EQ(orig, rev);
}
