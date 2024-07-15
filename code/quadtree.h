// Oleg Kotov

#pragma once

#include "rectangle.h"
#include <vector>

class Point;
class QuadTreeNode;

class QuadTree
{
public:

	QuadTree( const Rectangle& boundary, int capacity = 32 );
	~QuadTree();

	bool insert( const Point& point );

	int query( const Rectangle& range, std::vector<Point>& points ) const;
	int query( const Circle& range, std::vector<Point>& points ) const;

	const Rectangle& getBoundary() const
	{
		return m_boundary;
	}

	int32_t capacity() const
	{
		return m_capacity;
	}

	int32_t count() const
	{
		return m_count;
	}

	QuadTreeNode* data() const
	{
		return m_data;
	}

	void clear();

	QuadTreeNode* createNode( const Rectangle& boundary );

private:

	void increaseCapacity();

private:

	Rectangle m_boundary;

	int32_t m_capacity;  // in nodes, not points
	int32_t m_count = 0; // next_node_index

	QuadTreeNode* m_data = nullptr;
	QuadTreeNode* m_root = nullptr;
};

