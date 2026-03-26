// test_BoneNode.cpp — Differential tests for BoneNode_c.
// Hook paths: Global/BoneNode_c/<fn>
//
// BoneNode_c handles skeletal animation math — quaternion/euler conversions
// and bone tag lookups. Static pure-math functions, no instance needed.
// EulerToQuat/QuatToEuler internally use sin/cos — these tests catch
// any divergence between reversed and original trig implementations.

#include "StdInc.h"
#include "TestFramework.h"
#include "BoneNode_c.h"

// ======================================================================
// GetIdFromBoneTag — maps eBoneTag32 to internal bone ID
// ======================================================================

GAME_DIFF_TEST(BoneNode, GetIdFromBoneTag_AllBones) {
    eBoneTag32 tags[] = {
        BONE_ROOT, BONE_PELVIS, BONE_SPINE, BONE_SPINE1, BONE_NECK, BONE_HEAD,
        BONE_L_BROW, BONE_R_BROW, BONE_JAW,
        BONE_R_CLAVICLE, BONE_R_UPPER_ARM, BONE_R_FORE_ARM, BONE_R_HAND,
        BONE_R_FINGER, BONE_R_FINGER_01,
        BONE_L_CLAVICLE, BONE_L_UPPER_ARM, BONE_L_FORE_ARM, BONE_L_HAND,
        BONE_L_FINGER, BONE_L_FINGER_01,
        BONE_L_THIGH, BONE_L_CALF, BONE_L_FOOT, BONE_L_TOE_0,
        BONE_R_THIGH, BONE_R_CALF, BONE_R_FOOT, BONE_R_TOE_0,
    };
    for (auto tag : tags) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/BoneNode_c/GetIdFromBoneTag");
          orig = BoneNode_c::GetIdFromBoneTag(tag); }
        rev = BoneNode_c::GetIdFromBoneTag(tag);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(BoneNode, GetIdFromBoneTag_Invalid) {
    // Use BONE_UNKNOWN and other known-invalid values
    eBoneTag32 tags[] = { BONE_UNKNOWN };
    for (auto tag : tags) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/BoneNode_c/GetIdFromBoneTag");
          orig = BoneNode_c::GetIdFromBoneTag(tag); }
        rev = BoneNode_c::GetIdFromBoneTag(tag);
        EXPECT_EQ(orig, rev);
    }
}

// ======================================================================
// EulerToQuat — euler angles (radians) → quaternion
// Uses sin/cos internally — catches trig divergence
// ======================================================================

GAME_DIFF_TEST(BoneNode, EulerToQuat_Cardinals) {
    CVector eulers[] = {
        {0.f, 0.f, 0.f},
        {PI / 2.f, 0.f, 0.f},
        {0.f, PI / 2.f, 0.f},
        {0.f, 0.f, PI / 2.f},
        {PI, 0.f, 0.f},
        {0.f, PI, 0.f},
        {0.f, 0.f, PI},
    };
    for (auto& e : eulers) {
        RtQuat origQ{}, revQ{};
        { HookDisableGuard guard("Global/BoneNode_c/EulerToQuat");
          BoneNode_c::EulerToQuat(e, origQ); }
        BoneNode_c::EulerToQuat(e, revQ);
        EXPECT_NEAR(origQ.imag.x, revQ.imag.x, 1e-5f);
        EXPECT_NEAR(origQ.imag.y, revQ.imag.y, 1e-5f);
        EXPECT_NEAR(origQ.imag.z, revQ.imag.z, 1e-5f);
        EXPECT_NEAR(origQ.real,   revQ.real,   1e-5f);
    }
}

GAME_DIFF_TEST(BoneNode, EulerToQuat_Arbitrary) {
    CVector eulers[] = {
        {0.1f, 0.2f, 0.3f},
        {-0.5f, 1.0f, -0.3f},
        {1.57f, 0.78f, 3.14f},
        {0.001f, 0.001f, 0.001f},
        {-PI, -PI / 2.f, PI / 4.f},
    };
    for (auto& e : eulers) {
        RtQuat origQ{}, revQ{};
        { HookDisableGuard guard("Global/BoneNode_c/EulerToQuat");
          BoneNode_c::EulerToQuat(e, origQ); }
        BoneNode_c::EulerToQuat(e, revQ);
        EXPECT_NEAR(origQ.imag.x, revQ.imag.x, 1e-4f);
        EXPECT_NEAR(origQ.imag.y, revQ.imag.y, 1e-4f);
        EXPECT_NEAR(origQ.imag.z, revQ.imag.z, 1e-4f);
        EXPECT_NEAR(origQ.real,   revQ.real,   1e-4f);
    }
}

// ======================================================================
// QuatToEuler — quaternion → euler angles
// ======================================================================

