#pragma once

class Game;
class BoidManager;
class Boid;

class BoidSteeringController
{
public:
    BoidSteeringController(const BoidManager& boidManager, const Game& game);
    Vector3 GetBoidSteering(const Boid& boid) const;

private:
    const BoidManager& m_boidManager;
    const Game& m_game;

    float m_neighborsDetectionDotThreshold;
    float m_boundsMultiplier;
    float m_cameraMultiplier;
    float m_projectileMultiplier;
    float m_skyscrapersMultiplier;
    float m_cohesionMultiplier;
    float m_alignmentMultiplier;
    float m_separationMultiplier;

    Vector3 GetBoundsSteering(const Boid& boid) const;
    Vector3 GetCameraSteering(const Boid& boid) const;
    Vector3 GetProjectileSteering(const Boid& boid) const;
    Vector3 GetSkyscrapersSteering(const Boid& boid) const;

    std::vector<Boid*> GetBoidNeighbors(const Boid& boid) const;
    Vector3 GetCohesionSteering(const Boid& boid, const std::vector<Boid*>& neighbors) const;
    Vector3 GetAlignmentSteering(const Boid& boid, const std::vector<Boid*>& neighbors) const;
    Vector3 GetSeparationSteering(const Boid& boid, const std::vector<Boid*>& neighbors) const;
};
