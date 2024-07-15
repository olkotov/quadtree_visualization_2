// Oleg Kotov

#include "unit.h"
#include "params.h"

Unit::Unit( const Vec2& position, const Vec2& direction, float move_offset /* = 0.0f */, bool clockwise_movement /* = true */ )
{
	m_id = -1;
	m_origin = position;
	m_position = position;
	m_direction = direction;
	m_visibleUnitsCount = 0;
	m_move_offset = move_offset;
	m_clockwise_movement = clockwise_movement;
}

const Vec2& Unit::getPosition() const
{
	return m_position;
}

const Vec2& Unit::getDirection() const
{
	return m_direction;
}

bool Unit::inViewCone( Unit* other ) const
{
	if ( !other || other == this ) return false;

	Vec2 toOtherDir = other->getPosition() - getPosition();

	// check distance
	// actually we not need distance check with quadtree (twice)

	if ( !params.useSpatialPartition )
	{
		if ( params.useSquaredDistance )
		{
			// optimized
			float distSqr = toOtherDir.lengthSq();
			if ( distSqr > params.distanceSqr ) return false;
		}
		else
		{
			// not optimized
			float distance = toOtherDir.length();
			if ( distance > params.distance ) return false;
		}
	}

	// check sector range

	float angleCosine = Vec2::dotProduct( getDirection(), toOtherDir.normalized() );
	if ( angleCosine > params.sectorCosine ) return true;

	return false;
}

void Unit::move( float time )
{
	if ( m_clockwise_movement )
	{
		m_position.x = m_origin.x + cosf( time + m_move_offset ) * 100.0f;
		m_position.y = m_origin.y + sinf( time + m_move_offset ) * 100.0f;
		m_direction.x = cosf( time + m_move_offset + 1.57f );
		m_direction.y = sinf( time + m_move_offset + 1.57f );
	}
	else
	{
		m_position.x = m_origin.x + cosf( -time + m_move_offset ) * 100.0f;
		m_position.y = m_origin.y + sinf( -time + m_move_offset ) * 100.0f;
		m_direction.x = cosf( -time + m_move_offset - 1.57f );
		m_direction.y = sinf( -time + m_move_offset - 1.57f );
	}
}

