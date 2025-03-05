#pragma once
#include "Bounds.h"
#include "MathHelper.h"

class Entity
{
public:
    Entity() = default;
    Entity(Vector3 position, Vector3 size);

    void Destroy();
    bool IsPendingDestroy() const { return isPendingDestroy; }

    const Vector3Int& GetCellIndex() const { return cellIndex; }
    void SetCellIndex(Vector3Int newCellIndex) { cellIndex = newCellIndex; }

    const Vector3& GetPosition() const { return position; }
    void SetPosition(Vector3 newPosition) { position = newPosition; }

    const Bounds& GetBounds() const { return bounds; }
    void SetBounds(Vector3 center, Vector3 size) { bounds = Bounds(center, size); }

protected:
    Vector3 position;
    Bounds bounds;

private:
    Vector3Int cellIndex;
    bool isPendingDestroy = false;
};

class MovingEntity : public Entity
{
public:
    MovingEntity() = default;
    MovingEntity(Vector3 position, Vector3 size, Vector3 velocity);

    void UpdatePositionBasedOnVelocity(float deltaTime);
    Vector3 GetSteeringDirection() const;

    Vector3 GetVelocity() const { return m_velocity; }
    void SetVelocity(Vector3 newVelocity) { m_velocity = newVelocity; }

    Vector3 GetAcceleration() const { return m_acceleration; }
    void SetAcceleration(Vector3 newAcceleration) { m_acceleration = newAcceleration; }

protected:
    Vector3 m_velocity;
    Vector3 m_acceleration;

    void Teleport(Vector3 newPosition);
};