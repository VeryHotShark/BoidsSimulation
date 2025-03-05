#include "pch.h"
#include "Entity.h"

Entity::Entity(Vector3 position, Vector3 size)
    : position(position)
    , bounds(position, size)
    , cellIndex(Vector3Int(0,0,0))
{
}

void Entity::Destroy()
{
    isPendingDestroy = true;
}

MovingEntity::MovingEntity(Vector3 position, Vector3 size, Vector3 velocity)
    : Entity(position, size)
    , m_velocity(velocity)
{

}

void MovingEntity::Teleport(Vector3 newPosition)
{
    position = newPosition;
    bounds.UpdateBounds(position);
}

void MovingEntity::UpdatePositionBasedOnVelocity(float deltaTime)
{
    position += m_velocity * deltaTime;
    bounds.UpdateBounds(position);
}

Vector3 MovingEntity::GetSteeringDirection() const
{
    return MathHelper::GetNormalized(m_velocity);
}
