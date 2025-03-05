/**
* Copyright (c) 2021 CD Projekt Red. All Rights Reserved.
*/

#pragma once
#include "Bounds.h"
#include "Entity.h"
#include "IRenderContext.h"
#include "Octree.h"
#include "SpatialHashGrid.h"

class City
{

public:
	City();
	~City();

	void OnInitialize();
	void OnUpdate( float deltaTime );
	void OnRender( framework::RenderContextPtr& renderContext );
	void OnShutdown();

	float GetHighestSkyscraperYPos() const { return m_highestSkyscraperYPos; }
	const std::vector< Entity >& GetSkyscrapers() const { return m_skyscrapers; }
	//const Octree<Skyscraper>& GetSkyscrapersOctree() const { return m_skyscrapersOctree; }

private:
    void Load();

	float m_highestSkyscraperYPos = 0.0f;

	//Octree<Skyscraper> m_skyscrapersOctree;
	std::vector< Entity > m_skyscrapers;
	std::unique_ptr< DirectX::GeometricPrimitive > m_shape;
};

