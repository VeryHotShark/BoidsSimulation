#include "pch.h"
#include "ProjectileController.h"
#include "Game.h"
#include "Projectile.h"

namespace
{
    constexpr float PROJECTILE_RADIUS = 1.0f;
}

ProjectileController::ProjectileController(const Game& game)
    :  m_game(game)
    , m_leftButtonPressedLastFrame(false)
    , m_rightButtonPressedLastFrame(false)
    , m_projectileDrag(2.0f)
    , m_projectileSpeed(65.0f)
    , m_projectileEnergy(5.0f)
{
}

void ProjectileController::OnInitialize()
{
    m_projectileShape.reset();
    m_projectileShape = GetEngine().CreateSpherePrimitive(PROJECTILE_RADIUS);
}

void ProjectileController::OnShutdown()
{
    m_projectileShape.reset();
    m_projectiles.clear();
}

void ProjectileController::OnUpdate(float deltaTime, DirectX::Mouse& mouse, DirectX::GamePad& gamepad)
{
    UpdateInput(mouse, gamepad);
    UpdateProjectiles(deltaTime);
    RemovePendingProjectiles();
}

void ProjectileController::UpdateInput(DirectX::Mouse& mouse, DirectX::GamePad& gamepad)
{
    auto padState = gamepad.GetState(0);
    auto mouseState = mouse.GetState();

    const bool leftButtonPressedThisFrame = mouseState.leftButton || (padState.IsConnected() && padState.IsRightTriggerPressed());
    const bool rightButtonPressedThisFrame = mouseState.rightButton || (padState.IsConnected() && padState.IsLeftTriggerPressed());

    if (m_leftButtonPressedLastFrame && !leftButtonPressedThisFrame)
    {
        SpawnProjectile(true);
    }

    if (m_rightButtonPressedLastFrame && !rightButtonPressedThisFrame)
    {
        SpawnProjectile(false);
    }

    m_leftButtonPressedLastFrame = leftButtonPressedThisFrame;
    m_rightButtonPressedLastFrame = rightButtonPressedThisFrame;
}

void ProjectileController::UpdateProjectiles(float deltaTime)
{
    for (Projectile& projectile : m_projectiles)
    {
        projectile.CheckForSkyscrapers(m_game.GetCity().GetSkyscrapers());
        projectile.CheckForBoids(m_game.GetBoidManager().GetBoidsHashGrid());
        projectile.UpdateMovement(deltaTime);
    }
}

void ProjectileController::RemovePendingProjectiles()
{
    for (auto iter = m_projectiles.begin(); iter != m_projectiles.end(); )
    {
        if (iter->IsPendingDestroy())
        {
            iter = m_projectiles.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void ProjectileController::OnRender(framework::RenderContextPtr& renderContext)
{
    for (Projectile& projectile : m_projectiles)
    {
        const XMVECTOR projectileColor = (!projectile.IsPredator() ? Colors::Green.v :
                                            (projectile.ConsumedMax() ? Colors::Purple.v : Colors::Red.v));
        const Color finalColor = Color::Lerp(Colors::Black.v, projectileColor, (projectile.GetEnergy() / m_projectileEnergy));
        renderContext->RenderPrimitive(m_projectileShape, Vector3::One, projectile.GetPosition(), Vector3::Zero, finalColor);
    }
}

void ProjectileController::SpawnProjectile(bool predator)
{
    Projectile projectile(m_game.GetCamera().GetCameraDir() * m_projectileSpeed, m_game.GetCamera().GetCameraPos(), Vector3::One *(PROJECTILE_RADIUS * 2.0f), m_projectileDrag, m_projectileEnergy, predator);
    m_projectiles.push_back(std::move(projectile));
}

const std::vector<Projectile>& ProjectileController::GetProjectiles() const
{
    return m_projectiles;
}
