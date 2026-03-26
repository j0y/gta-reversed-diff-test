// test_CAnimBlendSequence.cpp — Differential tests for CAnimBlendSequence.
// Hook paths: Animation/CAnimBlendSequence/<fn>
//
// CAnimBlendSequence stores keyframe data for a single bone in an animation.
// Get live sequences from the player ped's animation associations.

#include "StdInc.h"
#include "TestFramework.h"
#include "Animation/AnimBlendSequence.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendNode.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"

// Collect valid sequences from player ped's animations
static std::vector<CAnimBlendSequence*> GetPlayerSequences(int maxCount = 20) {
    std::vector<CAnimBlendSequence*> result;
    auto* player = FindPlayerPed(0);
    if (!player || !player->m_pRwClump) return result;

    auto* assoc = RpAnimBlendClumpGetFirstAssociation(player->m_pRwClump);
    while (assoc && (int)result.size() < maxCount) {
        for (auto& node : assoc->GetNodes()) {
            if (node.IsValid() && node.GetSeq()) {
                result.push_back(node.GetSeq());
                if ((int)result.size() >= maxCount) break;
            }
        }
        assoc = RpAnimBlendGetNextAssociation(assoc);
    }
    return result;
}

// ======================================================================
// GetDataSize — computes byte size of keyframe data
// Pure calculation: depends on m_FramesNum, m_bHasTranslation, compressed flag
// ======================================================================

