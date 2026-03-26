// test_FxSystem_Layout.cpp — Tests that FxSystem_c struct layout matches original.
//
// The reversed code declares m_nRateMult, m_nTimeMult, m_nConstTime as uint16
// with float→uint16 scaling (mult*1000 or time*256). If the original stores
// these as float or uses different scaling, the raw memory will differ.
//
// Strategy: call original setter, snapshot raw bytes around the member,
// call reversed setter on a fresh system, compare byte-for-byte.
// This catches: wrong member type, wrong offset, wrong scaling factor.

#include "StdInc.h"
#include "TestFramework.h"
#include "Fx/FxSystem.h"
#include "Fx/FxManager.h"

// Snapshot a range of bytes from an FxSystem relative to its base
static void SnapshotBytes(FxSystem_c* sys, int offset, int size, uint8* out) {
    memcpy(out, (uint8*)sys + offset, size);
}

static bool BytesMatch(const uint8* a, const uint8* b, int size) {
    return memcmp(a, b, size) == 0;
}

static const char* FindWorkingFxNameLayout() {
    const char* names[] = { "blood_heli", "gunflash", "exhale", "cigsmoke", "water_splash" };
    for (auto* n : names) {
        if (g_fxMan.FindFxSystemBP(n)) return n;
    }
    return nullptr;
}

// RAII wrapper
struct LayoutTestFx {
    FxSystem_c* sys = nullptr;
    LayoutTestFx(const char* name) { sys = g_fxMan.CreateFxSystem(name, CVector(0,0,10), nullptr, true); }
    ~LayoutTestFx() { if (sys) sys->Kill(); }
    operator bool() const { return sys != nullptr; }
};

// ======================================================================
// SetRateMult — compare raw bytes at m_nRateMult offset
// Reversed: m_nRateMult = (uint16)(mult * 1000.0f) at offset of m_nRateMult
// If original stores float (4 bytes) vs uint16 (2 bytes), sizes will differ.
// ======================================================================

GAME_DIFF_TEST(FxSystem_Layout, SetRateMult_RawBytes) {
    auto* fxName = FindWorkingFxNameLayout();
    if (!fxName) return;

    // Compute offset of m_nRateMult from FxSystem_c base
    FxSystem_c dummy{};
    int offset = (int)((uint8*)&dummy.m_nRateMult - (uint8*)&dummy);
    // Snapshot 8 bytes around the member (catches overflow into adjacent fields)
    constexpr int SNAP_SIZE = 8;

    float testValues[] = { 0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 5.0f, 10.0f };
    for (float val : testValues) {
        uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];

        // Original setter
        LayoutTestFx fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetRateMult");
          fx1.sys->SetRateMult(val); }
        SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);

        // Reversed setter
        LayoutTestFx fx2(fxName);
        if (!fx2) return;
        fx2.sys->SetRateMult(val);
        SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);

        if (!BytesMatch(origSnap, revSnap, SNAP_SIZE)) {
            char buf[256];
            _snprintf(buf, sizeof(buf),
                "SetRateMult(%.1f): raw bytes differ at offset %d. "
                "orig=[%02X %02X %02X %02X %02X %02X %02X %02X] "
                "rev=[%02X %02X %02X %02X %02X %02X %02X %02X]",
                val, offset,
                origSnap[0], origSnap[1], origSnap[2], origSnap[3],
                origSnap[4], origSnap[5], origSnap[6], origSnap[7],
                revSnap[0], revSnap[1], revSnap[2], revSnap[3],
                revSnap[4], revSnap[5], revSnap[6], revSnap[7]);
            GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
        }
    }
}

// ======================================================================
// SetTimeMult — same approach
// ======================================================================

GAME_DIFF_TEST(FxSystem_Layout, SetTimeMult_RawBytes) {
    auto* fxName = FindWorkingFxNameLayout();
    if (!fxName) return;

    FxSystem_c dummy{};
    int offset = (int)((uint8*)&dummy.m_nTimeMult - (uint8*)&dummy);
    // Only compare the 2 bytes of m_nTimeMult itself — adjacent bitfields
    // may have different initial state between the two system instances.
    constexpr int SNAP_SIZE = 2;

    float testValues[] = { 0.0f, 0.5f, 1.0f, 2.0f, 5.0f };
    for (float val : testValues) {
        uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];

        LayoutTestFx fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetTimeMult");
          fx1.sys->SetTimeMult(val); }
        SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);

        LayoutTestFx fx2(fxName);
        if (!fx2) return;
        fx2.sys->SetTimeMult(val);
        SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);

        if (!BytesMatch(origSnap, revSnap, SNAP_SIZE)) {
            char buf[256];
            _snprintf(buf, sizeof(buf),
                "SetTimeMult(%.1f): m_nTimeMult bytes differ at offset %d. "
                "orig=[%02X %02X] rev=[%02X %02X]. "
                "Wrong type (float vs uint16) or wrong scaling factor?",
                val, offset,
                origSnap[0], origSnap[1],
                revSnap[0], revSnap[1]);
            GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
        }
    }
}

