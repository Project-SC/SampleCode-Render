#pragma once

#include "ResourceManager.h"
#include "Light.h"

using namespace graphics;

struct PerFrameConstantBuffer 
{
	XMFLOAT4X4	viewproj;
	XMFLOAT4X4	shadowtexmatrices[4];
	XMFLOAT4	eye;
	XMFLOAT4	frustum[6];
	LightSource light;
	BOOL		useTextures;
};

struct ShadowMapShaderConstants
{
	XMFLOAT4X4	shadowViewProj;
	XMFLOAT4	eye;
	XMFLOAT4	frustum[4];
};

class Frame 
{
public:
	Frame(unsigned int indexFrame, Device* dev, ResourceManager* rm, unsigned int h, unsigned int w, 
		unsigned int dimShadowAtlas = 4096);
	~Frame();

	ID3D12CommandAllocator* GetAllocator() { return m_pCmdAllocator; }

	void Reset();
	void AttachCommandList(ID3D12GraphicsCommandList* cmdList);

	void SetFrameConstants(PerFrameConstantBuffer frameConstants);
	void SetShadowConstants(ShadowMapShaderConstants shadowConstants, unsigned int i);
	void BeginShadowPass(ID3D12GraphicsCommandList* cmdList);
	void EndShadowPass(ID3D12GraphicsCommandList* cmdList);
	void BeginRenderPass(ID3D12GraphicsCommandList* cmdList, const float clearColor[4]);
	void EndRenderPass(ID3D12GraphicsCommandList* cmdList);
	void AttachShadowPassResources(unsigned int i, ID3D12GraphicsCommandList* cmdList, 
		unsigned int cbvDescTableIndex);
	void AttachFrameResources(ID3D12GraphicsCommandList* cmdList, unsigned int srvDescTableIndex, 
		unsigned int cbvDescTableIndex);
	
private:
	void InitShadowAtlas();
	void InitConstantBuffers();

	void WaitForGPU();

	Device*						m_pDev;
	ResourceManager*			m_pResMgr;
	ID3D12CommandAllocator*		m_pCmdAllocator;
	ID3D12Resource*				m_pBackBuffer;
	ID3D12Resource*				m_pDepthStencilBuffer;
	ID3D12Resource*				m_pShadowAtlas;
	ID3D12Resource*				m_pFrameConstants;
	ID3D12Resource*				m_pShadowConstants[4];
	ID3D12Fence*				m_pFence;
	HANDLE						m_hdlFenceEvent;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hdlBackBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hdlDSV;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hdlShadowAtlasDSV;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hdlShadowAtlasSRV_CPU;
	D3D12_GPU_DESCRIPTOR_HANDLE m_hdlShadowAtlasSRV_GPU;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hdlFrameConstantsCBV_CPU;
	D3D12_GPU_DESCRIPTOR_HANDLE m_hdlFrameConstantsCBV_GPU;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hdlShadowConstantsCBV_CPU[4];
	D3D12_GPU_DESCRIPTOR_HANDLE m_hdlShadowConstantsCBV_GPU[4];
	D3D12_VIEWPORT				m_vpShadowAtlas[4];
	D3D12_RECT					m_srShadowAtlas[4];
	PerFrameConstantBuffer*		m_pFrameConstantsMapped;
	ShadowMapShaderConstants*	m_pShadowConstantsMapped[4];
	unsigned long long			m_valFence;						// Value to check fence against to confirm GPU is done.
	unsigned int				m_iFrame;						// Which frame number is this frame?
	unsigned int				m_wScreen;
	unsigned int				m_hScreen;
	unsigned int				m_wShadowAtlas;
	unsigned int				m_hShadowAtlas;
};