GAME_DIFF_TEST(CAnimBlendSequence, GetDataSize_Uncompressed) {
    auto seqs = GetPlayerSequences();
    for (auto* seq : seqs) {
        if (seq->IsCompressed()) continue;

        size_t orig, rev;
        { HookDisableGuard guard("Animation/CAnimBlendSequence/GetDataSize");
          orig = seq->GetDataSize(false); }
        rev = seq->GetDataSize(false);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CAnimBlendSequence, GetDataSize_Compressed) {
    auto seqs = GetPlayerSequences();
    for (auto* seq : seqs) {
        // GetDataSize(true) computes compressed size regardless of current state
        size_t orig, rev;
        { HookDisableGuard guard("Animation/CAnimBlendSequence/GetDataSize");
          orig = seq->GetDataSize(true); }
        rev = seq->GetDataSize(true);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CAnimBlendSequence, GetDataSize_BothModes) {
    auto seqs = GetPlayerSequences();
    for (auto* seq : seqs) {
        // Compare uncompressed vs compressed size calculation
        size_t origU, revU, origC, revC;
        { HookDisableGuard guard("Animation/CAnimBlendSequence/GetDataSize");
          origU = seq->GetDataSize(false);
          origC = seq->GetDataSize(true); }
        revU = seq->GetDataSize(false);
        revC = seq->GetDataSize(true);

        EXPECT_EQ(origU, revU);
        EXPECT_EQ(origC, revC);
        // Compressed should be smaller or equal
        EXPECT_TRUE(revC <= revU);
    }
}

// ======================================================================
// GetUKeyFrame — get uncompressed keyframe by index
// Tests pointer arithmetic matches original
// ======================================================================

// GetUKeyFrame crashes in original code — the original has an assert(!m_bIsCompressed)
// that fires when sequences are compressed at runtime (most are after streaming).
// Tested indirectly via CAnimBlendNode::GetCurrentTranslation which calls GetUKeyFrame.

// ======================================================================
// GetCKeyFrame — get compressed keyframe by index
// ======================================================================

GAME_DIFF_TEST(CAnimBlendSequence, GetCKeyFrame) {
    auto seqs = GetPlayerSequences();
    for (auto* seq : seqs) {
        if (!seq->IsCompressed() || seq->m_FramesNum == 0) continue;

        size_t indices[] = { 0 };
        int numIndices = 1;
        if (seq->m_FramesNum > 2) {
            indices[numIndices++] = seq->m_FramesNum / 2;
        }
        if (seq->m_FramesNum > 1) {
            indices[numIndices++] = seq->m_FramesNum - 1;
        }

        for (int i = 0; i < numIndices; i++) {
            KeyFrameTransCompressed* orig;
            KeyFrameTransCompressed* rev;
            { HookDisableGuard guard("Animation/CAnimBlendSequence/GetCKeyFrame");
              orig = seq->GetCKeyFrame(indices[i]); }
            rev = seq->GetCKeyFrame(indices[i]);
            EXPECT_EQ(orig, rev);
        }
    }
}

// ======================================================================
// SetBoneTag / SetName — setter tests via raw memory comparison
// ======================================================================

GAME_DIFF_TEST(CAnimBlendSequence, SetBoneTag) {
    // Create a temporary sequence to test setters without corrupting live data
    CAnimBlendSequence origSeq{};
    CAnimBlendSequence revSeq{};

    eBoneTag32 tags[] = { BONE_HEAD, BONE_PELVIS, BONE_R_HAND, BONE_L_FOOT };
    for (auto tag : tags) {
        { HookDisableGuard guard("Animation/CAnimBlendSequence/SetBoneTag");
          origSeq.SetBoneTag(tag); }
        revSeq.SetBoneTag(tag);

        // Compare first 4 bytes (union: m_BoneTag or m_NameHashKey)
        EXPECT_EQ(memcmp(&origSeq, &revSeq, 4), 0);
    }
}

GAME_DIFF_TEST(CAnimBlendSequence, SetName) {
    CAnimBlendSequence origSeq{};
    CAnimBlendSequence revSeq{};

    const char* names[] = { "Root", "Pelvis", "Spine", "Head", "R_Hand", "" };
    for (auto* name : names) {
        { HookDisableGuard guard("Animation/CAnimBlendSequence/SetName");
          origSeq.SetName(name); }
        revSeq.SetName(name);

        // Compare hash key (stored in first 4 bytes of union)
        EXPECT_EQ(memcmp(&origSeq, &revSeq, 4), 0);
    }
}

// ======================================================================
// RemoveQuaternionFlips — fixes quaternion sign flips in keyframe data
// Test on a copy of live sequence data
// ======================================================================

GAME_DIFF_TEST(CAnimBlendSequence, RemoveQuaternionFlips) {
    auto seqs = GetPlayerSequences();
    for (auto* seq : seqs) {
        if (seq->IsCompressed() || seq->m_FramesNum <= 1) continue;
        if (!seq->m_bHasRotation) continue;

        // Snapshot keyframe data before
        size_t dataSize = seq->GetDataSize(false);
        if (dataSize == 0 || dataSize > 65536) continue;

        auto* backup = new uint8[dataSize];
        memcpy(backup, seq->m_Frames, dataSize);

        // Original
        { HookDisableGuard guard("Animation/CAnimBlendSequence/RemoveQuaternionFlips");
          seq->RemoveQuaternionFlips(); }
        auto* origData = new uint8[dataSize];
        memcpy(origData, seq->m_Frames, dataSize);

        // Restore and run reversed
        memcpy(seq->m_Frames, backup, dataSize);
        seq->RemoveQuaternionFlips();
        auto* revData = new uint8[dataSize];
        memcpy(revData, seq->m_Frames, dataSize);

        EXPECT_EQ(memcmp(origData, revData, dataSize), 0);

        // Restore original data
        memcpy(seq->m_Frames, backup, dataSize);
        delete[] backup;
        delete[] origData;
        delete[] revData;

        break; // One sequence is enough — this is an expensive test
    }
}

// ======================================================================
// Behavior tests
// ======================================================================

GAME_TEST(CAnimBlendSequence, PlayerHasSequences) {
    auto seqs = GetPlayerSequences();
    EXPECT_TRUE(seqs.size() > 0);
}

GAME_TEST(CAnimBlendSequence, SequenceProperties) {
    auto seqs = GetPlayerSequences();
    int withTranslation = 0;
    int withRotation = 0;
    int compressed = 0;

    for (auto* seq : seqs) {
        if (seq->HasTranslation()) withTranslation++;
        if (seq->m_bHasRotation) withRotation++;
        if (seq->IsCompressed()) compressed++;
        // Frame count should be reasonable
        EXPECT_TRUE(seq->m_FramesNum > 0);
        EXPECT_TRUE(seq->m_FramesNum < 10000);
    }

    // Most sequences should have rotation
    EXPECT_TRUE(withRotation > 0);
}

GAME_TEST(CAnimBlendSequence, GetDataSize_Consistent) {
    auto seqs = GetPlayerSequences();
    for (auto* seq : seqs) {
        size_t uncomp = seq->GetDataSize(false);
        size_t comp = seq->GetDataSize(true);
        // Both should be positive
        EXPECT_TRUE(uncomp > 0);
        EXPECT_TRUE(comp > 0);
        // Compressed <= uncompressed
        EXPECT_TRUE(comp <= uncomp);
    }
}
