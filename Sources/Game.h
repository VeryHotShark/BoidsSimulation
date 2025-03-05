#pragma once
#include "BoidManager.h"
#include "IGame.h"
#include "IEngine.h"
#include "IRenderContext.h"
#include "City.h"
#include "Camera.h"
#include "Crosshair.h"
#include "ProjectileController.h"

class Game final : public framework::IGame
{
public:
	Game();
	virtual ~Game() override;

	Game( Game&& ) = default;
	Game& operator= ( Game&& ) = default;

	Game( Game const& ) = delete;
	Game& operator= ( Game const& ) = delete;

	void OnInitialize() override;
    void OnUpdate( float deltaTime, DirectX::Keyboard& keyboard, DirectX::Mouse& mouse, DirectX::GamePad& gamepad ) override;
	void OnRender( framework::RenderContextPtr& renderContext ) override;
	void OnShutdown() override;

	const City& GetCity() const { return *m_city.get(); }
	const Camera& GetCamera() const { return *m_camera.get(); }
	const ProjectileController& GetProjectileController() const { return *m_projectileController.get(); }

	BoidManager& GetBoidManager() { return *m_boidManager.get(); }
	const BoidManager& GetBoidManager() const { return  *m_boidManager.get(); }

private:
    std::unique_ptr< City >					                m_city;
	std::unique_ptr< Camera >				                m_camera;
    std::unique_ptr< Crosshair >			                m_crosshair;
	std::unique_ptr< BoidManager >			                m_boidManager;
	std::unique_ptr< ProjectileController >			        m_projectileController;
};

