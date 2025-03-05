#include "pch.h"
#include "Projectile.h"

#include "BoidManager.h"
#include "City.h"

Projectile::OnDestroyDelegate Projectile::OnDestroy = nullptr;

namespace
{
    constexpr float PREDATOR_PURSUE_RADIUS = 10.0f;
    constexpr float PREDATOR_ACCELERATION_MULTIPLIER = 30.0f;
    constexpr int PREDATOR_MAX_CONSUMPTION_COUNT = 10;
}

Projectile::Projectile(Vector3 velocity, Vector3 position, Vector3 size, float drag, float energy, bool predator)
    : MovingEntity(position, size, velocity)
    , m_drag(drag)
    , m_energy(energy)
    , m_isPredator(predator)
    , m_consumedBoidsCount(0)
{
}

bool Projectile::CanConsume() const
{
    return m_isPredator && !ConsumedMax();
}

bool Projectile::ConsumedMax() const
{
    return m_consumedBoidsCount >= PREDATOR_MAX_CONSUMPTION_COUNT;
}

void Projectile::UpdateMovement(float deltaTime)
{
    m_velocity *= std::max(0.0f, 1.0f - m_drag * deltaTime);

    if (m_energy > 0.0f)
    {
        m_velocity += m_acceleration * deltaTime;
        m_energy -= deltaTime;
    }
    else
    {
        if (Projectile::OnDestroy)
        {
            Projectile::OnDestroy(position, GetSteeringDirection());
        }

        Destroy();
        return;
    }

    UpdatePositionBasedOnVelocity(deltaTime);
}

void Projectile::CheckForBoids(const SpatialHashGrid<Boid>& boidsHashGrid)
{
    if (!CanConsume())
    {
        //energy = 0.0f; don't empty energy so they can act as avoidance points
        m_acceleration = Vector3::Zero;
        return;
    }

    const std::vector<Boid*> boids = boidsHashGrid.QueryInRadius(position, PREDATOR_PURSUE_RADIUS);

    Vector3 bestDirection = m_acceleration; // if no valid boid will be found just use previous acceleration
    float closestDistanceSquared = std::numeric_limits<float>::max();

    for (Boid* boid : boids)
    {
        if (CanConsume() && bounds.RadiusIntersects(boid->GetBounds()))
        {
            boid->Destroy();
            ++m_consumedBoidsCount;
            m_energy += 1.0f;
            continue;
        }

        const Vector3 vectorToBoid = boid->GetPosition() - position;
        const float distanceSquared = vectorToBoid.LengthSquared();

        if (distanceSquared < closestDistanceSquared)
        {
            closestDistanceSquared = distanceSquared;
            bestDirection = vectorToBoid;
        }
    }

    m_acceleration = MathHelper::GetNormalized(bestDirection) * PREDATOR_ACCELERATION_MULTIPLIER;
}

void Projectile::CheckForSkyscrapers(const std::vector<Entity>& skyscrapers)
{
    for (const Entity& skyscraper : skyscrapers)
    {
        if (!skyscraper.GetBounds().IntersectsSphere(bounds))
        {
            continue;
        }
        ;
        const Vector3 normal = skyscraper.GetBounds().ClosestSurfaceNormal(position);
        const Vector3 closestPoint = skyscraper.GetBounds().ClosestPoint(position);
        const float penetrationDepth = bounds.biggestExtent - (position - closestPoint).Length();
        if (penetrationDepth > 0.0f)
        {
            Teleport(position + normal * penetrationDepth);
        }

        m_velocity = Vector3::Reflect(m_velocity, normal);
        return;
    }
}