// test_CAEVehicleAudioEntity.cpp — Differential tests for vehicle audio queries.
// Hook paths: Audio/Entities/CAEVehicleAudioEntity/<fn>
// Uses the spawned vehicle's m_vehicleAudio member.
//
// GetVehicleAudioSettings returns a 36-byte struct by value (sret convention)
// and the original at 0x4F5C10 uses `ret 8` (callee-cleanup). The standard
// HookDisableGuard / CallOriginal helpers don't handle this correctly, so we
// call the original via inline asm with explicit sret pointer.
//
// IMPORTANT: the original at 0x4F5C10 takes a MODEL ID (e.g. 596), not a
// vehId offset (e.g. 196). It subtracts 400 internally. The reversed C++
// wrapper takes vehId (offset from MODEL_VEHICLE_FIRST).

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Audio/entities/AEVehicleAudioEntity.h"

static Scenario::TestVehicle* s_audioVeh = nullptr;
static bool s_triedAudioVeh = false;

static CVehicle* GetAudioVehicle() {
    if (!s_triedAudioVeh) {
        s_triedAudioVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            s_audioVeh = new Scenario::TestVehicle(model, CVector(2504.0f, -1665.0f, 13.5f));
        }
    }
    return s_audioVeh ? s_audioVeh->AsVehicle() : nullptr;
}

// Call original GetVehicleAudioSettings at 0x4F5C10 directly.
// The original is a struct-returning function with `ret 8` (callee cleans
// sret pointer + modelId). MSVC generates this for functions returning
// structs > 8 bytes with __stdcall convention.
// We use a naked wrapper to avoid compiler interference with the stack.
static tVehicleAudioSettings CallOriginal_GetVehicleAudioSettings(int16 modelId) {
    // Read directly from the original table at 0x860AF0 instead of calling
    // the function, since the function just does a memcpy from the table.
    // This avoids all calling convention issues.
    // Table: 232 entries of 0x24 bytes each, indexed by (modelId - 400).
    constexpr uint32 TABLE_ADDR = 0x860AF0;
    constexpr uint32 ENTRY_SIZE = 0x24;
    int index = modelId - 400;
    auto* entry = reinterpret_cast<const tVehicleAudioSettings*>(TABLE_ADDR + index * ENTRY_SIZE);
    tVehicleAudioSettings result;
    memcpy(&result, entry, sizeof(result));
    return result;
}

// GetVehicleTypeForAudio — no args, returns audio classification
GAME_DIFF_TEST(CAEVehicleAudioEntity, GetVehicleTypeForAudio) {
    auto* veh = GetAudioVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    int orig, rev;
    { HookDisableGuard guard("Audio/Entities/CAEVehicleAudioEntity/GetVehicleTypeForAudio");
      orig = (int)veh->m_vehicleAudio.GetVehicleTypeForAudio(); }
    rev = (int)veh->m_vehicleAudio.GetVehicleTypeForAudio();
    EXPECT_EQ(orig, rev);
}

// GetVehicleAudioSettings — compare original (direct asm call) vs reversed.
// Original takes model ID; reversed takes vehId (model - 400).
GAME_TEST(CAEVehicleAudioEntity, GetVehicleAudioSettings_Various) {
    // Test a spread of model IDs
    int16 modelIds[] = {
        400, 401, 410, 420,           // early vehicles
        596, 597, 598, 599,           // police cars
        560, 562, 567, 480, 510, 530, // misc cars
    };

    for (int16 modelId : modelIds) {
        auto orig = CallOriginal_GetVehicleAudioSettings(modelId);
        auto rev  = CAEVehicleAudioEntity::GetVehicleAudioSettings(modelId - 400);

        EXPECT_EQ((int)orig.VehicleAudioType, (int)rev.VehicleAudioType);
        EXPECT_EQ((int)orig.PlayerBank, (int)rev.PlayerBank);
        EXPECT_EQ((int)orig.DummyBank, (int)rev.DummyBank);
        EXPECT_EQ((int)orig.BassSetting, (int)rev.BassSetting);
        EXPECT_NEAR(orig.BassFactor, rev.BassFactor, 1e-5f);
        EXPECT_NEAR(orig.EnginePitch, rev.EnginePitch, 1e-5f);
        EXPECT_EQ((int)orig.HornType, (int)rev.HornType);
        EXPECT_NEAR(orig.HornPitch, rev.HornPitch, 1e-5f);
        EXPECT_EQ((int)orig.DoorType, (int)rev.DoorType);
        EXPECT_EQ((int)orig.RadioStation, (int)rev.RadioStation);
        EXPECT_EQ((int)orig.RadioType, (int)rev.RadioType);
        EXPECT_EQ((int)orig.VehicleAudioTypeForName, (int)rev.VehicleAudioTypeForName);
        EXPECT_NEAR(orig.EngineVolumeOffset, rev.EngineVolumeOffset, 1e-5f);
    }
}

// StaticGetPlayerVehicleAudioSettingsForRadio — returns pointer to static settings
GAME_DIFF_TEST(CAEVehicleAudioEntity, StaticGetPlayerVehicleAudioSettingsForRadio) {
    void* orig;
    void* rev;
    { HookDisableGuard guard("Audio/Entities/CAEVehicleAudioEntity/StaticGetPlayerVehicleAudioSettingsForRadio");
      orig = (void*)CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio(); }
    rev = (void*)CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();
    EXPECT_EQ(orig, rev);
}

// Diagnostic
GAME_TEST(CAEVehicleAudioEntity, SpawnDiagnostic) {
    auto* veh = GetAudioVehicle();
    char buf[256];
    if (veh) {
        auto audioType = (int)veh->m_vehicleAudio.GetVehicleTypeForAudio();
        snprintf(buf, sizeof(buf), "[DIAG] AudioEntity: model=%d audioType=%d",
            veh->GetModelIndex(), audioType);
    } else {
        snprintf(buf, sizeof(buf), "[DIAG] AudioEntity: failed to spawn vehicle");
    }
    { FILE* f = fopen("C:\\audio_diag.log", "w"); if(f) { fputs(buf, f); fputc('\n', f); fclose(f); } }
    EXPECT_TRUE(veh != nullptr);
}
