#include "pch.h"
#include "Game.h"

Game::Game()
{
    m_city = std::make_unique< City >();
    m_camera = std::make_unique< Camera >();
    m_crosshair = std::make_unique< Crosshair >(*this);
    m_boidManager = std::make_unique< BoidManager >(*this);
    m_projectileController = std::make_unique< ProjectileController >(*this);
}

Game::~Game() = default;

void Game::OnInitialize()
{
    m_city->OnInitialize();
    m_crosshair->OnInitialize();
    m_boidManager->OnInitialize();
    m_projectileController->OnInitialize();
}

void Game::OnUpdate( float deltaTime, DirectX::Keyboard& keyboard, DirectX::Mouse& mouse, DirectX::GamePad& gamepad )
{
	m_camera->OnUpdate( deltaTime, keyboard, mouse, gamepad );
	m_city->OnUpdate( deltaTime );
    m_crosshair->OnUpdate( deltaTime, mouse, gamepad);
    m_boidManager->OnUpdate(deltaTime, keyboard);
    m_projectileController->OnUpdate(deltaTime, mouse, gamepad);
}

void Game::OnRender( framework::RenderContextPtr& renderContext )
{
	m_city->OnRender( renderContext );
    m_boidManager->OnRender(renderContext);
    m_projectileController->OnRender(renderContext);
    m_crosshair->OnRender( renderContext );
}

void Game::OnShutdown()
{
	m_city->OnShutdown();
    m_crosshair->OnShutdown();
    m_boidManager->OnShutdown();
    m_projectileController->OnShutdown();
}
