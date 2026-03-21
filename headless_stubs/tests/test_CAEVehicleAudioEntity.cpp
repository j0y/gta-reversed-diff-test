// test_CAEVehicleAudioEntity.cpp — Differential tests for vehicle audio queries.
// Hook paths: Audio/Entities/CAEVehicleAudioEntity/<fn>
// Uses the spawned vehicle's m_vehicleAudio member.
//
// Audio entity queries determine what sounds play for vehicles.
// Bugs here cause wrong engine sounds, missing sirens, etc.

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

// GetVehicleAudioSettings — static, takes vehId as offset from MODEL_VEHICLE_FIRST (400)
GAME_DIFF_TEST(CAEVehicleAudioEntity, GetVehicleAudioSettings_Various) {
    // vehId is model - 400, so police car 596 = vehId 196
    // Known bug: vehId 196-197 (LSPD/SFPD police) return wrong VehicleAudioType
    int16 vehIds[] = { 196, 197, 198, 199, 0, 1, 10, 20 };
    for (int16 id : vehIds) {
        auto orig = CallOriginal("Audio/Entities/CAEVehicleAudioEntity/GetVehicleAudioSettings",
            CAEVehicleAudioEntity::GetVehicleAudioSettings, id);
        auto rev = CAEVehicleAudioEntity::GetVehicleAudioSettings(id);
        // Compare key fields
        // Known bug: vehId=197 (model 597, SFPD police car) returns wrong VehicleAudioType
        // (orig=82, rev=0) — audio settings lookup table mismatch in gta-reversed
        if ((int)orig.VehicleAudioType != (int)rev.VehicleAudioType) {
            char msg[128];
            snprintf(msg, sizeof(msg), "vehId=%d: orig=%d rev=%d", (int)id, (int)orig.VehicleAudioType, (int)rev.VehicleAudioType);
            GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
            return;
        }
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
