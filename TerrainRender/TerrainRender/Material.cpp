#include "Material.h"

TerrainMaterial::TerrainMaterial(ResourceManager* rm, const char* fnNormals, const char* fndiffuse, XMFLOAT4 colors[4]) : m_pResMgr(rm)
{
	unsigned int index[2], height, width;

	index[0] = m_pResMgr->LoadFile(fnNormals, height, width);
	index[1] = m_pResMgr->LoadFile(fndiffuse, height, width);

	D3D12_RESOURCE_DESC	descTex = {};
	descTex.MipLevels = 1;
	descTex.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descTex.Width = width;
	descTex.Height = height;
	descTex.Flags = D3D12_RESOURCE_FLAG_NONE;
	descTex.DepthOrArraySize = 8;
	descTex.SampleDesc.Count = 1;
	descTex.SampleDesc.Quality = 0;
	descTex.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	ID3D12Resource* textures;
	unsigned int iBuffer = m_pResMgr->NewBuffer(textures, &descTex, &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr);
	textures->SetName(L"Texture Array Buffer");

	D3D12_SUBRESOURCE_DATA dataTex[2];

	for (int i = 0; i < 2; i++)
	{
		dataTex[i] = {};
		dataTex[i].pData = m_pResMgr->GetFileData(index[0]);
		dataTex[i].RowPitch = width * 4 * sizeof(unsigned char);
		dataTex[i].SlicePitch = height * width * 4 * sizeof(unsigned char);
	}
	
	m_pResMgr->UploadToBuffer(iBuffer, 1, dataTex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	D3D12_SHADER_RESOURCE_VIEW_DESC	descSRV = {};
	descSRV.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	descSRV.Format = descTex.Format;
	descSRV.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	descSRV.Texture2DArray.ArraySize = descTex.DepthOrArraySize;
	descSRV.Texture2DArray.MipLevels = descTex.MipLevels;

	m_pResMgr->AddSRV(textures, &descSRV, m_hdlTextureSRV_CPU, m_hdlTextureSRV_GPU);

	m_listColors[0] = colors[0];
	m_listColors[1] = colors[1];
	m_listColors[2] = colors[2];
	m_listColors[3] = colors[3];
}

TerrainMaterial::~TerrainMaterial() 
{
	m_pResMgr = nullptr;
}

void TerrainMaterial::Attach(ID3D12GraphicsCommandList* cmdList, unsigned int srvDescTableIndex) 
{
	cmdList->SetGraphicsRootDescriptorTable(srvDescTableIndex, m_hdlTextureSRV_GPU);
}