// test_CPedGeometryAnalyser.cpp — Differential tests for CPedGeometryAnalyser.
// Hook paths: Global/CPedGeometryAnalyser/<fn>
//
// CanPedTargetPed crashes in original code on spawned peds — skipped.
// ComputeClosestSurfacePoint is overloaded — hook path not found. Skipped.
// ComputeEntityBoundingBox* functions take (float, ...) not (CPed&, ...).
//
// Only CanPedTargetPed is both reversed and takes ped refs, but crashes.
// This file is kept as a placeholder for future tests if crash is resolved.

#include "StdInc.h"
#include "TestFramework.h"
#include "PedGeometryAnalyser.h"
