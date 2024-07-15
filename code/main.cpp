// Oleg Kotov

#include "unit.h"
#include "mathutils.h"
#include "rectangle.h"
#include "quadtree.h"
#include "quadtreenode.h"
#include "params.h"

#include <SFML/Graphics.hpp>

#include <vector>
#include <thread>
#include <chrono> // performance
#include <stdio.h>
#include <time.h> // random seed
#include <mutex>

const int SPAWN_COUNT = 90;

bool drawQuadTreeQuads = false;
bool paused = false;
int32_t checkCount = 0;

uint32_t numUnits = 2000; // 10,000
uint16_t winSize = 800;

std::vector<Unit*> units;
std::vector<Point> points;

Rectangle boundary( 0, 0, winSize, winSize );
QuadTree quadtree( boundary, 4096 );

// multithreading
uint8_t numThreads;
int32_t blockSize;
std::vector<std::thread> threads;
std::mutex checkCountMutex;

// this not used quadtree
void countVisibleWorkerRegular( uint8_t threadId, int32_t startIndex, int32_t endIndex )
{
	int32_t diff = endIndex - startIndex;

	// printf( "[%i] worker started, unitCount: %i\n", threadId, diff );

	for ( int32_t i = startIndex; i < endIndex; ++i )
	{
		// not good for performance, just for debug
		{
			std::lock_guard<std::mutex> lock( checkCountMutex );
			checkCount += units.size();
		}

		units[i]->countVisibleUnits( units );
	}
}

// this used quadtree
void countVisibleWorkerSpatial( uint8_t threadId, int32_t startIndex, int32_t endIndex )
{
	int32_t diff = endIndex - startIndex;

	// printf( "[%i] worker started, unitCount: %i\n", threadId, diff );

	std::vector<Point> nearestPoints;
	std::vector<Unit*> nearestUnits;

	nearestPoints.reserve( 100 );
	nearestUnits.reserve( 100 );

	Circle circle = {};
	circle.radius = params.distance;

	// try to be more fast with iterators ¯\_(-.-)_/¯
	// for ( int32_t i = startIndex; i < endIndex; ++i )
	for ( auto it = units.begin() + startIndex; it != units.begin() + endIndex; ++it )
	{
		// Unit* unit = units[i];
		Unit* unit = *it;
		const Vec2& position = unit->getPosition();

		circle.x = position.x;
		circle.y = position.y;

		nearestPoints.clear();
		quadtree.query( circle, nearestPoints );

		nearestUnits.clear();

		std::size_t nearestPointsCount = nearestPoints.size();

		for ( int32_t j = 0; j < nearestPointsCount; ++j )
		{
			Unit* nearestUnit = (Unit*)nearestPoints[j].getUserPointer();
			nearestUnits.push_back( nearestUnit );
		}

		// not good for performance, just for debug
		{
			std::lock_guard<std::mutex> lock( checkCountMutex );
			checkCount += nearestUnits.size();
		}

		unit->countVisibleUnits( nearestUnits );
	}
}

void spawnUnit( int32_t unitId )
{
	// position

	int x = getRandomNumberInRange( 0, winSize );
	int y = getRandomNumberInRange( 0, winSize );

	Vec2 position( x, y );
		
	// direction
	Vec2 direction = getRandomUnitVector();

	// move offset
	float move_offset = getRandomNumberInRange( 0.0f, 10.0f );

	// clockwise movement
	bool clockwise_movement = getRandomBool();

	// create unit, not good, but okay (we can use pool)
	Unit* unit = new Unit( position, direction, move_offset, clockwise_movement );
	unit->setId( unitId );

	units.push_back( unit );

	// create point
	Point point( x, y );
	point.setUserPointer( unit );

	points.push_back( point );
}

void updateThreadSettings()
{
	numThreads = std::max( (uint32_t)1, std::thread::hardware_concurrency() );

	if ( !params.useMultithreading )
	{
		numThreads = 1; // disable multithreading
	}

	blockSize = numUnits / numThreads;

	// wait not wait
	/*for ( auto& thread : threads )
	{
		if ( thread.joinable() )
		{
			thread.join();
		}
	}*/

	threads.clear();
	threads.resize( numThreads );
}

void updateUnitVisibility()
{
	// start
	for ( uint8_t i = 0; i < numThreads; ++i )
	{
		int32_t start = i * blockSize;
		int32_t end = ( i == numThreads - 1 ) ? numUnits : start + blockSize;

		threads[i] = std::thread( ( params.useSpatialPartition ) ? countVisibleWorkerSpatial : countVisibleWorkerRegular, i, start, end );
	}

	// wait
	for ( auto& entry : threads )
	{
		entry.join();
	}
}

