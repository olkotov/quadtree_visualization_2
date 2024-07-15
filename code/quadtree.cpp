// Oleg Kotov

#include "quadtree.h"
#include "point.h"
#include "quadtreenode.h"

QuadTree::QuadTree( const Rectangle& boundary, int capacity /* = 32 */ )
	: m_boundary( boundary )
	, m_capacity( capacity )
{
	m_data = new QuadTreeNode[capacity];
	m_root = createNode( boundary );
}

QuadTree::~QuadTree()
{
	delete[] m_data;
}

bool QuadTree::insert( const Point& point )
{
	if ( m_count + 4 >= m_capacity )
	{
		increaseCapacity();
	}

	return m_root->insert( point );
}

int QuadTree::query( const Rectangle& range, std::vector<Point>& points ) const
{
	return m_root->query( range, points );
}

int QuadTree::query( const Circle& range, std::vector<Point>& points ) const
{
	return m_root->query( range, points );
}

void QuadTree::clear()
{
	for ( int32_t i = 0; i < m_count; ++i )
	{
		QuadTreeNode& node = m_data[i];
		node.m_points.clear();
		node.m_top_left = nullptr;
		node.m_top_right = nullptr;
		node.m_bottom_left = nullptr;
		node.m_bottom_right = nullptr;
	}

	m_count = 0;
	m_root = createNode( m_boundary );
}

QuadTreeNode* QuadTree::createNode( const Rectangle& boundary )
{
	QuadTreeNode* node = &m_data[m_count++];
	node->m_boundary = boundary;
	node->m_tree = this;
	return node;
}

void QuadTree::increaseCapacity()
{
	int newCapacity = m_capacity * 2; 

	QuadTreeNode* newData = new QuadTreeNode[newCapacity];

	for ( int i = 0; i < m_count; ++i )
	{
		newData[i] = m_data[i];
	}

	// update pointers to new memory

	for ( int i = 0; i < m_count; ++i )
	{
		QuadTreeNode& node = newData[i];

		// has children
		if ( node.m_top_left )
		{
			int32_t index = 0;
			index = node.m_top_left - m_data;
			// index = std::distance( m_data, node.m_top_left );
			node.m_top_left = &newData[index];

			index = node.m_top_right - m_data;
			node.m_top_right = &newData[index];

			index = node.m_bottom_left - m_data;
			node.m_bottom_left = &newData[index];

			index = node.m_bottom_right - m_data;
			node.m_bottom_right = &newData[index];
		}
	}

	delete[] m_data;

	m_data = newData;
	m_capacity = newCapacity;

	m_root = &m_data[0];
}

