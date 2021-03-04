#pragma once
#include "D3DX12.h"
#include "Camera.h"
#include "Terrain.h"
#include <DirectXMath.h>

using namespace DirectX;

const int MAX_TRIANGLES = 10000;

class QuadTree
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT3 texture;
		XMFLOAT3 normal;
	};

	struct NodeType
	{
        float positionX, positionZ, width;
		int triangleCount;
		D3D12_SUBRESOURCE_DATA* dataVB, dataIB;
        NodeType* nodes[4];
	};

public:
	QuadTree(Camera *Cam);
	~QuadTree();

	bool Initialize(int vertexCount);
	void Shutdown();
	int GetDrawCount();
	void checkFrustum(NodeType* node);

private:
	void CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth);
	void CreateTreeNode(NodeType* node, float positionX, float positionZ, float width);
	int CountTriangles(float positionX, float positionZ, float width);
	bool IsTriangleContained(int index, float positionX, float positionZ, float width);
	void ReleaseNode(NodeType*);

private:
	Camera* m_pCam;
	Terrain* m_pTerrain;
	int m_triangleCount, m_drawCount;
	VertexType* m_vertexList = nullptr;
	NodeType* m_parentNode = nullptr;
};