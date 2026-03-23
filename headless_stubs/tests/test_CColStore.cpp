// test_CColStore.cpp — Deep differential tests for CColStore.
// Hook paths: Global/CColStore/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Collision/ColStore.h"

// --- HasCollisionLoaded ---

// HasCollisionLoaded hook path not found at runtime.
// CColStore uses a different hook category — needs investigation.
GAME_TEST(CColStore, HasCollisionLoaded_Behavior) {
    CVector pos(2495.0f, -1665.0f, 13.5f);
    bool loaded = CColStore::HasCollisionLoaded(pos, 0);
    // At state 9 near Grove Street, collision should be loaded
    EXPECT_TRUE(loaded);
}
