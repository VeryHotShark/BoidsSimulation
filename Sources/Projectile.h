#pragma once
#include "Entity.h"
#include "SpatialHashGrid.h"

class Boid;
class Entity;

class Projectile : public MovingEntity
{
public:
	using OnDestroyDelegate = std::function<void(Vector3 position, Vector3 velocity)>;
	static OnDestroyDelegate OnDestroy;

	Projectile(Vector3 velocity, Vector3 position, Vector3 size, float drag, float energy, bool predator);

	bool ConsumedMax() const;

	void UpdateMovement(float deltaTime);
	void CheckForBoids(const SpatialHashGrid<Boid>& boidsHashGrid);
	void CheckForSkyscrapers(const std::vector<Entity>& skyscrapers);

	float GetEnergy() const { return m_energy; }
	bool IsPredator() const { return m_isPredator;  }

private:
	bool CanConsume() const;

	float m_drag;
	float m_energy;
	bool m_isPredator;
	int m_consumedBoidsCount;
};
