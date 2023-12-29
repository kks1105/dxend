#include "Framework.h"
#include "Camera.h"

Camera::Camera()
{
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matRotation);

	Rotation();
	View();
}

Camera::~Camera()
{
}

void Camera::Position(float x, float y, float z)
{
	Position(Vector3(x, y, z));
}

void Camera::Position(Vector3& vec)
{
	position = vec;

	Move();
}

void Camera::Position(Vector3* vec)
{
	*vec = position;
}

void Camera::Rotation(float x, float y, float z)
{
	Rotation(Vector3(x, y, z));
}

void Camera::Rotation(Vector3& vec)
{
	rotation = vec;

	Rotation();
}

void Camera::Rotation(Vector3* vec)
{
	*vec = rotation;
}

void Camera::RotationDegree(float x, float y, float z)
{
	RotationDegree(Vector3(x, y, z));
}

void Camera::RotationDegree(Vector3& vec)
{
	rotation = vec * 0.01745328f; //PI / 180

	Rotation(rotation);
}

void Camera::RotationDegree(Vector3* vec)
{
	*vec = rotation * 57.29577957f;
}

void Camera::GetMatrix(Matrix* matrix)
{
	*matrix = matView;
}

void Camera::Rotation()
{
	D3DXMatrixRotationYawPitchRoll(&matRotation, rotation.y, rotation.x, rotation.z);

	D3DXVec3TransformNormal(&right, &Vector3(1, 0, 0), &matRotation);
	D3DXVec3TransformNormal(&up, &Vector3(0, 1, 0), &matRotation);
	D3DXVec3TransformNormal(&forward, &Vector3(0, 0, 1), &matRotation);
}

void Camera::Move()
{
	View();
}

void Camera::View()
{
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}
