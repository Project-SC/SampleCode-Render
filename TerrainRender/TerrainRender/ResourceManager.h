#pragma once

#include "Graphics.h"
#include <vector>

using namespace graphics;

static const unsigned long long DEFAULT_UPLOAD_BUFFER_SIZE = 100000000;

class ResourceManager
{
public:
	ResourceManager(Device* d, unsigned int numRTVs, unsigned int numDSVs, unsigned int numCBVSRVUAVs, 
		unsigned int numSamplers);
	~ResourceManager();

	ID3D12DescriptorHeap* GetCBVSRVUAVHeap() { return m_pheapCBVSRVUAV; }

	void AddRTV(ID3D12Resource* tex, D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handleCPU);
	void AddDSV(ID3D12Resource* tex, D3D12_DEPTH_STENCIL_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handleCPU);
	void AddCBV(D3D12_CONSTANT_BUFFER_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handleCPU,
		D3D12_GPU_DESCRIPTOR_HANDLE& handleGPU);
	void AddSRV(ID3D12Resource* tex, D3D12_SHADER_RESOURCE_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handleCPU,
		D3D12_GPU_DESCRIPTOR_HANDLE& handleGPU);
	void AddSampler(D3D12_SAMPLER_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE& handleCPU);

	unsigned int AddExistingResource(ID3D12Resource* tex);
	unsigned int NewBuffer(ID3D12Resource*& buffer, D3D12_RESOURCE_DESC* descBuffer, D3D12_HEAP_PROPERTIES* props,
		D3D12_HEAP_FLAGS flags, D3D12_RESOURCE_STATES state, D3D12_CLEAR_VALUE* clear);
	unsigned int NewBufferAt(unsigned int i, ID3D12Resource*& buffer, D3D12_RESOURCE_DESC* descBuffer,
		D3D12_HEAP_PROPERTIES* props, D3D12_HEAP_FLAGS flags, D3D12_RESOURCE_STATES state, D3D12_CLEAR_VALUE* clear);
	void UploadToBuffer(unsigned int i, unsigned int numSubResources, D3D12_SUBRESOURCE_DATA* data, D3D12_RESOURCE_STATES initialState);

	ID3D12Resource* GetResource(unsigned int index);

	unsigned int LoadFile(const char* fn, unsigned int& h, unsigned int& w);
	unsigned char* GetFileData(unsigned int i);
	void UnloadFileData(unsigned int i);
	void WaitForGPU();

private:
	Device*							m_pDev;
	ID3D12CommandAllocator*			m_pCmdAllocator;
	ID3D12GraphicsCommandList*		m_pCmdList;
	ID3D12Fence*					m_pFence;
	HANDLE							m_hdlFenceEvent;
	ID3D12DescriptorHeap*			m_pheapRTV;						// Render Target View
	ID3D12DescriptorHeap*			m_pheapDSV;						// Depth Stencil View
	ID3D12DescriptorHeap*			m_pheapCBVSRVUAV;				// Constant Buffer View, Shader Resource View, Unordered Access View
	ID3D12DescriptorHeap*			m_pheapSampler;					// Sampler
	std::vector<ID3D12Resource*>	m_listResources;
	std::vector<unsigned char*>		m_listFileData;					
	ID3D12Resource*					m_pUpload;
	unsigned long long				m_iUpload;
	unsigned long long				m_valFence;
	unsigned int					m_numRTVs;
	unsigned int					m_numDSVs;
	unsigned int					m_numCBVSRVUAVs;
	unsigned int					m_numSamplers;
	unsigned int					m_indexFirstFreeSlotRTV;
	unsigned int					m_indexFirstFreeSlotDSV;
	unsigned int					m_indexFirstFreeSlotCBVSRVUAV;
	unsigned int					m_indexFirstFreeSlotSampler;
	unsigned int					m_sizeRTVHeapDesc;
	unsigned int					m_sizeDSVHeapDesc;
	unsigned int					m_sizeCBVSRVUAVHeapDesc;
	unsigned int					m_sizeSamplerHeapDesc;
};