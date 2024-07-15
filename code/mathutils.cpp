// Oleg Kotov

#include "mathutils.h"
#include "vec2.h"

#define _USE_MATH_DEFINES
#include <math.h> // M_PI

#include <stdlib.h> // rand()

const float Deg2Rad = M_PI / 180.0f;

int getRandomNumberInRange( int min, int max )
{
    return min + rand() % ( max - min + 1 );
}

float getRandomNumberInRange( float min, float max )
{
    return rand() / (float)RAND_MAX * ( max - min ) + min;
}

Vec2 getRandomUnitVector()
{
    float angle = getRandomNumberInRange( (float)-M_PI, (float)M_PI );
	return Vec2( cosf( angle ), sinf( angle ) ); // direction
}

bool getRandomBool()
{
	return rand() % 2 == 0;
}

