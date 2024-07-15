// Oleg Kotov

#pragma once

#include "point.h"

class Circle
{
public:

	Circle() = default;

	Circle( float x, float y, float radius )
	{
		this->x = x;
		this->y = y;
		this->radius = radius;
	}

public:

	bool contains( const Point& point ) const
	{
		float dx = point.x - x;
		float dy = point.y - y;

		float distanceSqr = dx * dx + dy * dy;

		return distanceSqr <= radius * radius;
	}

public:

	float x = 0.0f;
	float y = 0.0f;
	float radius = 0.5f;
};

