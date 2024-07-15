// Oleg Kotov

#pragma once

// point class for quadtree

class Point
{
public:

	Point() = default;

	Point( float x, float y )
	{
		this->x = x;
		this->y = y;
	}

	void setUserPointer( void* userPointer )
	{
		m_userPointer = userPointer;
	}

	void* getUserPointer() const
	{
		return m_userPointer;
	}

public:

	float x = 0.0f;
	float y = 0.0f;

private:

	void* m_userPointer = nullptr;
};