// Also test the wider region separately to catch adjacent field corruption
GAME_DIFF_TEST(FxSystem_Layout, SetTimeMult_AdjacentFields) {
    auto* fxName = FindWorkingFxNameLayout();
    if (!fxName) return;

    FxSystem_c dummy{};
    // Check: does original SetTimeMult write ONLY m_nTimeMult, or also adjacent bytes?
    // Zero the region in both systems first, then call setter, compare wider region.
    int offset = (int)((uint8*)&dummy.m_nTimeMult - (uint8*)&dummy);
    constexpr int SNAP_SIZE = 8;

    LayoutTestFx fx1(fxName), fx2(fxName);
    if (!fx1 || !fx2) return;

    // Zero the 8-byte region in both
    memset((uint8*)fx1.sys + offset, 0, SNAP_SIZE);
    memset((uint8*)fx2.sys + offset, 0, SNAP_SIZE);

    float val = 2.5f;
    { HookDisableGuard guard("Fx/FxSystem_c/SetTimeMult");
      fx1.sys->SetTimeMult(val); }
    fx2.sys->SetTimeMult(val);

    uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];
    SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);
    SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);

    if (!BytesMatch(origSnap, revSnap, SNAP_SIZE)) {
        char buf[256];
        _snprintf(buf, sizeof(buf),
            "SetTimeMult(%.1f) with zeroed region: "
            "orig=[%02X %02X %02X %02X %02X %02X %02X %02X] "
            "rev=[%02X %02X %02X %02X %02X %02X %02X %02X]. "
            "Original may write more bytes than reversed (type mismatch?).",
            val,
            origSnap[0], origSnap[1], origSnap[2], origSnap[3],
            origSnap[4], origSnap[5], origSnap[6], origSnap[7],
            revSnap[0], revSnap[1], revSnap[2], revSnap[3],
            revSnap[4], revSnap[5], revSnap[6], revSnap[7]);
        GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
    }
}

// ======================================================================
// SetConstTime — m_UseConstTime (bool) + m_nConstTime (uint16)
// ======================================================================

GAME_DIFF_TEST(FxSystem_Layout, SetConstTime_RawBytes) {
    auto* fxName = FindWorkingFxNameLayout();
    if (!fxName) return;

    FxSystem_c dummy{};
    // Snapshot from m_UseConstTime through m_nConstTime + padding
    int offset = (int)((uint8*)&dummy.m_UseConstTime - (uint8*)&dummy);
    constexpr int SNAP_SIZE = 16; // covers bool + float + float + uint16

    struct Case { bool on; float time; };
    Case cases[] = { {false, 0.f}, {true, 0.5f}, {true, 1.0f}, {true, 0.1f}, {true, 10.0f} };

    for (auto& c : cases) {
        uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];

        LayoutTestFx fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetConstTime");
          fx1.sys->SetConstTime(c.on, c.time); }
        SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);

        LayoutTestFx fx2(fxName);
        if (!fx2) return;
        fx2.sys->SetConstTime(c.on, c.time);
        SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);

        if (!BytesMatch(origSnap, revSnap, SNAP_SIZE)) {
            char buf[512];
            _snprintf(buf, sizeof(buf),
                "SetConstTime(%s, %.1f): raw bytes differ at offset %d. "
                "orig=[%02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X %02X] "
                "rev=[%02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X %02X  %02X %02X %02X %02X]",
                c.on ? "true" : "false", c.time, offset,
                origSnap[0], origSnap[1], origSnap[2], origSnap[3],
                origSnap[4], origSnap[5], origSnap[6], origSnap[7],
                origSnap[8], origSnap[9], origSnap[10], origSnap[11],
                origSnap[12], origSnap[13], origSnap[14], origSnap[15],
                revSnap[0], revSnap[1], revSnap[2], revSnap[3],
                revSnap[4], revSnap[5], revSnap[6], revSnap[7],
                revSnap[8], revSnap[9], revSnap[10], revSnap[11],
                revSnap[12], revSnap[13], revSnap[14], revSnap[15]);
            GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
        }
    }
}

// ======================================================================
// SetLocalParticles / SetMustCreatePrts — bitfield setters
// These are packed bitfields — particularly sensitive to offset/order bugs
// ======================================================================

