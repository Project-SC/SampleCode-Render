#pragma once
#include "ResourceManager.h"

class TerrainMaterial 
{
public:
	TerrainMaterial(ResourceManager* rm, const char* fnNormals, const char* fndiffuse, XMFLOAT4 colors[4]);
	~TerrainMaterial();

	// Attach our SRV to the provided root descriptor table slot.
	void Attach(ID3D12GraphicsCommandList* cmdList,	unsigned int srvDescTableIndex);
	// return the array of colors.
	XMFLOAT4* GetColors() { return m_listColors; }
private:
	ResourceManager*			m_pResMgr;
	D3D12_CPU_DESCRIPTOR_HANDLE m_hdlTextureSRV_CPU;
	D3D12_GPU_DESCRIPTOR_HANDLE m_hdlTextureSRV_GPU;
	XMFLOAT4					m_listColors[4];
};