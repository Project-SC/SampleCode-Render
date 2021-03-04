#include "Camera.h"

Camera::Camera(int h, int w) 
{
	m_angleYaw = m_anglePitch = m_angleRoll = 0.0f;
	m_wScreen = w;
	m_hScreen = h;
	m_fovVertical = 60.0f;
	double tmp = atan(tan(XMConvertToRadians(m_fovVertical) * 0.5) * w / h) * 2.0;
	m_fovHorizontal = XMConvertToDegrees((float)tmp);

	// projection matrix
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fovVertical), (float)w / (float)h, 0.1f, 300000.0f);
	XMStoreFloat4x4(&m_mProjection, proj);

	// set camera state
	//m_vPos = XMFLOAT4(7000.0f, 7000.0f, 150.0f, 0.0f);
	m_vPos = XMFLOAT4(0.0f, 0.0f, 150.0f, 0.0f);
	XMVECTOR look = XMVector3Normalize(XMLoadFloat4(&XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f)));
	XMStoreFloat4(&m_vStartLook, look);
	XMVECTOR left = XMVector3Cross(look, XMLoadFloat4(&XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f)));
	XMStoreFloat4(&m_vStartLeft, left);
	XMVECTOR up = XMVector3Cross(left, look);
	XMStoreFloat4(&m_vStartUp, up);

	Update();
}

Camera::~Camera() 
{
}

XMFLOAT4X4 Camera::GetViewProjectionMatrixTransposed()
{
	XMMATRIX view = XMLoadFloat4x4(&m_mView);
	XMMATRIX proj = XMLoadFloat4x4(&m_mProjection);
	XMMATRIX viewproj = XMMatrixTranspose(view * proj);
	XMFLOAT4X4 final;
	XMStoreFloat4x4(&final, viewproj);
	return final;
}

void Camera::GetViewFrustum(XMFLOAT4 planes[6])
{
	XMMATRIX view = XMLoadFloat4x4(&m_mView);
	XMMATRIX proj = XMLoadFloat4x4(&m_mProjection);
	XMFLOAT4X4 M;
	XMStoreFloat4x4(&M, view * proj);

	// left
	planes[0].x = M(0, 3) + M(0, 0);
	planes[0].y = M(1, 3) + M(1, 0);
	planes[0].z = M(2, 3) + M(2, 0);
	planes[0].w = M(3, 3) + M(3, 0);
	
	// right
	planes[1].x = M(0, 3) - M(0, 0);
	planes[1].y = M(1, 3) - M(1, 0);
	planes[1].z = M(2, 3) - M(2, 0);
	planes[1].w = M(3, 3) - M(3, 0);

	// bottom
	planes[2].x = M(0, 3) + M(0, 1);
	planes[2].y = M(1, 3) + M(1, 1);
	planes[2].z = M(2, 3) + M(2, 1);
	planes[2].w = M(3, 3) + M(3, 1);

	// top
	planes[3].x = M(0, 3) - M(0, 1);
	planes[3].y = M(1, 3) - M(1, 1);
	planes[3].z = M(2, 3) - M(2, 1);
	planes[3].w = M(3, 3) - M(3, 1);

	// near
	planes[4].x = M(0, 3) + M(0, 2);
	planes[4].y = M(1, 3) + M(1, 2);
	planes[4].z = M(2, 3) + M(2, 2);
	planes[4].w = M(3, 3) + M(3, 2);

	// far
	planes[5].x = M(0, 3) - M(0, 2);
	planes[5].y = M(1, 3) - M(1, 2);
	planes[5].z = M(2, 3) - M(2, 2);
	planes[5].w = M(3, 3) - M(3, 2);

	// normalize all planes
	for (auto i = 0; i < 6; ++i) 
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&planes[i]));
		XMStoreFloat4(&planes[i], v);
	}
}

