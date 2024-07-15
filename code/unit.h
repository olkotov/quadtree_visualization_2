// Oleg Kotov

#pragma once

#include "vec2.h"
#include <stdint.h>
#include <vector>

class Unit
{
public:

	Unit( const Vec2& position, const Vec2& direction, float move_offset = 0.0f, bool clockwise_movement = true );

public:

	const Vec2& getPosition() const;
	const Vec2& getDirection() const;

	bool inViewCone( Unit* other ) const;

	inline void Unit::countVisibleUnits( const std::vector<Unit*>& units )
	{
		m_visibleUnitsCount = 0;
		m_visibleUnitsIds.clear();

		for ( const auto& unit : units )
		{
			if ( inViewCone( unit ) )
			{
				m_visibleUnitsCount++;
				m_visibleUnitsIds.push_back( unit->m_id );
			}
		}
	}

	void move( float time );

	void setId( uint32_t id )
	{
		m_id = id;
	}

	uint32_t getId() const
	{
		return m_id;
	}

	uint32_t getVisibleUnitsCount() const
	{
		return m_visibleUnitsCount;
	}

	const std::vector<uint32_t>& getVisibleUnitsIds() const
	{
		return m_visibleUnitsIds;
	}

private:

	uint32_t m_id;

	Vec2 m_origin;
	Vec2 m_position;
	Vec2 m_direction;

	// movement
	float m_move_offset = 0.0f;
	bool m_clockwise_movement = false;

	// view
	uint32_t m_visibleUnitsCount;
	std::vector<uint32_t> m_visibleUnitsIds;
};

