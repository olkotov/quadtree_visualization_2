// Oleg Kotov

#pragma once

#include "mathutils.h"
#include <math.h>

struct Params
{
	const float sectorAngleInDegrees = 135.5f;
	const float distance = 50.0f;

	const float sectorCosine = cosf( sectorAngleInDegrees * 0.5f * Deg2Rad ); // +- half of angle
	const float distanceSqr = distance * distance;

	bool useSquaredDistance = true;
	bool useSpatialPartition = true;
	bool useMultithreading = true;
};

extern Params params;

