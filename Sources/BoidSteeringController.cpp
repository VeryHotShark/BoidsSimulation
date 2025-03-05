#include "pch.h"
#include "BoidSteeringController.h"

#include "BoidManager.h"
#include "Game.h"
#include "MathHelper.h"

namespace
{
    constexpr float NEIGHBORS_DETECTION_RADIUS = 3.0f;
    constexpr float NEIGHBORS_DETECTION_RADIUS_SQUARED = NEIGHBORS_DETECTION_RADIUS * NEIGHBORS_DETECTION_RADIUS;
    constexpr float NEIGHBORS_DETECTION_HALF_ANGLE = 130.0f;

    constexpr float SKYSCRAPER_AVOIDANCE_DISTANCE = 2.5f;
    constexpr float SKYSCRAPER_AVOIDANCE_DISTANCE_SQUARED = SKYSCRAPER_AVOIDANCE_DISTANCE * SKYSCRAPER_AVOIDANCE_DISTANCE;
    constexpr float PROJECTILE_DETECTION_RADIUS_SQUARED = 100.0f;
    constexpr float CAMERA_DETECTION_RADIUS_SQUARED = 25.0f;

    constexpr float BOUNDS_AVOIDANCE_DISTANCE = 5.0f;
}

BoidSteeringController::BoidSteeringController(const BoidManager& boidManager, const Game& game)
    : m_boidManager(boidManager)
    , m_game(game)
    , m_boundsMultiplier(3.0f)
    , m_cameraMultiplier(2.0f)
    , m_projectileMultiplier(2.2f)
    , m_skyscrapersMultiplier(4.0f)
    , m_cohesionMultiplier(1.75f)
    , m_alignmentMultiplier(1.f)
    , m_separationMultiplier(1.2f)
{
    m_neighborsDetectionDotThreshold = std::cos(MathHelper::DegreesToRadians(NEIGHBORS_DETECTION_HALF_ANGLE));
}

Vector3 BoidSteeringController::GetBoidSteering(const Boid& boid) const
{
    Vector3 finalSteering = Vector3::Zero;
    const Vector3 boundsSteering = GetBoundsSteering(boid);
    const Vector3 cameraSteering = GetCameraSteering(boid);
    const Vector3 projectileSteering = GetProjectileSteering(boid);
    const Vector3 skyscrapersSteering = GetSkyscrapersSteering(boid);
    finalSteering += boundsSteering + cameraSteering + projectileSteering + skyscrapersSteering;

    const std::vector<Boid*> neighbors = GetBoidNeighbors(boid);

    if (neighbors.empty())
    {
        return finalSteering;
    }

    const Vector3 cohesionSteering = GetCohesionSteering(boid, neighbors);
    const Vector3 alignmentSteering = GetAlignmentSteering(boid, neighbors);
    const Vector3 separationSteering = GetSeparationSteering(boid, neighbors);
    finalSteering += cohesionSteering + alignmentSteering + separationSteering;
    return MathHelper::GetNormalized(finalSteering);
}

