#pragma once
#include "IRenderContext.h"
#include "Projectile.h"
#include "SpatialHashGrid.h"

class Boid;
class Skyscraper;
class Game;

class ProjectileController
{
public:
	ProjectileController(const Game& game);

	void OnInitialize();
    void UpdateInput(DirectX::Mouse& mouse, DirectX::GamePad& gamepad);
    void OnUpdate(float deltaTime, DirectX::Mouse& mouse, DirectX::GamePad& gamepad);
	void OnRender(framework::RenderContextPtr& renderContext);
	void OnShutdown();

    void UpdateProjectiles(float deltaTime);
    void RemovePendingProjectiles();

	const std::vector<Projectile>& GetProjectiles() const;
private:
	const Game& m_game;

	bool m_leftButtonPressedLastFrame;
	bool m_rightButtonPressedLastFrame;

	float m_projectileDrag;
	float m_projectileSpeed;
	float m_projectileEnergy;

	std::vector<Projectile> m_projectiles;
	std::unique_ptr< DirectX::GeometricPrimitive > m_projectileShape;

	void SpawnProjectile(bool predator);

};

