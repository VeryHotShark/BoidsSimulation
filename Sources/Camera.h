#pragma once

class Camera
{

public:
	Camera();
	~Camera();
	void OnUpdate( float deltaTime, DirectX::Keyboard& keyboard, DirectX::Mouse& mouse, DirectX::GamePad& gamepad );

	Vector3 GetCameraPos() const { return m_cameraPos; }
	Vector3 GetCameraDir() const { return m_cameraDir; }
	Vector3 GetMovementInput() const { return m_moveInput; }

private:
	void RotationInput( DirectX::Mouse& mouse, DirectX::GamePad& gamepad );
	Vector3 MovementInput( DirectX::Keyboard& keyboard, DirectX::GamePad& gamepad );

	Vector3 m_cameraDir;
	Vector3 m_cameraPos;
	Vector3 m_moveInput;

	Vector3 m_desiredPos;
	Vector3 m_desiredDir;

	float m_pitch;
	float m_yaw;

	float m_dashCooldownTimer;
	bool m_dashPressedLastFrame;
};

