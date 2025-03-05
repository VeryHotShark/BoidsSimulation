#include "pch.h"
#include "BoidManager.h"
#include "MathHelper.h"
#include "Game.h"

namespace 
{
    constexpr float STEERING_UPDATE_INTERVAL = 0.0f;
    constexpr float STEERING_UPDATE_INTERVAL_INCREMENT = 0.015f;
    constexpr float STEERING_UPDATE_INTERVAL_DECREMENT = 0.0075f;
    constexpr float BOID_RADIUS = 0.6f;
    constexpr float BOID_HASH_GRID_CELL_SIZE = 6.0f;
    constexpr Vector3 BOUNDS_SIZE = Vector3(45.0f, 35.0f, 45.0f);
    constexpr int BOID_INCREMENT_COUNT = 500;
    constexpr int BOID_DECREMENT_COUNT = 250;

}

Boid::Boid(uint8_t flockID, Vector3 velocity, Vector3 position, Vector3 size)
    : MovingEntity(position, size, velocity)
    , flockID(flockID)
{
}

BoidManager::BoidManager(const Game& game)
    : m_game(game)
    , m_boidsAmount(1000)
    , m_flocksCount(2)
    , m_boidMaxSpeed(10.5f)
    , m_boidMinSpeed(6.5f)
    , m_boidAccelerationMultiplier(50.0f)
    , m_steeringUpdateTimer(0.0f)
    , m_steeringUpdateInterval(STEERING_UPDATE_INTERVAL)
    , m_boidSteeringController(*this, game)
    , m_boidsHashGrid(BOID_HASH_GRID_CELL_SIZE)
    , m_spawnKeyPressedLastFrame(false)
    , m_despawnKeyPressedLastFrame(false)
    , m_increaseKeyPressedLastFrame(false)
    , m_decreaseKeyPressedLastFrame(false)
{
    // Probably Shouldn't have this tight coupling, consider Game class as a mediator or some Event Manager
    Projectile::OnDestroy = [this](Vector3 position, Vector3 velocity)
    {
        SpawnBoidAtPosition(position, velocity, MathHelper::RandomFromRange(0, m_flocksCount - 1));
    };

    m_bounds = Bounds(Vector3::Up * BOUNDS_SIZE.y / 2.0f, BOUNDS_SIZE);

    m_flockColors.reserve(m_flocksCount);
    for (int i = 0; i < m_flocksCount; i++)
    {
        const float intensity = 1.0f - (static_cast<float>(i) / static_cast<float>(m_flocksCount));
        const float remapped_intensity = MathHelper::GetProportional(0.0f, 1.0f, intensity, 0.5f, 1.0f);
        m_flockColors.push_back({ remapped_intensity, remapped_intensity, 0.0f, 1.0f });
    }
}

BoidManager::~BoidManager()
{
    if(Projectile::OnDestroy)
    {
        Projectile::OnDestroy = nullptr;
    }
}

void BoidManager::OnInitialize()
{
    m_boidShape.reset();
    m_simulationBoundsShape.reset();
    m_boidShape = GetEngine().CreateSpherePrimitive(BOID_RADIUS);
    m_simulationBoundsShape = GetEngine().CreateBoxPrimitive(m_bounds.size);

    m_boids.reserve(m_boidsAmount * 2);
    SpawnBoids(m_boidsAmount);
}

void BoidManager::SpawnBoids(int amount)
{
    static constexpr float BOID_MIN_BOUNDS_Y_FACTOR = 0.5f;
    const Vector3 bounds_offset = Vector3(m_bounds.extents.x, 0.0f, m_bounds.extents.z);

    for(int i = 0; i < amount; i++)
    {
        const Vector3 random_position = Vector3(m_bounds.size.x * MathHelper::RandomValue(), m_bounds.size.y * std::max(BOID_MIN_BOUNDS_Y_FACTOR, MathHelper::RandomValue()), m_bounds.size.z * MathHelper::RandomValue()) - bounds_offset;
        SpawnBoidAtPosition(random_position, MathHelper::RandomDirection(), m_flocksCount > 1 ? static_cast<uint8_t>(i % m_flocksCount) : 0);
    }
}

void BoidManager::RemoveBoids(int amount) const
{
    const int amount_to_destroy = std::min<int>(m_boids.size() , amount);
    for(int i = 0; i < amount_to_destroy; i++)
    {
        m_boids[i]->Destroy();
    }
}

void BoidManager::SpawnBoidAtPosition(Vector3 position, Vector3 velocity, uint8_t team_id)
{
    assert(team_id < m_flocksCount);
    m_boids.push_back(std::make_unique<Boid>(team_id, velocity, position, Vector3::One * (BOID_RADIUS * 2.0f)));
    m_boidsHashGrid.AddEntity(m_boids.back().get());
}