Unit* selectRandomUnit()
{
	if ( units.size() > 0 )
	{
		uint32_t selectedUnitIndex = getRandomNumberInRange( 0, units.size() - 1 );
		return units[selectedUnitIndex];
	}

	return nullptr;
}

void drawQuads( QuadTree& quadtree, sf::RenderWindow& window )
{
	sf::RectangleShape rectShape;
	rectShape.setFillColor( sf::Color::Transparent );
	rectShape.setOutlineColor( sf::Color( 139, 83, 106 ) );
	rectShape.setOutlineThickness( 0.5f );

	for ( int32_t i = 0; i < quadtree.count(); ++i )
	{
		QuadTreeNode* node = &quadtree.data()[i];
		auto boundary = node->getBoundary();

		rectShape.setPosition( boundary.x, boundary.y );
		rectShape.setSize( sf::Vector2f( boundary.width, boundary.height ) );
		window.draw( rectShape );
	}
}

int main()
{
	sf::RenderWindow window( sf::VideoMode( winSize, winSize ), "Units in View" );

	sf::Clock clock;

	float timeSinceStartup = 0.0f;

	sf::Font font;
	if ( !font.loadFromFile( "inter-regular.otf" ) ) return -1;

	sf::Text text( "", font, 16 );
	text.setPosition( 10.0f, 12.0f );
	text.setFillColor( sf::Color::White );

	sf::CircleShape pointShape( 3.0f );
	pointShape.setFillColor( sf::Color( 0, 255, 127, 50 ) );
	pointShape.setOrigin( pointShape.getRadius(), pointShape.getRadius() );

	sf::CircleShape highlightedPointShape( 3.5f );
	highlightedPointShape.setFillColor( sf::Color( 0, 255, 127, 150 ) ); // sf::Color( 0, 255, 127, 150 )
	highlightedPointShape.setOrigin( highlightedPointShape.getRadius(), highlightedPointShape.getRadius() );

	sf::CircleShape selectCircle;
	selectCircle.setFillColor( sf::Color( 0, 120, 215, 50 ) );
	selectCircle.setOutlineColor( sf::Color( 0, 120, 215 ) );
	selectCircle.setOutlineThickness( 1.0f );
	selectCircle.setRadius( params.distance );
	selectCircle.setOrigin( selectCircle.getRadius(), selectCircle.getRadius() );

	// srand( time( NULL ) ); // set random seed

	printf( "numUnits: %i \n", numUnits );

	updateThreadSettings();

	printf( "numThreads: %i \n", numThreads );
	printf( "blockSize: %i \n", blockSize );

	// create units
	units.reserve( numUnits );

	for ( int32_t i = 0; i < numUnits; ++i )
	{
		spawnUnit( units.size() );
	}

	// fill quadtree and print time
	{
		auto start = std::chrono::high_resolution_clock::now();

		// fill quadtree for single test
		for ( const auto& point : points )
		{
			quadtree.insert( point );
		}

		auto end = std::chrono::high_resolution_clock::now();

		uint64_t quadtreeFillTime = std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count();
		printf( "quadtreeFillTime: %lli microsecond, ~%.3f seconds\n", quadtreeFillTime, quadtreeFillTime / 1000000.0f );
	}

	// update units view info and print time
	{
		auto start = std::chrono::high_resolution_clock::now();
		updateUnitVisibility();
		auto end = std::chrono::high_resolution_clock::now();

		uint64_t unitsViewInfoUpdateTime = std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count();
		printf( "unitsViewInfoUpdateTime: %lli microsecond, ~%.3f seconds\n", unitsViewInfoUpdateTime, unitsViewInfoUpdateTime / 1000000.0f );
	}

	Unit* selectedUnit = selectRandomUnit();

	// game loop

	while ( window.isOpen() )
    {
		sf::Event event;
        
		while ( window.pollEvent( event ) )
        {
            if ( event.type == sf::Event::Closed ) window.close();

			if ( event.type == sf::Event::KeyPressed )
			{
				if ( event.key.code == sf::Keyboard::F1 )
				{
					selectedUnit = selectRandomUnit();
				}
				else if ( event.key.code == sf::Keyboard::F2 )
				{
					paused = !paused;
				}
				else if ( event.key.code == sf::Keyboard::F3 )
				{
					drawQuadTreeQuads = !drawQuadTreeQuads;
				}
				else if ( event.key.code == sf::Keyboard::F4 )
				{
					params.useSquaredDistance = !params.useSquaredDistance;
				}
				else if ( event.key.code == sf::Keyboard::F5 )
				{
					params.useMultithreading = !params.useMultithreading;
					updateThreadSettings();
				}
				else if ( event.key.code == sf::Keyboard::F6 )
				{
					params.useSpatialPartition = !params.useSpatialPartition;

					if ( !params.useSpatialPartition )
					{
						quadtree.clear();
					}
				}
				else if ( event.key.code == sf::Keyboard::F8 )
				{
					units.clear();
					points.clear();
					quadtree.clear();

					numUnits = 0;
					selectedUnit = nullptr;

					updateThreadSettings();
				}
			}
        }
		
		if ( sf::Keyboard::isKeyPressed( sf::Keyboard::F7 ) )
		{
			for ( int i = 0; i < SPAWN_COUNT; ++i )
			{
				spawnUnit( units.size() );
			}

			numUnits = units.size();

			updateThreadSettings();

			quadtree.clear();

			for ( auto& point : points )
			{
				quadtree.insert( point );
			}

			updateUnitVisibility();

			if ( !selectedUnit )
			{
				selectedUnit = selectRandomUnit();
			}
		}

		// ===========================================

		float deltaTime = clock.restart().asSeconds();

		if ( !paused && units.size() > 0 )
		{
			timeSinceStartup += deltaTime;
			checkCount = 0;

			for ( const auto& unit : units )
			{
				unit->move( timeSinceStartup );
			}

			if ( params.useSpatialPartition )
			{
				quadtree.clear();

				// we can store a point in an entity
				for ( auto& point : points )
				{
					Unit* unit = (Unit*)point.getUserPointer();
					const Vec2& pos = unit->getPosition();

					// sync unit and point position
					point.x = pos.x;
					point.y = pos.y;

					quadtree.insert( point );
				}
			}

			updateUnitVisibility();
		}

		// ===========================================

		window.clear( sf::Color( 30, 30, 30 ) );

		// draw boundaries of quadtree nodes
		if ( drawQuadTreeQuads )
		{
			drawQuads( quadtree, window );
		}

		// draw all unit points
		for ( const auto& unit : units )
		{
			const Vec2& pos = unit->getPosition();

			pointShape.setPosition( pos.x, pos.y );
			window.draw( pointShape );
		}

		// draw unit range and view cone
		if ( selectedUnit )
		{
			// draw circle shape
			const Vec2& pos = selectedUnit->getPosition();
			
			selectCircle.setPosition( pos.x, pos.y );
			window.draw( selectCircle );

			// draw cone view shape
			const Vec2& dir = selectedUnit->getDirection();

			float angleDeg = atan2f( dir.y, dir.x );
			angleDeg *= 180.0f / 3.14159f;

			sf::VertexArray coneShape( sf::PrimitiveType::TriangleFan );
			float centerX = pos.x;
			float centerY = pos.y;
			float radius = params.distance;
			float startAngle = -( 135.5 * 0.5f ) + angleDeg; // deg
			float endAngle = ( 135.5 * 0.5f ) + angleDeg; // deg
			float step = 2.0f * 3.14159f / 360.0f; // step in rad

			coneShape.append( sf::Vertex( sf::Vector2f( centerX, centerY ), sf::Color( 255, 99, 71, 170 ) ) );

			for ( float angle = startAngle; angle <= endAngle; angle += step )
			{
				float x = centerX + radius * std::cos( angle * 3.14159f / 180.0f );
				float y = centerY + radius * std::sin( angle * 3.14159f / 180.0f );
				coneShape.append( sf::Vertex( sf::Vector2f( x, y ), sf::Color( 255, 99, 71, 170 ) ) );
			}

			window.draw( coneShape );
		}

		// draw units in cone view
		if ( selectedUnit )
		{
			auto unitsInView = selectedUnit->getVisibleUnitsIds();

			for ( const auto& unitIndex : unitsInView )
			{
				const Vec2& pos = units[unitIndex]->getPosition();
			
				highlightedPointShape.setPosition( pos.x, pos.y );
				window.draw( highlightedPointShape );
			}
		}

		char textString[255];
		sprintf( textString, "%.2f fps / %.2f ms\n\ntimeSinceStartup: %.2f\nnumUnits: %d\ncheckCount: %d\nunitId: %d\nviewCount: %d\n\nuseSquaredDistance: %s\nuseMultithreading: %s\nuseSpatialPartition: %s",
			1.0f / deltaTime,
			deltaTime * 1000,
			timeSinceStartup,
			numUnits,
			checkCount,
			( selectedUnit ) ? selectedUnit->getId() : -1,
			( selectedUnit ) ? selectedUnit->getVisibleUnitsCount() : 0,
			params.useSquaredDistance ? "true" : "false",
			params.useMultithreading ? "true" : "false",
			params.useSpatialPartition ? "true" : "false"
		);

		text.setString( textString );
		window.draw( text );

		window.display();
	}

	return 0;
}

