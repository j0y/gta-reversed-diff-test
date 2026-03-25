// test_CPopulation6.cpp — Extended CPopulation differential tests.
// Hook paths: Global/CPopulation/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Population.h"

GAME_DIFF_TEST(CPopulation6, IsSkateable_GroveStreet) {
    CVector pos(2488.f, -1666.f, 13.5f);
    bool orig, rev;
    { HookDisableGuard guard("Global/CPopulation/IsSkateable"); orig = CPopulation::IsSkateable(pos); }
    rev = CPopulation::IsSkateable(pos);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPopulation6, IsSkateable_Various) {
    CVector positions[] = {
        {0.f, 0.f, 10.f},
        {1500.f, -1500.f, 20.f},
        {-2170.f, 250.f, 30.f},
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/IsSkateable"); orig = CPopulation::IsSkateable(pos); }
        rev = CPopulation::IsSkateable(pos);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPopulation6, CanSolicitPlayerOnFoot_Models) {
    eModelID models[] = { (eModelID)7, (eModelID)9, (eModelID)10, (eModelID)12, (eModelID)20, (eModelID)65, (eModelID)93 };
    for (auto m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/CanSolicitPlayerOnFoot"); orig = CPopulation::CanSolicitPlayerOnFoot(m); }
        rev = CPopulation::CanSolicitPlayerOnFoot(m);
        EXPECT_EQ(orig, rev);
    }
}

// CanSolicitPlayerInCar calls GetPedModelInfo() which crashes on non-ped models.
// Use only known ped model IDs (0-312 range, loaded ones).
GAME_DIFF_TEST(CPopulation6, CanSolicitPlayerInCar_PedModels) {
    // Only use models that are actually loaded ped models
    for (int32 id = 1; id <= 312; id++) {
        if (!CStreaming::IsModelLoaded(id)) continue;
        auto* mi = CModelInfo::GetModelInfo(id);
        if (!mi || mi->GetModelType() != MODEL_INFO_PED) continue;
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/CanSolicitPlayerInCar");
          orig = CPopulation::CanSolicitPlayerInCar((eModelID)id); }
        rev = CPopulation::CanSolicitPlayerInCar((eModelID)id);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPopulation6, PedMICanBeCreatedAtAttractor_Models) {
    eModelID models[] = { (eModelID)7, (eModelID)9, (eModelID)20, (eModelID)25, (eModelID)50 };
    for (auto m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/PedMICanBeCreatedAtAttractor"); orig = CPopulation::PedMICanBeCreatedAtAttractor(m); }
        rev = CPopulation::PedMICanBeCreatedAtAttractor(m);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPopulation6, PedMICanBeCreatedInInterior_Models) {
    eModelID models[] = { (eModelID)7, (eModelID)9, (eModelID)20, (eModelID)25, (eModelID)50, (eModelID)100 };
    for (auto m : models) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPopulation/PedMICanBeCreatedInInterior"); orig = CPopulation::PedMICanBeCreatedInInterior(m); }
        rev = CPopulation::PedMICanBeCreatedInInterior(m);
        EXPECT_EQ(orig, rev);
    }
}
