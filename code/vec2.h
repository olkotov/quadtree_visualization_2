// Oleg Kotov

#pragma once

class Vec2
{
public:

	Vec2();
	Vec2( float x, float y );

	Vec2 operator-( const Vec2& v ) const;

public:

	float length() const;
	float lengthSq() const;

	Vec2 normalized() const;

	static float dotProduct( const Vec2& a, const Vec2& b );

public:

	float x;
	float y;
};