void BoidManager::OnUpdate(float deltaTime, DirectX::Keyboard& keyboard)
{
    UpdateBoids(deltaTime);
    RemovePendingBoids();
    OnInput(keyboard);
}

void BoidManager::OnInput(DirectX::Keyboard& keyboard)
{

    auto keyboardState = keyboard.GetState();

    // Should probably create some simple InputManager / InputWrapper to detect click release, tap etc
    if (m_spawnKeyPressedLastFrame && !keyboardState.P)
    {
        SpawnBoids(BOID_INCREMENT_COUNT);
    }
    else if (m_despawnKeyPressedLastFrame && !keyboardState.O)
    {
        RemoveBoids(BOID_DECREMENT_COUNT);
    }
    else if(m_increaseKeyPressedLastFrame && !keyboardState.L)
    {
        m_steeringUpdateInterval += STEERING_UPDATE_INTERVAL_INCREMENT;
    }
    else if (m_decreaseKeyPressedLastFrame && !keyboardState.K)
    {
        m_steeringUpdateInterval -= STEERING_UPDATE_INTERVAL_DECREMENT;
        m_steeringUpdateInterval = std::max(0.0f, m_steeringUpdateInterval);
    }

    m_spawnKeyPressedLastFrame = keyboardState.P;
    m_despawnKeyPressedLastFrame = keyboardState.O;
    m_increaseKeyPressedLastFrame= keyboardState.L;
    m_decreaseKeyPressedLastFrame = keyboardState.K;
}

void BoidManager::UpdateBoids(float deltaTime)
{
    m_steeringUpdateTimer -= deltaTime;
    bool canUpdateSteering = m_steeringUpdateTimer < 0.0f;

    for (std::unique_ptr<Boid>& boidPtr : m_boids)
    {
        Boid& boid = *boidPtr.get();

        if(canUpdateSteering)
        {
            m_boidsHashGrid.UpdateEntity(boidPtr.get());
            boid.SetAcceleration(m_boidSteeringController.GetBoidSteering(boid) * m_boidAccelerationMultiplier);
        }

        const Vector3 newVelocity = boid.GetVelocity() + boid.GetAcceleration() * deltaTime;
        boid.SetVelocity(newVelocity);

        const float currentSpeedSquared = boid.GetVelocity().LengthSquared();

        if (currentSpeedSquared > m_boidMaxSpeed * m_boidMaxSpeed)
        {
            Vector3 direction;
            boid.GetVelocity().Normalize(direction);
            boid.SetVelocity(direction * m_boidMaxSpeed);
        }
        else if (currentSpeedSquared < m_boidMinSpeed * m_boidMinSpeed)
        {
            Vector3 direction;
            boid.GetVelocity().Normalize(direction);
            boid.SetVelocity(direction * m_boidMinSpeed);
        }

        boid.UpdatePositionBasedOnVelocity(deltaTime);
    }

    if(canUpdateSteering)
    {
        m_steeringUpdateTimer = m_steeringUpdateInterval;
    }
}

void BoidManager::RemovePendingBoids()
{
    for (auto iter = m_boids.begin(); iter != m_boids.end(); )
    {
        if (iter->get()->IsPendingDestroy())
        {
            m_boidsHashGrid.RemoveEntity(iter->get());
            iter = m_boids.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void BoidManager::OnRender(const framework::RenderContextPtr& renderContext) const
{
	for (const std::unique_ptr<Boid>& boid : m_boids)
	{
		renderContext->RenderPrimitive(m_boidShape, Vector3::One, boid->GetPosition(), Vector3::Zero, m_flockColors[boid->flockID]);
	}

    static constexpr  XMVECTORF32 boundsColor = { 1.0f, 1.0f, 1.0f, 0.2f };
    renderContext->RenderPrimitive(m_simulationBoundsShape, Vector3::One, m_bounds.center, Vector3::Zero, boundsColor);
    renderContext->RenderText(std::string("boids count: " + std::to_string(m_boids.size())),
                              GetEngine().GetWindowSize() * (Vector2::UnitX * 0.4f), 1.0f);
    renderContext->RenderText(std::string("update interval: " + std::to_string(m_steeringUpdateInterval)), Vector2(GetEngine().GetWindowSize().x * 0.4f, 20.0f), 1.0f);
}

void BoidManager::OnShutdown()
{
    m_boids.clear();
    m_boidShape.reset();
    m_simulationBoundsShape.reset();
}