GAME_DIFF_TEST(FxSystem_Layout, SetLocalParticles_RawBytes) {
    auto* fxName = FindWorkingFxNameLayout();
    if (!fxName) return;

    // Bitfields start after m_nTimeMult. Compute offset from m_nTimeMult + sizeof(uint16).
    FxSystem_c dummy{};
    int offset = (int)((uint8*)&dummy.m_nTimeMult - (uint8*)&dummy) + sizeof(uint16);
    constexpr int SNAP_SIZE = 4;

    for (bool val : {false, true}) {
        uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];

        LayoutTestFx fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetLocalParticles");
          fx1.sys->SetLocalParticles(val); }
        SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);

        LayoutTestFx fx2(fxName);
        if (!fx2) return;
        fx2.sys->SetLocalParticles(val);
        SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);

        if (!BytesMatch(origSnap, revSnap, SNAP_SIZE)) {
            char buf[256];
            _snprintf(buf, sizeof(buf),
                "SetLocalParticles(%s): bitfield bytes differ at offset %d. "
                "orig=[%02X %02X %02X %02X] rev=[%02X %02X %02X %02X]",
                val ? "true" : "false", offset,
                origSnap[0], origSnap[1], origSnap[2], origSnap[3],
                revSnap[0], revSnap[1], revSnap[2], revSnap[3]);
            GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
        }
    }
}

GAME_DIFF_TEST(FxSystem_Layout, SetMustCreatePrts_RawBytes) {
    auto* fxName = FindWorkingFxNameLayout();
    if (!fxName) return;

    FxSystem_c dummy{};
    int offset = (int)((uint8*)&dummy.m_nTimeMult - (uint8*)&dummy) + sizeof(uint16);
    constexpr int SNAP_SIZE = 4;

    for (bool val : {false, true}) {
        uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];

        LayoutTestFx fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetMustCreatePrts");
          fx1.sys->SetMustCreatePrts(val); }
        SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);

        LayoutTestFx fx2(fxName);
        if (!fx2) return;
        fx2.sys->SetMustCreatePrts(val);
        SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);

        if (!BytesMatch(origSnap, revSnap, SNAP_SIZE)) {
            char buf[256];
            _snprintf(buf, sizeof(buf),
                "SetMustCreatePrts(%s): bitfield bytes differ at offset %d. "
                "orig=[%02X %02X %02X %02X] rev=[%02X %02X %02X %02X]",
                val ? "true" : "false", offset,
                origSnap[0], origSnap[1], origSnap[2], origSnap[3],
                revSnap[0], revSnap[1], revSnap[2], revSnap[3]);
            GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
        }
    }
}

// ======================================================================
// SetOffsetPos — writes to m_LocalMatrix.pos (RwMatrix position row)
// ======================================================================

GAME_DIFF_TEST(FxSystem_Layout, SetOffsetPos_RawBytes) {
    auto* fxName = FindWorkingFxNameLayout();
    if (!fxName) return;

    FxSystem_c dummy{};
    // m_LocalMatrix is an RwMatrix — 64 bytes. Snapshot the whole thing.
    int offset = (int)((uint8*)&dummy.m_LocalMatrix - (uint8*)&dummy);
    constexpr int SNAP_SIZE = 64;

    CVector positions[] = {
        {0.f, 0.f, 0.f}, {1.f, 2.f, 3.f}, {-100.f, 50.f, 0.001f},
    };

    for (auto& pos : positions) {
        uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];

        LayoutTestFx fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetOffsetPos");
          fx1.sys->SetOffsetPos(pos); }
        SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);

        LayoutTestFx fx2(fxName);
        if (!fx2) return;
        fx2.sys->SetOffsetPos(pos);
        SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);

        if (!BytesMatch(origSnap, revSnap, SNAP_SIZE)) {
            // Find first differing byte
            int diffAt = -1;
            for (int i = 0; i < SNAP_SIZE; i++) {
                if (origSnap[i] != revSnap[i]) { diffAt = i; break; }
            }
            char buf[256];
            _snprintf(buf, sizeof(buf),
                "SetOffsetPos(%.1f,%.1f,%.1f): matrix bytes differ. "
                "First diff at offset+%d: orig=%02X rev=%02X",
                pos.x, pos.y, pos.z, diffAt,
                diffAt >= 0 ? origSnap[diffAt] : 0,
                diffAt >= 0 ? revSnap[diffAt] : 0);
            GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
        }
    }
}

// ======================================================================
// SetVelAdd — writes to m_VelAdd (CVector)
// ======================================================================

