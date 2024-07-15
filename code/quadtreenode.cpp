// Oleg Kotov

#include "quadtreenode.h"
#include "quadtree.h"
#include "point.h"
#include "circle.h"
#include "rectangle.h"

bool QuadTreeNode::insert( const Point& point )
{
	if ( !m_boundary.contains( point ) ) return false;

	if ( m_points.size() < m_capacity )
	{
		m_points.push_back( point );
		return true;
	}
	else
	{
		subdivide();

		if ( m_top_left->insert( point ) ) return true;
		if ( m_top_right->insert( point ) ) return true;
		if ( m_bottom_left->insert( point ) ) return true;
		if ( m_bottom_right->insert( point ) ) return true;

		return false;
	}
}

int QuadTreeNode::query( const Rectangle& range, std::vector<Point>& points ) const
{
	if ( !m_boundary.intersects( range ) ) return 0;

	int checkCount = 0;

	for ( const auto& point : m_points )
	{
		checkCount++;
		if ( range.contains( point ) ) points.push_back( point );
	}

	if ( m_top_left )
	{
		checkCount += m_top_left->query( range, points );
		checkCount += m_top_right->query( range, points );
		checkCount += m_bottom_left->query( range, points );
		checkCount += m_bottom_right->query( range, points );
	}

	return checkCount;
}

int QuadTreeNode::query( const Circle& range, std::vector<Point>& points ) const
{
	if ( !m_boundary.intersects( range ) ) return 0;

	int checkCount = 0;

	for ( const auto& point : m_points )
	{
		checkCount++;
		if ( range.contains( point ) ) points.push_back( point );
	}

	if ( m_top_left )
	{
		checkCount += m_top_left->query( range, points );
		checkCount += m_top_right->query( range, points );
		checkCount += m_bottom_left->query( range, points );
		checkCount += m_bottom_right->query( range, points );
	}

	return checkCount;
}

void QuadTreeNode::subdivide()
{
	// if already divided - return
	if ( m_top_left ) return;

	float x = m_boundary.x;
	float y = m_boundary.y;
	float w = m_boundary.width * 0.5f;
	float h = m_boundary.height * 0.5f;

	m_top_left = m_tree->createNode( Rectangle( x, y, w, h ) );
	m_top_right = m_tree->createNode( Rectangle( x + w, y, w, h ) );
	m_bottom_left = m_tree->createNode( Rectangle( x, y + h, w, h ) );
	m_bottom_right = m_tree->createNode( Rectangle( x + w, y + h, w, h ) );
}