GAME_DIFF_TEST(BoneNode, QuatToEuler_Cardinals) {
    RtQuat quats[] = {
        {{0.f, 0.f, 0.f}, 1.f},                    // identity
        {{0.7071f, 0.f, 0.f}, 0.7071f},            // 90° X
        {{0.f, 0.7071f, 0.f}, 0.7071f},            // 90° Y
        {{0.f, 0.f, 0.7071f}, 0.7071f},            // 90° Z
        {{0.01f, 0.02f, 0.03f}, 0.9993f},          // small rotation
    };
    for (auto& q : quats) {
        CVector origE{}, revE{};
        { HookDisableGuard guard("Global/BoneNode_c/QuatToEuler");
          BoneNode_c::QuatToEuler(q, origE); }
        BoneNode_c::QuatToEuler(q, revE);
        EXPECT_NEAR(origE.x, revE.x, 1e-4f);
        EXPECT_NEAR(origE.y, revE.y, 1e-4f);
        EXPECT_NEAR(origE.z, revE.z, 1e-4f);
    }
}

GAME_DIFF_TEST(BoneNode, QuatToEuler_Arbitrary) {
    // Note: uses std::sin/cos internally — original uses MSVC 2004 CRT,
    // reversed uses C++23. Tolerance must accommodate ~1e-4 trig divergence
    // amplified by atan2/asin in the euler extraction.
    RtQuat quats[] = {
        {{0.5f, 0.5f, 0.5f}, 0.5f},                // 120° around (1,1,1)
        {{-0.3f, 0.6f, -0.1f}, 0.7348f},           // arbitrary
        {{0.f, 0.f, 1.f}, 0.f},                     // 180° Z
        {{0.9239f, 0.f, 0.f}, 0.3827f},             // 135° X
    };
    for (auto& q : quats) {
        CVector origE{}, revE{};
        { HookDisableGuard guard("Global/BoneNode_c/QuatToEuler");
          BoneNode_c::QuatToEuler(q, origE); }
        BoneNode_c::QuatToEuler(q, revE);
        EXPECT_NEAR(origE.x, revE.x, 1e-3f);
        EXPECT_NEAR(origE.y, revE.y, 1e-3f);
        EXPECT_NEAR(origE.z, revE.z, 1e-3f);
    }
}

// ======================================================================
// Round-trip: EulerToQuat → QuatToEuler
// Original round-trip vs reversed round-trip
// ======================================================================

GAME_DIFF_TEST(BoneNode, EulerQuatRoundTrip) {
    CVector inputs[] = {
        {0.f, 0.f, 0.f},
        {0.3f, 0.5f, 0.1f},
        {-0.2f, 0.8f, -0.4f},
        {1.0f, -0.5f, 0.7f},
    };
    for (auto& euler : inputs) {
        // Original round-trip
        RtQuat origQ{};
        CVector origResult{};
        { HookDisableGuard guard("Global/BoneNode_c/EulerToQuat");
          BoneNode_c::EulerToQuat(euler, origQ); }
        { HookDisableGuard guard("Global/BoneNode_c/QuatToEuler");
          BoneNode_c::QuatToEuler(origQ, origResult); }

        // Reversed round-trip
        RtQuat revQ{};
        CVector revResult{};
        BoneNode_c::EulerToQuat(euler, revQ);
        BoneNode_c::QuatToEuler(revQ, revResult);

        EXPECT_NEAR(origResult.x, revResult.x, 1e-4f);
        EXPECT_NEAR(origResult.y, revResult.y, 1e-4f);
        EXPECT_NEAR(origResult.z, revResult.z, 1e-4f);
    }
}

// ======================================================================
// Behavior tests
// ======================================================================

GAME_TEST(BoneNode, EulerToQuat_Identity) {
    CVector zero(0.f, 0.f, 0.f);
    RtQuat q{};
    BoneNode_c::EulerToQuat(zero, q);
    EXPECT_NEAR(q.imag.x, 0.f, 1e-5f);
    EXPECT_NEAR(q.imag.y, 0.f, 1e-5f);
    EXPECT_NEAR(q.imag.z, 0.f, 1e-5f);
    EXPECT_NEAR(q.real, 1.f, 1e-5f);
}

GAME_TEST(BoneNode, EulerToQuat_UnitLength) {
    CVector eulers[] = {
        {0.1f, 0.2f, 0.3f}, {1.0f, 0.f, 0.f}, {0.f, -1.5f, 0.8f},
    };
    for (auto& e : eulers) {
        RtQuat q{};
        BoneNode_c::EulerToQuat(e, q);
        float len = q.imag.x * q.imag.x + q.imag.y * q.imag.y +
                    q.imag.z * q.imag.z + q.real * q.real;
        EXPECT_NEAR(len, 1.0f, 1e-3f);
    }
}

GAME_TEST(BoneNode, GetIdFromBoneTag_KnownMappings) {
    EXPECT_EQ(BoneNode_c::GetIdFromBoneTag(BONE_ROOT), 0);
    int32 pelvisId = BoneNode_c::GetIdFromBoneTag(BONE_PELVIS);
    int32 headId = BoneNode_c::GetIdFromBoneTag(BONE_HEAD);
    EXPECT_NE(pelvisId, headId);
}
