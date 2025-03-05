#include "pch.h"
#include "Crosshair.h"
#include "Engine.h"
#include "Game.h"

Crosshair::Crosshair(const Game& game) : m_game(game) {};
Crosshair::~Crosshair() = default;

namespace 
{
    constexpr float SMOOTHSTEP_MULTIPLIER = 7.5f;
    constexpr float MAX_HUD_ROTATION = 0.03f;
    constexpr float MAX_HUD_MOVEMENT = 15.0f;
    constexpr float MAX_HUD_SCALE_OFFSET = 0.05f;
}

void Crosshair::OnInitialize()
{
    Vector2 textureSize;
    m_crosshair = GetEngine().CreateTexture( L"../../data/assets/crosshair.png", &textureSize );

    m_origin.x = textureSize.x * 0.5f;
    m_origin.y = textureSize.y * 0.5f;

    m_hud = GetEngine().CreateTexture(L"../../data/assets/cockpit.png", &textureSize);
    m_hudOrigin = textureSize * 0.5f;
}

void Crosshair::OnUpdate( float deltaTime, DirectX::Mouse& mouse, DirectX::GamePad& gamepad)
{
    auto padState = gamepad.GetState( 0 );
    auto mouseState = mouse.GetState();
    if ( ( padState.IsConnected() && padState.IsRightTriggerPressed() ) || mouseState.leftButton )
    {
        m_color = Colors::Red.v;
        m_scale = 0.11f;
        m_rotation = -25.0f;
    }
    else
    {
        m_color = Colors::DarkRed.v;
        m_scale = 0.1f;
        m_rotation = 0.0f;
    }

    if ( ( padState.IsConnected() && padState.IsLeftTriggerPressed() ) || mouseState.rightButton )
    {
        m_color = Colors::Green.v;
        m_scale = 0.09f;
        m_rotation = 25.0f;
    }

    m_desiredMove = Vector3::SmoothStep(m_desiredMove, m_game.GetCamera().GetMovementInput(), SMOOTHSTEP_MULTIPLIER * deltaTime);
}

void Crosshair::OnRender( framework::RenderContextPtr& renderContext )
{
    Vector2 windowSize = GetEngine().GetWindowSize();
    renderContext->RenderTexture( m_crosshair, windowSize * 0.5f, m_color, m_rotation, m_origin, m_scale );
    //renderContext->RenderTexture(m_hud, (windowSize * 0.5f) + m_desiredMove * MAX_HUD_MOVEMENT, { 0.f, 0.f, 0.445098066f, 1.f }, -m_desiredMove.x * MAX_HUD_ROTATION , m_hudOrigin, 1.0f + m_desiredMove.z * MAX_HUD_SCALE_OFFSET);
}

void Crosshair::OnShutdown()
{
    m_crosshair.Reset();
    m_hud.Reset();
}
