// ScenarioHelpers.h — Helpers for creating game objects in tests.
// Provides RAII wrappers that create objects and clean them up automatically.

#pragma once

#include "StdInc.h"
#include "Streaming.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/CivilianPed.h"
#include "World.h"

namespace Scenario {

// Find a loaded vehicle model ID, or -1 if none found
inline int32 FindLoadedVehicleModel() {
    // Common vehicles likely loaded during init
    int32 candidates[] = { 596, 597, 598, 599, // police cars
                           400, 401, 402, 404, 410, 420, 436, 445, 451,
                           480, 500, 560, 580 };
    for (int32 id : candidates) {
        if (CStreaming::IsModelLoaded(id)) return id;
    }
    // Brute force search 400-611
    for (int32 id = 400; id <= 611; id++) {
        if (CStreaming::IsModelLoaded(id)) return id;
    }
    return -1;
}

// Find a loaded ped model ID (non-player), or -1
inline int32 FindLoadedPedModel() {
    // Common ped models
    int32 candidates[] = { 7, 9, 10, 11, 12, 19, 20, 21, 22, 23, 24, 25, 26 };
    for (int32 id : candidates) {
        if (CStreaming::IsModelLoaded(id)) return id;
    }
    for (int32 id = 1; id <= 312; id++) {
        if (CStreaming::IsModelLoaded(id)) return id;
    }
    return -1;
}

// RAII vehicle wrapper — creates on construction, removes on destruction
struct TestVehicle {
    CAutomobile* ptr = nullptr;

    TestVehicle(int32 modelId, CVector pos) {
        if (modelId < 0 || !CStreaming::IsModelLoaded(modelId)) return;
        ptr = new CAutomobile(modelId, RANDOM_VEHICLE, true);
        if (ptr) {
            ptr->SetPosn(pos);
            ptr->SetStatus(STATUS_ABANDONED);
            CWorld::Add(ptr);
        }
    }

    ~TestVehicle() {
        if (ptr) {
            CWorld::Remove(ptr);
            delete ptr;
            ptr = nullptr;
        }
    }

    operator bool() const { return ptr != nullptr; }
    CAutomobile* operator->() const { return ptr; }
    CVehicle* AsVehicle() const { return static_cast<CVehicle*>(ptr); }

    // Non-copyable
    TestVehicle(const TestVehicle&) = delete;
    TestVehicle& operator=(const TestVehicle&) = delete;
};

// RAII ped wrapper
struct TestPed {
    CCivilianPed* ptr = nullptr;

    TestPed(ePedType pedType, int32 modelId, CVector pos) {
        if (modelId < 0 || !CStreaming::IsModelLoaded(modelId)) return;
        ptr = new CCivilianPed(pedType, modelId);
        if (ptr) {
            ptr->SetPosn(pos);
            CWorld::Add(ptr);
        }
    }

    ~TestPed() {
        if (ptr) {
            CWorld::Remove(ptr);
            delete ptr;
            ptr = nullptr;
        }
    }

    operator bool() const { return ptr != nullptr; }
    CCivilianPed* operator->() const { return ptr; }
    CPed* AsPed() const { return static_cast<CPed*>(ptr); }

    // Non-copyable
    TestPed(const TestPed&) = delete;
    TestPed& operator=(const TestPed&) = delete;
};

} // namespace Scenario
