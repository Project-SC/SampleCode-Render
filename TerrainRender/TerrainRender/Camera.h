#pragma once

#include <DirectXMath.h>
#include "BoundingVolume.h"

using namespace DirectX;

struct Frustum 
{
	XMFLOAT3 nlb;
	XMFLOAT3 nlt;
	XMFLOAT3 nrb;
	XMFLOAT3 nrt;
	XMFLOAT3 flb;
	XMFLOAT3 flt;
	XMFLOAT3 frb;
	XMFLOAT3 frt;
	BoundingSphere bs;
};

class Camera
{
public:
	Camera(int h, int w);
	~Camera();

	XMFLOAT4X4 GetViewProjectionMatrixTransposed();
	XMFLOAT4 GetEyePosition() { return m_vPos; }
	void GetViewFrustum(XMFLOAT4 planes[6]);
	void Translate(XMFLOAT3 move);
	void Pitch(float theta);
	void Yaw(float theta);
	void Roll(float theta);
	void LockPosition(XMFLOAT4 p);
	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);

private:
	void Update();
	
	XMFLOAT4X4	m_mProjection;	// Projection matrix
	XMFLOAT4X4	m_mView;		// View matrix
	XMFLOAT4	m_vPos;			// Camera position
	XMFLOAT4	m_vStartLook;	// Starting lookat vector
	XMFLOAT4	m_vStartUp;		// Starting up vector
	XMFLOAT4	m_vStartLeft;	// Starting left vector
	XMFLOAT4	m_vCurLook;		// Current lookat vector
	XMFLOAT4	m_vCurUp;		// Current up vector
	XMFLOAT4	m_vCurLeft;		// Current left vector
	int			m_wScreen;
	int			m_hScreen;
	float		m_fovHorizontal;
	float		m_fovVertical;
	float		m_angleYaw;
	float		m_anglePitch;
	float		m_angleRoll;
};