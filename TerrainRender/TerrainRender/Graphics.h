#pragma once

// Linkages
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "D3DX12.h"
#include <dxgi1_5.h>
#include <DirectXMath.h>
#include <D3DCompiler.h>
#include <stdexcept>

namespace graphics 
{
	using namespace DirectX;

	static const float SCREEN_DEPTH = 1000.0f;
	static const float SCREEN_NEAR = 0.1f;
	static const UINT FACTORY_DEBUG = DXGI_CREATE_FACTORY_DEBUG;
	static const DXGI_FORMAT DESIRED_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
	static const D3D_FEATURE_LEVEL	FEATURE_LEVEL = D3D_FEATURE_LEVEL_11_0;
	enum ShaderType { PIXEL_SHADER, VERTEX_SHADER, GEOMETRY_SHADER, HULL_SHADER, DOMAIN_SHADER };

	class GFX_Exception : public std::runtime_error 
	{
	public:
		GFX_Exception(const char *msg) : std::runtime_error(msg) {}
	};
	
	void CompileShader(LPCWSTR fn, ShaderType st, D3D12_SHADER_BYTECODE& bcShader);

	class Device 
	{
	public:
		Device(HWND win, unsigned int h, unsigned int w, bool fullscreen = false, unsigned int numFrames = 3);
		~Device();

		unsigned int GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE ht);

		void CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE clt, ID3D12CommandAllocator*& allocator);
		void GetBackBuffer(unsigned int i, ID3D12Resource*& buffer);
		unsigned int GetCurrentBackBuffer();
		void SetFence(ID3D12Fence* fence, unsigned long long val);

		void CreateRootSig(CD3DX12_ROOT_SIGNATURE_DESC* desc, ID3D12RootSignature*& root);
		void CreatePSO(D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc, ID3D12PipelineState*& pso);
		
		void CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_DESC* desc, ID3D12DescriptorHeap*& heap);
		void CreateSRV(ID3D12Resource*& tex, D3D12_SHADER_RESOURCE_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void CreateCBV(D3D12_CONSTANT_BUFFER_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void CreateDSV(ID3D12Resource*& tex, D3D12_DEPTH_STENCIL_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void CreateRTV(ID3D12Resource*& tex, D3D12_RENDER_TARGET_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void CreateSampler(D3D12_SAMPLER_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void CreateFence(unsigned long long valInit, D3D12_FENCE_FLAGS flags, ID3D12Fence*& fence);
		void CreateGraphicsCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator* alloc, ID3D12GraphicsCommandList*& list,
			unsigned int mask = 0, ID3D12PipelineState* psoInit = nullptr);

		void CreateCommittedResource(ID3D12Resource*& heap, D3D12_RESOURCE_DESC* desc, D3D12_HEAP_PROPERTIES* props, D3D12_HEAP_FLAGS flags,
			D3D12_RESOURCE_STATES state, D3D12_CLEAR_VALUE* clear);

		void ExecuteCommandLists(ID3D12CommandList* lCmds[], unsigned int numCommands);
		void Present();
		
	private:
		ID3D12Device*				m_pDev;
		ID3D12CommandQueue*			m_pCmdQ;
		IDXGISwapChain3*			m_pSwapChain;
		unsigned int				m_wScreen;
		unsigned int				m_hScreen;
		unsigned int				m_numFrames;
		bool						m_isWindowed;
	};
};