GAME_DIFF_TEST(FxSystem_Layout, SetVelAdd_RawBytes) {
    auto* fxName = FindWorkingFxNameLayout();
    if (!fxName) return;

    FxSystem_c dummy{};
    int offset = (int)((uint8*)&dummy.m_VelAdd - (uint8*)&dummy);
    constexpr int SNAP_SIZE = 12; // CVector = 3 floats

    CVector velocities[] = {
        {0.f, 0.f, 0.f}, {0.1f, 0.2f, 0.3f}, {-1.f, 0.f, 5.f},
    };

    for (auto& vel : velocities) {
        uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];

        LayoutTestFx fx1(fxName);
        if (!fx1) return;
        { HookDisableGuard guard("Fx/FxSystem_c/SetVelAdd");
          fx1.sys->SetVelAdd(&vel); }
        SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);

        LayoutTestFx fx2(fxName);
        if (!fx2) return;
        fx2.sys->SetVelAdd(&vel);
        SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);

        if (!BytesMatch(origSnap, revSnap, SNAP_SIZE)) {
            char buf[256];
            _snprintf(buf, sizeof(buf),
                "SetVelAdd(%.1f,%.1f,%.1f): velocity bytes differ at offset %d.",
                vel.x, vel.y, vel.z, offset);
            GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
        }
    }
}

// ======================================================================
// Play/Stop/Pause — check m_nPlayStatus raw byte + offset
// ======================================================================

GAME_DIFF_TEST(FxSystem_Layout, PlayStatus_RawBytes) {
    auto* fxName = FindWorkingFxNameLayout();
    if (!fxName) return;

    FxSystem_c dummy{};
    int offset = (int)((uint8*)&dummy.m_nPlayStatus - (uint8*)&dummy);
    constexpr int SNAP_SIZE = 4; // status + kill status + padding

    // Test: New → Play → Pause → Stop
    struct Step { const char* hookName; void (FxSystem_c::*fn)(); const char* label; };
    // We'll test each transition separately

    // New system (no action)
    {
        uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];
        LayoutTestFx fx1(fxName), fx2(fxName);
        if (!fx1 || !fx2) return;
        SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);
        SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);
        EXPECT_TRUE(BytesMatch(origSnap, revSnap, SNAP_SIZE));
    }

    // After Play
    {
        uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];
        LayoutTestFx fx1(fxName), fx2(fxName);
        if (!fx1 || !fx2) return;
        { HookDisableGuard guard("Fx/FxSystem_c/Play"); fx1.sys->Play(); }
        fx2.sys->Play();
        SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);
        SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);
        if (!BytesMatch(origSnap, revSnap, SNAP_SIZE)) {
            char buf[128];
            _snprintf(buf, sizeof(buf), "After Play: status bytes differ. orig=[%02X %02X %02X %02X] rev=[%02X %02X %02X %02X]",
                origSnap[0], origSnap[1], origSnap[2], origSnap[3],
                revSnap[0], revSnap[1], revSnap[2], revSnap[3]);
            GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
        }
    }

    // After Stop
    {
        uint8 origSnap[SNAP_SIZE], revSnap[SNAP_SIZE];
        LayoutTestFx fx1(fxName), fx2(fxName);
        if (!fx1 || !fx2) return;
        fx1.sys->Play(); fx2.sys->Play();
        { HookDisableGuard guard("Fx/FxSystem_c/Stop"); fx1.sys->Stop(); }
        fx2.sys->Stop();
        SnapshotBytes(fx1.sys, offset, SNAP_SIZE, origSnap);
        SnapshotBytes(fx2.sys, offset, SNAP_SIZE, revSnap);
        if (!BytesMatch(origSnap, revSnap, SNAP_SIZE)) {
            char buf[128];
            _snprintf(buf, sizeof(buf), "After Stop: status bytes differ. orig=[%02X %02X %02X %02X] rev=[%02X %02X %02X %02X]",
                origSnap[0], origSnap[1], origSnap[2], origSnap[3],
                revSnap[0], revSnap[1], revSnap[2], revSnap[3]);
            GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
        }
    }
}

// ======================================================================
// Struct size validation
// ======================================================================

GAME_TEST(FxSystem_Layout, StructSize) {
    // If the struct size is wrong, every offset after the first wrong
    // member will be shifted, causing subtle bugs.
    // The original struct at the hooked addresses expects a specific size.
    // VALIDATE_SIZE in the source should catch this at compile time,
    // but let's also verify at runtime.
    size_t sz = sizeof(FxSystem_c);
    // Log the size for reference
    char buf[128];
    _snprintf(buf, sizeof(buf), "sizeof(FxSystem_c) = %u bytes", (uint32)sz);
    // Just ensure it's reasonable (not 0, not absurdly large)
    EXPECT_TRUE(sz > 64);
    EXPECT_TRUE(sz < 4096);
}