Vector3 BoidSteeringController::GetBoundsSteering(const Boid& boid) const
{
    static const Vector3 BOUNDS_MAX_WITH_THRESHOLD = m_boidManager.GetBounds().max - Vector3::One * BOUNDS_AVOIDANCE_DISTANCE;
    static const Vector3 BOUNDS_MIN_WITH_THRESHOLD = m_boidManager.GetBounds().min + Vector3::One * BOUNDS_AVOIDANCE_DISTANCE;

    const Vector3 position = boid.GetPosition();
    const float xNegativePushFactor = MathHelper::GetProportional(BOUNDS_MAX_WITH_THRESHOLD.x, m_boidManager.GetBounds().max.x, position.x, 0.0f, 1.0f);
    const float xPositivePushFactor = MathHelper::GetProportional(BOUNDS_MIN_WITH_THRESHOLD.x, m_boidManager.GetBounds().min.x, position.x, 0.0f, 1.0f);
    const float yNegativePushFactor = MathHelper::GetProportional(BOUNDS_MAX_WITH_THRESHOLD.y, m_boidManager.GetBounds().max.y, position.y, 0.0f, 1.0f);
    const float yPositivePushFactor = MathHelper::GetProportional(BOUNDS_MIN_WITH_THRESHOLD.y, m_boidManager.GetBounds().min.y, position.y, 0.0f, 1.0f);
    const float zNegativePushFactor = MathHelper::GetProportional(BOUNDS_MAX_WITH_THRESHOLD.z, m_boidManager.GetBounds().max.z, position.z, 0.0f, 1.0f);
    const float zPositivePushFactor = MathHelper::GetProportional(BOUNDS_MIN_WITH_THRESHOLD.z, m_boidManager.GetBounds().min.z, position.z, 0.0f, 1.0f);

    Vector3 steering = Vector3::Zero;

    if (xNegativePushFactor > 0.0f)
    {
        steering.x = -xNegativePushFactor;
    }
    else if (xPositivePushFactor > 0.0f)
    {
        steering.x = xPositivePushFactor;
    }

    if (yNegativePushFactor > 0.0f)
    {
        steering.y = -yNegativePushFactor;
    }
    else if (yPositivePushFactor > 0.0f)
    {
        steering.y = yPositivePushFactor;
    }

    if (zNegativePushFactor > 0.0f)
    {
        steering.z = -zNegativePushFactor;
    }
    else if (zPositivePushFactor > 0.0f)
    {
        steering.z = zPositivePushFactor;
    }

    return steering * m_boundsMultiplier;
}

Vector3 BoidSteeringController::GetCameraSteering(const Boid& boid) const
{
    const Vector3 vectorFromCamera = boid.GetPosition() - m_game.GetCamera().GetCameraPos();
    const float distanceSquaredToCamera = vectorFromCamera.LengthSquared();

    if (distanceSquaredToCamera > CAMERA_DETECTION_RADIUS_SQUARED)
    {
        return Vector3::Zero;
    }

    const float push_ratio = 1.0f - (distanceSquaredToCamera / CAMERA_DETECTION_RADIUS_SQUARED);
    const Vector3 steering = MathHelper::GetNormalized(vectorFromCamera) * push_ratio;
    return steering * m_cameraMultiplier;
}

Vector3 BoidSteeringController::GetProjectileSteering(const Boid& boid) const
{
    const std::vector<Projectile>& projectiles = m_game.GetProjectileController().GetProjectiles();
    if (projectiles.empty())
    {
        return Vector3::Zero;
    }

    Vector3 steering = Vector3::Zero;

    for (const Projectile& projectile : projectiles)
    {
        const Vector3 vectorFromProjectile = boid.GetPosition() - projectile.GetPosition();
        const float distanceSquaredToProjectile = vectorFromProjectile.LengthSquared();

        if (distanceSquaredToProjectile > PROJECTILE_DETECTION_RADIUS_SQUARED)
        {
            continue;
        }

        const float push_ratio = 1.0f - (distanceSquaredToProjectile / PROJECTILE_DETECTION_RADIUS_SQUARED);
        steering += MathHelper::GetNormalized(projectile.IsPredator() ? vectorFromProjectile : -vectorFromProjectile) * push_ratio;
    }

    return steering * m_projectileMultiplier;
}

