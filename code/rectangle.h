// Oleg Kotov

#pragma once

#include "point.h"
#include "circle.h"

#include <algorithm>

class Rectangle
{
public:

	Rectangle() = default;

	Rectangle( float x, float y, float width, float height )
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

public:

	bool contains( const Point& point ) const
	{
		return point.x >= x && point.x <= x + width &&
			   point.y >= y && point.y <= y + height;
	}

	bool intersects( const Rectangle& box ) const
	{
		return x < box.x + box.width && x + width > box.x &&
			   y < box.y + box.height && y + height > box.y;
	}

	bool intersects( const Circle& circle ) const
	{
		float closestX = std::max( x, std::min( circle.x, x + width ) );
		float closestY = std::max( y, std::min( circle.y, y + height ) );

		float dx = closestX - circle.x;
		float dy = closestY - circle.y;

		float distanceSqr = dx * dx + dy * dy;

		return distanceSqr <= circle.radius * circle.radius;
	}

public:

	float x = 0.0f;
	float y = 0.0f;
	float width = 1.0f;
	float height = 1.0f;
};