void Camera::Translate(XMFLOAT3 move)
{
	XMVECTOR look = XMLoadFloat4(&m_vCurLook);
	XMVECTOR left = XMLoadFloat4(&m_vCurLeft);
	XMVECTOR up = XMLoadFloat4(&m_vCurUp);
	XMVECTOR tmp = XMLoadFloat4(&m_vPos);
	
	tmp += look * move.x + left * move.y + up * move.z;

	XMStoreFloat4(&m_vPos, tmp);
	
	Update();
}

void Camera::Pitch(float theta)
{
	m_anglePitch += theta;
	m_anglePitch = m_anglePitch > 360 ? m_anglePitch - 360 : m_anglePitch < -360 ? m_anglePitch + 360 : m_anglePitch;

	Update();
}

void Camera::Yaw(float theta)
{
	m_angleYaw += theta;
	m_angleYaw = m_angleYaw > 360 ? m_angleYaw - 360 : m_angleYaw < -360 ? m_angleYaw + 360 : m_angleYaw;

	Update();
}

void Camera::Roll(float theta)
{
	m_angleRoll += theta;
	m_angleRoll = m_angleRoll > 360 ? m_angleRoll - 360 : m_angleRoll < -360 ? m_angleRoll + 360 : m_angleRoll;

	Update();
}

void Camera::LockPosition(XMFLOAT4 p)
{
	m_vPos = p;

	Update();
}

bool Camera::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	XMFLOAT4 frustum[6];
	GetViewFrustum(frustum);

	XMVECTOR vFrustum[6];
	*vFrustum = XMLoadFloat4(frustum);
	

	// �� ������ �ҿ� ť���� �� ���� �ִ��� Ȯ���մϴ�.
	for (int i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(vFrustum[i], XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter - radius), 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(vFrustum[i], XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter - radius), 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(vFrustum[i], XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter - radius), 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(vFrustum[i], XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter - radius), 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(vFrustum[i], XMVectorSet((xCenter - radius), (yCenter - radius), (zCenter + radius), 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(vFrustum[i], XMVectorSet((xCenter + radius), (yCenter - radius), (zCenter + radius), 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(vFrustum[i], XMVectorSet((xCenter - radius), (yCenter + radius), (zCenter + radius), 1.0f))) >= 0.0f)
			continue;

		if (XMVectorGetX(XMPlaneDotCoord(vFrustum[i], XMVectorSet((xCenter + radius), (yCenter + radius), (zCenter + radius), 1.0f))) >= 0.0f)
			continue;

		return false;
	}

	return true;
}

void Camera::Update() 
{
	XMVECTOR look = XMLoadFloat4(&m_vStartLook);
	XMVECTOR up = XMLoadFloat4(&m_vStartUp);
	
	float pitch_rad = XMConvertToRadians(m_anglePitch);
	float yaw_rad = XMConvertToRadians(m_angleYaw);
	float roll_rad = XMConvertToRadians(m_angleRoll);

	XMMATRIX rot, rotp, roty, rotr;
	XMVECTOR left = XMLoadFloat4(&m_vStartLeft);

	rotp = XMMatrixRotationAxis(left, pitch_rad);
	roty = XMMatrixRotationAxis(up, yaw_rad);
	rotr = XMMatrixRotationAxis(look, roll_rad);
	rot = rotp * roty * rotr;
	look = XMVector3Normalize(XMVector3Transform(look, rot));
	left = XMVector3Normalize(XMVector3Transform(left, rot));
	up = XMVector3Cross(left, look);

	XMStoreFloat4(&m_vCurLook, look);
	XMStoreFloat4(&m_vCurUp, up);
	XMStoreFloat4(&m_vCurLeft, left);

	XMVECTOR camera = XMLoadFloat4(&m_vPos);
	XMVECTOR target = camera + look;
	XMMATRIX view = XMMatrixLookAtLH(camera, target, up);
	XMStoreFloat4x4(&m_mView, view);
}