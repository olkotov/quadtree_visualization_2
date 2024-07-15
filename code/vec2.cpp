// Oleg Kotov

#include "vec2.h"
#include <math.h>

Vec2::Vec2()
{
	x = 0.0f;
	y = 0.0f;
}

Vec2::Vec2( float x, float y )
{
	this->x = x;
	this->y = y;
}

Vec2 Vec2::operator-( const Vec2& v ) const
{
	return Vec2( x - v.x, y - v.y );
}

float Vec2::length() const
{
    return sqrtf( x * x + y * y );
}

float Vec2::lengthSq() const
{
	return x * x + y * y;
}

Vec2 Vec2::normalized() const
{
    Vec2 result;
    
    float lengthSqr = lengthSq();

    if ( lengthSqr > 0.0f )
    {
        float invLength = 1.0f / sqrtf( lengthSqr );
        
        result.x = x * invLength;
        result.y = y * invLength;
    }

    return result;
}

float Vec2::dotProduct( const Vec2& a, const Vec2& b )
{
	return a.x * b.x + a.y * b.y;
}