Vector3 BoidSteeringController::GetSkyscrapersSteering(const Boid& boid) const
{
    if (boid.GetPosition().y > m_game.GetCity().GetHighestSkyscraperYPos() + SKYSCRAPER_AVOIDANCE_DISTANCE)
    {
        return Vector3::Zero;
    }

    /// Didn't notice big improvements using Octrees, need to investigate the implementation
    //const auto skyscrapers = m_game.GetCity().GetSkyscrapersOctree().QueryEntitiesInBounds(boid.bounds);

    Vector3 steering = Vector3::Zero;

    for (const Entity& skyscraper : m_game.GetCity().GetSkyscrapers())
    {
        const Vector3 closestPoint = skyscraper.GetBounds().ClosestPoint(boid.GetPosition());
        const Vector3 vectorToBoid = boid.GetPosition() - closestPoint;
        const float distanceSquared = vectorToBoid.LengthSquared() - boid.GetBounds().GetBiggestExtentSquared();

        if (distanceSquared < SKYSCRAPER_AVOIDANCE_DISTANCE_SQUARED)
        {
            const Vector3 avoidanceForce = MathHelper::GetNormalized(vectorToBoid) * (1.0f - (distanceSquared / SKYSCRAPER_AVOIDANCE_DISTANCE_SQUARED));
            steering += avoidanceForce;
        }
    }

    //steering += boid.GetSteeringDirection(); // + = smoother ; - = snappier ; none = parallel
    return steering * m_skyscrapersMultiplier;
}

std::vector<Boid*> BoidSteeringController::GetBoidNeighbors(const Boid& boid) const
{
    std::vector<Boid*> neighbors = m_boidManager.GetBoidsHashGrid().QueryInRadius(boid.GetPosition(), NEIGHBORS_DETECTION_RADIUS);

    neighbors.erase(
        std::remove_if(neighbors.begin(), neighbors.end(),
            [&](const Boid* neighbor)
            {
                if (neighbor == &boid)
                {
                    return true;
                }

                const Vector3 directionToNeighbor = MathHelper::GetNormalized(neighbor->GetPosition() - boid.GetPosition());
                return boid.GetSteeringDirection().Dot(directionToNeighbor) < m_neighborsDetectionDotThreshold;
            }),
        neighbors.end());

    return neighbors;
}

Vector3 BoidSteeringController::GetCohesionSteering(const Boid& boid, const std::vector<Boid*>& neighbors) const
{
    Vector3 averagePosition = Vector3::Zero;
    int validNeighbors = 0;

    for (const Boid* neighbor : neighbors)
    {
        if (neighbor->flockID == boid.flockID)
        {
            ++validNeighbors;
            averagePosition += neighbor->GetPosition();
        }
    }

    if (validNeighbors == 0)
    {
        return Vector3::Zero;
    }

    averagePosition /= static_cast<float>(validNeighbors);

    const Vector3 steering = MathHelper::GetNormalized(averagePosition - boid.GetPosition());
    return steering * m_cohesionMultiplier;
}

Vector3 BoidSteeringController::GetAlignmentSteering(const Boid& boid, const std::vector<Boid*>& neighbors) const
{
    Vector3 steering = Vector3::Zero;
    int validNeighbors = 0;

    for (const Boid* neighbor : neighbors)
    {
        if (neighbor->flockID == boid.flockID)
        {
            ++validNeighbors;
            steering += neighbor->GetSteeringDirection();
        }
    }

    if (validNeighbors == 0)
    {
        return Vector3::Zero;
    }

    steering -= boid.GetSteeringDirection(); // This is before division so the steering is smoother
    steering /= static_cast<float>(validNeighbors);
    return steering * m_alignmentMultiplier;
}

Vector3 BoidSteeringController::GetSeparationSteering(const Boid& boid, const std::vector<Boid*>& neighbors) const
{
    Vector3 steering = Vector3::Zero;

    for (const Boid* neighbor : neighbors)
    {
        const Vector3 vector_from_neighbor = boid.GetPosition() - neighbor->GetPosition();
        const float push_ratio = 1.0f - (vector_from_neighbor.LengthSquared() / NEIGHBORS_DETECTION_RADIUS_SQUARED);
        steering += MathHelper::GetNormalized(vector_from_neighbor) * push_ratio;
    }

    return steering * m_separationMultiplier;
}