// test_CObject5.cpp — Differential tests for CObject on spawned objects.
// Hook paths: Entity/Object/CObject/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Object/Object.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "World.h"

static CObject* s_obj5 = nullptr;
static bool s_triedObj5 = false;

static CObject* GetTestObject() {
    if (!s_triedObj5) {
        s_triedObj5 = true;
        // Find a loaded object model (models 615+)
        for (int32 id = 615; id < 20000; id++) {
            auto* mi = CModelInfo::GetModelInfo(id);
            if (!mi) continue;
            if (!CStreaming::IsModelLoaded(id)) continue;
            // Try to create an object from it
            s_obj5 = new CObject(id, false);
            if (s_obj5) {
                s_obj5->SetPosn(CVector(2492.0f, -1666.0f, 13.5f));
                CWorld::Add(s_obj5);
                break;
            }
        }
    }
    return s_obj5;
}

#define OBJ5_GUARD() if (!GetTestObject()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No object model loaded"); return; }

GAME_DIFF_TEST(CObject5, CanBeDeleted) {
    OBJ5_GUARD()
    bool orig, rev;
    { HookDisableGuard guard("Entity/Object/CObject/CanBeDeleted"); orig = s_obj5->CanBeDeleted(); }
    rev = s_obj5->CanBeDeleted();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CObject5, CanBeTargetted) {
    OBJ5_GUARD()
    bool orig, rev;
    { HookDisableGuard guard("Entity/Object/CObject/CanBeTargetted"); orig = s_obj5->CanBeTargetted(); }
    rev = s_obj5->CanBeTargetted();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CObject5, CanBeUsedToTakeCoverBehind) {
    OBJ5_GUARD()
    bool orig, rev;
    { HookDisableGuard guard("Entity/Object/CObject/CanBeUsedToTakeCoverBehind"); orig = s_obj5->CanBeUsedToTakeCoverBehind(); }
    rev = s_obj5->CanBeUsedToTakeCoverBehind();
    EXPECT_EQ(orig, rev);
}

// --- CEntity base queries on object ---

GAME_DIFF_TEST(CObject5, Entity_IsVisible) {
    OBJ5_GUARD()
    auto* ent = static_cast<CEntity*>(s_obj5);
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/IsVisible"); orig = ent->IsVisible(); }
    rev = ent->IsVisible();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CObject5, Entity_GetIsStatic) {
    OBJ5_GUARD()
    auto* ent = static_cast<CEntity*>(s_obj5);
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetIsStatic"); orig = ent->GetIsStatic(); }
    rev = ent->GetIsStatic();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CObject5, Entity_GetColModel) {
    OBJ5_GUARD()
    auto* ent = static_cast<CEntity*>(s_obj5);
    CColModel* orig;
    CColModel* rev;
    { HookDisableGuard guard("Entity/CEntity/GetColModel"); orig = ent->GetColModel(); }
    rev = ent->GetColModel();
    EXPECT_EQ(orig, rev);
}
