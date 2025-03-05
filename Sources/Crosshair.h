#pragma once
#include "IRenderContext.h"

class Game;

class Crosshair
{

public:
    Crosshair(const Game& game);
    ~Crosshair();

    void OnInitialize();
    void OnUpdate( float deltaTime, DirectX::Mouse& mouse, DirectX::GamePad& gamepad);
    void OnRender( cdp_framework::RenderContextPtr& renderContext );
    void OnShutdown();

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        m_crosshair     = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>        m_hud = nullptr;

    const Game& m_game;

    Vector2                                                 m_origin        = Vector2::Zero;
    Vector2                                                 m_hudOrigin     = Vector2::Zero;
    Color                                                   m_color         = Colors::DarkRed.v;
    float                                                   m_rotation      = 0.0f;
    float                                                   m_scale         = 0.1f;

    Vector3                                                 m_desiredMove = Vector3::Zero;
};

