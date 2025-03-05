#include "pch.h"
#include "Camera.h"
#include "IEngine.h"

namespace
{
	const Vector3 START_POSITION = { 0.0f, 17.0f, -40.5f };
	constexpr float ROTATION_GAIN = 0.075f;
	constexpr float MOVEMENT_GAIN = 0.20f;
	constexpr float DASH_GAIN = 40.5f;
	constexpr float DASH_COOLDOWN = 1.5f;
	constexpr float SMOOTHSTEP_MOVEMENT_MULTIPLIER = 10.0f;
	constexpr float SMOOTHSTEP_ROTATION_MULTIPLIER = 7.0f;
}

Camera::Camera() :
	m_pitch(0),
	m_yaw(0),
	m_cameraPos( START_POSITION ) ,
    m_desiredPos( START_POSITION) ,
    m_dashPressedLastFrame(false) ,
    m_dashCooldownTimer(0.0f)
{
}

Camera::~Camera() = default;

void Camera::OnUpdate( float deltaTime, DirectX::Keyboard& keyboard, DirectX::Mouse& mouse, DirectX::GamePad& gamepad )
{
	UNREFERENCED_PARAMETER( deltaTime );

	framework::IEngine& engine = GetEngine();

    mouse.SetMode( Mouse::MODE_RELATIVE );

	RotationInput( mouse, gamepad );
	m_moveInput = MovementInput(keyboard, gamepad);
	Vector3 move = m_moveInput;

	auto kb = keyboard.GetState();

	m_dashCooldownTimer -= deltaTime;

	if(m_dashPressedLastFrame && !kb.LeftShift && m_dashCooldownTimer < 0.0f)
	{
		m_dashCooldownTimer = DASH_COOLDOWN;
		move *= DASH_GAIN;
	}

	m_dashPressedLastFrame = kb.LeftShift;


	const float limit = M_PI / 2.0f - 0.01f;
	m_pitch = std::max( -limit, std::min( +limit, m_pitch ) );

	if ( m_yaw > M_PI )
	{
		m_yaw -= M_PI * 2.f;
	}
	else if ( m_yaw < -M_PI )
	{
		m_yaw += M_PI * 2.f;
	}

	Quaternion q = Quaternion::CreateFromYawPitchRoll( m_yaw, m_pitch, 0.f );
	move = Vector3::Transform( move, q );
	move.y *= -1.0f;
	move *= MOVEMENT_GAIN;

	m_desiredPos += move;
	m_cameraPos = Vector3::SmoothStep(m_cameraPos, m_desiredPos, SMOOTHSTEP_MOVEMENT_MULTIPLIER * deltaTime);

	float y = sinf( m_pitch );
	float r = cosf( m_pitch );
	float z = r * cosf( m_yaw );
	float x = r * sinf( m_yaw );

	m_desiredDir = Vector3(x, y, z);
	m_cameraDir = Vector3::SmoothStep(m_cameraDir, m_desiredDir, SMOOTHSTEP_ROTATION_MULTIPLIER * deltaTime);;
	Vector3 lookAt = m_cameraPos + m_cameraDir;
	engine.LookAt( m_cameraPos, lookAt );
}

void Camera::RotationInput( DirectX::Mouse& mouse, DirectX::GamePad& gamepad )
{
	// GamePad
	auto padState = gamepad.GetState( 0 );
	if ( padState.IsConnected() )
	{

		if ( padState.IsRightStickPressed() )
		{
			m_yaw = m_pitch = 0.f;
		}
		else
		{
			m_yaw += -padState.thumbSticks.rightX * ROTATION_GAIN;
			m_pitch += padState.thumbSticks.rightY * ROTATION_GAIN;
		}
	}

	// Mouse
	auto mouseState = mouse.GetState();
	if ( mouseState.positionMode == DirectX::Mouse::MODE_RELATIVE )
	{
		Vector3 delta = Vector3( float( mouseState.x ), float( mouseState.y ), 0.f )
			* ROTATION_GAIN;

		m_pitch -= delta.y * ROTATION_GAIN;
		m_yaw -= delta.x * ROTATION_GAIN;
	}
}

Vector3 Camera::MovementInput( DirectX::Keyboard& keyboard, DirectX::GamePad& gamepad )
{
	Vector3 move = Vector3::Zero;

	// GamePad
	auto padState = gamepad.GetState( 0 );
	if ( padState.IsConnected() )
	{
		move.x += -padState.thumbSticks.leftX;
		move.z += padState.thumbSticks.leftY;
	}

	// Keyboard
    auto kb = keyboard.GetState();
	if ( kb.Up || kb.W )
		move.z += 1.f;

	if ( kb.Down || kb.S )
		move.z -= 1.f;

	if ( kb.Left || kb.A )
		move.x += 1.f;

	if ( kb.Right || kb.D )
		move.x -= 1.f;

	if ( kb.PageUp || kb.Space )
		move.y -= 1.f;

	if ( kb.PageDown || kb.LeftControl )
		move.y += 1.f;

	return move;
}
