// Oleg Kotov

#pragma once

#include "rectangle.h"
#include <vector>

class Point;
class Circle;
class QuadTree;

class QuadTreeNode
{
public:

	QuadTreeNode() = default;
	~QuadTreeNode() = default;

	bool insert( const Point& point );

	int query( const Rectangle& range, std::vector<Point>& points ) const;
	int query( const Circle& range, std::vector<Point>& points ) const;

	const Rectangle& getBoundary() const
	{
		return m_boundary;
	}

private:

	void subdivide();

private:

	Rectangle m_boundary;
	QuadTree* m_tree = nullptr;

	int m_capacity = 4;
	std::vector<Point> m_points;

	friend class QuadTree;

	QuadTreeNode* m_top_left = nullptr;
	QuadTreeNode* m_top_right = nullptr;
	QuadTreeNode* m_bottom_left = nullptr;
	QuadTreeNode* m_bottom_right = nullptr;
};

