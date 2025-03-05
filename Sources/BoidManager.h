#pragma once
#include "BoidSteeringController.h"
#include "Entity.h"
#include "IRenderContext.h"
#include "Octree.h"
#include "SpatialHashGrid.h"

class Game;

class Boid : public MovingEntity
{
public:
    uint8_t flockID;

    Boid(uint8_t flockID, Vector3 velocity, Vector3 position, Vector3 size);
};

class BoidManager
{
public:
    BoidManager(const Game& game);
    ~BoidManager();

    void OnInitialize();
    void OnUpdate(float deltaTime, DirectX::Keyboard& keyboard);
    void OnInput(DirectX::Keyboard& keyboard);
    void OnRender(const framework::RenderContextPtr& renderContext) const;
    void OnShutdown();

    const Bounds& GetBounds() const { return m_bounds; }
    const SpatialHashGrid<Boid>& GetBoidsHashGrid() const { return m_boidsHashGrid; }

private:

    void SpawnBoids(int amount);
    void RemoveBoids(int amount) const;

    void SpawnBoidAtPosition(Vector3 position, Vector3 velocity, uint8_t team_id = 0);
    void UpdateBoids(float deltaTime);
    void RemovePendingBoids();

    const Game& m_game;

    bool m_spawnKeyPressedLastFrame;
    bool m_despawnKeyPressedLastFrame;

    bool m_increaseKeyPressedLastFrame;
    bool m_decreaseKeyPressedLastFrame;

    int m_boidsAmount;
    int m_flocksCount;

    float m_boidMaxSpeed;
    float m_boidMinSpeed;
    float m_boidAccelerationMultiplier;

    float m_steeringUpdateTimer;
    float m_steeringUpdateInterval;

    Bounds m_bounds;
    SpatialHashGrid<Boid> m_boidsHashGrid;
    BoidSteeringController m_boidSteeringController;

    std::vector<XMVECTOR> m_flockColors;
    std::vector<std::unique_ptr<Boid>> m_boids;
    std::unique_ptr< DirectX::GeometricPrimitive > m_boidShape;
    std::unique_ptr< DirectX::GeometricPrimitive > m_simulationBoundsShape;
};

