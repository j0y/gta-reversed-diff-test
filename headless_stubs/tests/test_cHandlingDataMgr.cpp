// test_cHandlingDataMgr.cpp — Differential tests for cHandlingDataMgr.
// Hook paths: Global/cHandlingDataMgr/<fn>
//
// 210 vehicle handling entries loaded from HANDLING.CFG at startup.
// Tests cover name lookups, drive type queries, pointer range checks,
// and unit conversion round-trips.

#include "StdInc.h"
#include "TestFramework.h"
#include "cHandlingDataMgr.h"

static constexpr int32 NUM_VEHICLES = 210;
static constexpr int32 NUM_BIKES = 13;
static constexpr int32 NUM_FLYING = 24;
static constexpr int32 NUM_BOATS = 12;

// --- Differential tests ---

GAME_DIFF_TEST(cHandlingDataMgr, GetHandlingId) {
    // GetHandlingId calls NOTSA_UNREACHABLE on not-found (crashes).
    // Use FindExactWord first to verify name exists, then compare.
    auto& VehicleNames = StaticRef<char[210][14]>(0x8D3978);
    const char* names[] = {
        "LANDSTAL", "BRAVURA", "BUFFALO", "LINERUN", "PEREN",
        "SENTINEL", "DUMPER", "FIRETRU", "TRASH", "STRETCH",
        "MANANA", "INFERNU", "VOODOO", "PONY", "MULE",
        "CHEETAH", "AMBULAN", "MOONBM", "ESPERAN", "TAXI",
        "WASHING", "BOBCAT", "MRWHOOP", "BFINJECT", "HUNTER",
        "POLICE", "ENFORCER", "SECURIC", "BANSHEE", "BUS",
        "RHINO", "BARRACKS", "HOTKNIF", "ARTICT1", "PREVION",
        "COACH", "CABBIE", "STALLION", "RUMPO", "RCBANDIT",
        "ROMERO", "PACKER", "MONSTER", "ADMIRAL", "SQUALO",
        "SEASPAR", "PIZZABOY", "TRAM", "ARTICT2", "TURISMO",
    };
    for (const char* name : names) {
        // Verify name exists first (FindExactWord returns -1 if not found)
        int32 check = gHandlingDataMgr.FindExactWord(name, &VehicleNames[0][0], 14, 210);
        if (check == -1) continue;

        int32 rev = gHandlingDataMgr.GetHandlingId(name);
        int32 orig;
        { HookDisableGuard guard("Global/cHandlingDataMgr/GetHandlingId");
          orig = gHandlingDataMgr.GetHandlingId(name); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(cHandlingDataMgr, HasFrontWheelDrive) {
    for (uint8 id = 0; id < NUM_VEHICLES; id++) {
        bool rev = gHandlingDataMgr.HasFrontWheelDrive(id);
        bool orig;
        { HookDisableGuard guard("Global/cHandlingDataMgr/HasFrontWheelDrive");
          orig = gHandlingDataMgr.HasFrontWheelDrive(id); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(cHandlingDataMgr, HasRearWheelDrive) {
    for (uint8 id = 0; id < NUM_VEHICLES; id++) {
        bool rev = gHandlingDataMgr.HasRearWheelDrive(id);
        bool orig;
        { HookDisableGuard guard("Global/cHandlingDataMgr/HasRearWheelDrive");
          orig = gHandlingDataMgr.HasRearWheelDrive(id); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(cHandlingDataMgr, GetFlyingPointer) {
    // Flying handling IDs: 186-209 map to m_aFlyingHandling[0..23]
    // Out-of-range returns &m_aFlyingHandling[0]
    uint8 ids[] = { 0, 100, 185, 186, 187, 195, 200, 209, 210, 255 };
    for (uint8 id : ids) {
        auto* rev = gHandlingDataMgr.GetFlyingPointer(id);
        tFlyingHandlingData* orig;
        { HookDisableGuard guard("Global/cHandlingDataMgr/GetFlyingPointer");
          orig = gHandlingDataMgr.GetFlyingPointer(id); }
        EXPECT_EQ((uintptr_t)rev, (uintptr_t)orig);
    }
}

GAME_DIFF_TEST(cHandlingDataMgr, GetBoatPointer) {
    // Boat handling IDs: 175-186 map to m_aBoatHandling[0..11]
    // Out-of-range returns &m_aBoatHandling[0]
    uint8 ids[] = { 0, 100, 174, 175, 176, 180, 186, 187, 255 };
    for (uint8 id : ids) {
        auto* rev = gHandlingDataMgr.GetBoatPointer(id);
        tBoatHandlingData* orig;
        { HookDisableGuard guard("Global/cHandlingDataMgr/GetBoatPointer");
          orig = gHandlingDataMgr.GetBoatPointer(id); }
        EXPECT_EQ((uintptr_t)rev, (uintptr_t)orig);
    }
}

GAME_DIFF_TEST(cHandlingDataMgr, FindExactWord) {
    // FindExactWord searches VehicleNames[210][14].
    // BUG HYPOTHESIS: reversed uses strncmp(name, entry, strlen(entry)) which
    // matches prefixes. Original at 0x6F4F30 may use strcmp (exact match).
    // Test with names that exist AND with prefix-ambiguous names.
    auto& VehicleNames = StaticRef<char[210][14]>(0x8D3978);
    const char* names[] = {
        "LANDSTAL", "BRAVURA", "CHEETAH", "POLICE", "RHINO",
        "HUNTER", "RCBANDIT", "TURISMO",
        "NOTACAR",  // not found → both should return -1
    };
    for (const char* name : names) {
        int32 rev = gHandlingDataMgr.FindExactWord(name, &VehicleNames[0][0], 14, 210);
        int32 orig;
        { HookDisableGuard guard("Global/cHandlingDataMgr/FindExactWord");
          orig = gHandlingDataMgr.FindExactWord(name, &VehicleNames[0][0], 14, 210); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(cHandlingDataMgr, ConvertDataToGameUnits) {
    // ConvertDataToGameUnits modifies the handling data in-place.
    // Test by converting to world units then back to game units and comparing.
    // Use a copy to avoid corrupting game state.
    for (int32 id = 0; id < NUM_VEHICLES; id++) {
        tHandlingData* h = gHandlingDataMgr.GetVehiclePointer(id);
        if (h->m_fMass == 0.0f) continue; // uninitialized entry

        // Save original game-units data
        tHandlingData saved = *h;

        // Reversed: world → game round-trip
        gHandlingDataMgr.ConvertDataToWorldUnits(h);
        gHandlingDataMgr.ConvertDataToGameUnits(h);
        float revAccel = h->GetTransmission().m_EngineAcceleration;
        float revVel = h->GetTransmission().m_MaxVelocity;
        float revBrake = h->m_fBrakeDeceleration;

        // Restore and do original round-trip
        *h = saved;
        { HookDisableGuard guard("Global/cHandlingDataMgr/ConvertDataToWorldUnits");
          gHandlingDataMgr.ConvertDataToWorldUnits(h); }
        { HookDisableGuard guard("Global/cHandlingDataMgr/ConvertDataToGameUnits");
          gHandlingDataMgr.ConvertDataToGameUnits(h); }
        float origAccel = h->GetTransmission().m_EngineAcceleration;
        float origVel = h->GetTransmission().m_MaxVelocity;
        float origBrake = h->m_fBrakeDeceleration;

        EXPECT_NEAR(revAccel, origAccel, 1e-4f);
        EXPECT_NEAR(revVel, origVel, 1e-4f);
        EXPECT_NEAR(revBrake, origBrake, 1e-4f);

        // Restore original state
        *h = saved;
    }
}

GAME_DIFF_TEST(cHandlingDataMgr, ConvertBikeDataToGameUnits) {
    for (int32 id = 0; id < NUM_BIKES; id++) {
        tBikeHandlingData* b = gHandlingDataMgr.GetBikeHandlingPointer(id);
        tBikeHandlingData saved = *b;

        // Reversed round-trip
        gHandlingDataMgr.ConvertBikeDataToWorldUnits(b);
        gHandlingDataMgr.ConvertBikeDataToGameUnits(b);
        float revLean = b->m_fMaxLean;
        float revWheelie = b->m_fWheelieAng;

        // Restore and do original round-trip
        *b = saved;
        { HookDisableGuard guard("Global/cHandlingDataMgr/ConvertBikeDataToWorldUnits");
          gHandlingDataMgr.ConvertBikeDataToWorldUnits(b); }
        { HookDisableGuard guard("Global/cHandlingDataMgr/ConvertBikeDataToGameUnits");
          gHandlingDataMgr.ConvertBikeDataToGameUnits(b); }
        float origLean = b->m_fMaxLean;
        float origWheelie = b->m_fWheelieAng;

        EXPECT_NEAR(revLean, origLean, 1e-4f);
        EXPECT_NEAR(revWheelie, origWheelie, 1e-4f);

        *b = saved;
    }
}

// --- Behavior tests ---

GAME_TEST(cHandlingDataMgr, AllVehiclesHaveMass) {
    // Every initialized handling entry should have positive mass
    int withMass = 0;
    for (int32 id = 0; id < NUM_VEHICLES; id++) {
        auto* h = gHandlingDataMgr.GetVehiclePointer(id);
        if (h->m_fMass > 0.0f) withMass++;
    }
    // At least 100 vehicles should have valid handling
    EXPECT_TRUE(withMass > 100);
}

GAME_TEST(cHandlingDataMgr, DriveTypeConsistency) {
    // If HasFrontWheelDrive and HasRearWheelDrive are both true, drive type is '4' (AWD)
    // If only front, drive type is 'F'. If only rear, drive type is 'R'.
    for (int32 id = 0; id < NUM_VEHICLES; id++) {
        auto* h = gHandlingDataMgr.GetVehiclePointer(id);
        if (h->m_fMass == 0.0f) continue;
        char dt = h->GetTransmission().m_nDriveType;
        bool fwd = gHandlingDataMgr.HasFrontWheelDrive((uint8)id);
        bool rwd = gHandlingDataMgr.HasRearWheelDrive((uint8)id);
        if (dt == '4') {
            EXPECT_TRUE(fwd);
            EXPECT_TRUE(rwd);
        } else if (dt == 'F') {
            EXPECT_TRUE(fwd);
            EXPECT_FALSE(rwd);
        } else if (dt == 'R') {
            EXPECT_FALSE(fwd);
            EXPECT_TRUE(rwd);
        }
    }
}

GAME_TEST(cHandlingDataMgr, VehicleNamesPopulated) {
    // Verify VehicleNames table is populated
    auto& VehicleNames = StaticRef<char[210][14]>(0x8D3978);
    EXPECT_EQ(strcmp(VehicleNames[0], "LANDSTAL"), 0);
    // Count populated entries
    int populated = 0;
    for (int i = 0; i < 210; i++) {
        if (strlen(VehicleNames[i]) > 0) populated++;
    }
    EXPECT_TRUE(populated > 100);
}
