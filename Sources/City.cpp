/**
* Copyright (c) 2021 CD Projekt Red. All Rights Reserved.
*/

#include "pch.h"
#include "City.h"
#include "Engine.h"
#include "StepTimer.h"
#include "DeviceResources.h"
#include "Game.h"

City::City() = default;
City::~City() = default;

void City::OnInitialize()
{
	/// Bounds class should have function to Encapsule other Bounds so we can have proper Octree bounds calculated
	//m_skyscrapersOctree = Octree<Skyscraper>(Bounds(Vector3::Up * 15.0f, Vector3::One * 45.0f), 1, 1);
    Load();
}

void City::Load()
{
	OnShutdown();

	std::ifstream stream( "../../data/city/city.json" );
	std::string fileData( ( std::istreambuf_iterator< char >( stream ) ), std::istreambuf_iterator< char >() );

	Json::Document document;
	document.Parse( fileData.c_str() );
	assert( !document.HasParseError() );

	assert( document.IsObject() );

	// skyscrapers
	assert( document.HasMember( "skyscrapers" ) );
	const Json::Value& arrayObject = document[ "skyscrapers" ];
	assert( arrayObject.IsArray() );

	m_skyscrapers.reserve(arrayObject.Size());
	m_highestSkyscraperYPos = std::numeric_limits<float>::min();
	for ( Json::SizeType i = 0; i < arrayObject.Size(); i++ )
	{
		Entity newSkyscraper;

		Vector3 position;
        Vector3 dimensions;

        // pos_x
        assert(arrayObject[ i ].HasMember( "pos_x" ));
        assert(arrayObject[ i ][ "pos_x" ].IsFloat());
        position.x = arrayObject[ i ][ "pos_x" ].GetFloat();

        // pos_z
        assert(arrayObject[ i ].HasMember( "pos_z" ));
        assert(arrayObject[ i ][ "pos_z" ].IsFloat());
        position.z = arrayObject[ i ][ "pos_z" ].GetFloat();

        // width
		assert( arrayObject[ i ].HasMember( "width" ) );
		assert( arrayObject[ i ][ "width" ].IsFloat() );
		dimensions.x = arrayObject[ i ][ "width" ].GetFloat();

		// length
		assert( arrayObject[ i ].HasMember( "length" ) );
		assert( arrayObject[ i ][ "length" ].IsFloat() );
		dimensions.z = arrayObject[ i ][ "length" ].GetFloat();

		// height
		assert( arrayObject[ i ].HasMember( "height" ) );
		assert( arrayObject[ i ][ "height" ].IsFloat() );
		dimensions.y = arrayObject[ i ][ "height" ].GetFloat();

		position.y = arrayObject[i].HasMember("pos_y") ? arrayObject[i]["pos_y"].GetFloat() : dimensions.y * 0.5f;

		newSkyscraper.SetPosition(position);
		newSkyscraper.SetBounds(newSkyscraper.GetPosition(), dimensions);

		const float maxYPos = newSkyscraper.GetBounds().max.y;
		if(maxYPos > m_highestSkyscraperYPos)
		{
			m_highestSkyscraperYPos = maxYPos;
		}

		m_skyscrapers.push_back( std::move( newSkyscraper ) );
		//m_skyscrapersOctree.AddEntity(&m_skyscrapers.back());
	}

	m_shape = GetEngine().CreateBoxPrimitive(Vector3::One);
}

void City::OnUpdate( float deltaTime )
{
	UNREFERENCED_PARAMETER( deltaTime );
}

void City::OnRender( framework::RenderContextPtr& renderContext )
{
	for ( Entity& skyscraper : m_skyscrapers )
	{
		renderContext->RenderPrimitive(m_shape, skyscraper.GetBounds().size, skyscraper.GetPosition(), Vector3::Zero, Colors::BlueViolet );
	}

	//m_skyscrapersOctree.OnRender(renderContext);
}

void City::OnShutdown()
{
	m_shape.reset();
	m_skyscrapers.clear();
}
