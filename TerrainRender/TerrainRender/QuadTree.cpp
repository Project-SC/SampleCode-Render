#include "quadtree.h"
#include <iostream>

using namespace std;

QuadTree::QuadTree(Camera* Cam)
	: m_pCam(Cam)
{
}

QuadTree::~QuadTree()
{
}

bool QuadTree::Initialize(int vertexCount)
{
	float centerX = 0.0f;
	float centerZ = 0.0f;
	float width = 0.0f;

	int nVertexCount = vertexCount;

	m_triangleCount = nVertexCount / 3;

	CalculateMeshDimensions(nVertexCount, centerX, centerZ, width);

	m_parentNode = new NodeType;
	if (!m_parentNode)
	{
		return false;
	}

	CreateTreeNode(m_parentNode, centerX, centerZ, width);

	return true;
}


void QuadTree::Shutdown()
{
	if(m_parentNode)
	{
		ReleaseNode(m_parentNode);
		delete m_parentNode;
		m_parentNode = 0;
	}
}


int QuadTree::GetDrawCount()
{
	return m_drawCount;
}


void QuadTree::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
	centerX = 0.0f;
	centerZ = 0.0f;

	for(int i=0; i<vertexCount; i++)
	{
		centerX += m_vertexList[i].position.x;
		centerZ += m_vertexList[i].position.z;
	}

	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	float minWidth = fabsf(m_vertexList[0].position.x - centerX);
	float minDepth = fabsf(m_vertexList[0].position.z - centerZ);

	for(int i=0; i<vertexCount; i++)
	{
		float width = fabsf(m_vertexList[i].position.x - centerX);	
		float depth = fabsf(m_vertexList[i].position.z - centerZ);	

		if(width > maxWidth) { maxWidth = width; }
		if(depth > maxDepth) { maxDepth = depth; }
		if(width < minWidth) { minWidth = width; }
		if(depth < minDepth) { minDepth = depth; }
	}

	float maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)max(fabs(minDepth), fabs(maxDepth));
	
	meshWidth = max(maxX, maxZ) * 2.0f;
}


void QuadTree::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width)
{
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	node->triangleCount = 0;

	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	int numTriangles = CountTriangles(positionX, positionZ, width);

	if (numTriangles == 0)
	{
		return;
	}

	if (numTriangles > MAX_TRIANGLES)
	{
		for (int i = 0; i < 4; i++)
		{
			float offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
			float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

			int count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			if (count > 0)
			{
				node->nodes[i] = new NodeType;
				CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
			}
		}
		return;
	}
}


int QuadTree::CountTriangles(float positionX, float positionZ, float width)
{
	int count = 0;

	for(int i=0; i<m_triangleCount; i++)
	{
		if(IsTriangleContained(i, positionX, positionZ, width) == true)
		{
			count++;
		}
	}

	return count;
}


bool QuadTree::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
	float radius = width / 2.0f;

	int vertexIndex = index * 3;

	float x1 = m_vertexList[vertexIndex].position.x;
	float z1 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;
	
	float x2 = m_vertexList[vertexIndex].position.x;
	float z2 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;

	float x3 = m_vertexList[vertexIndex].position.x;
	float z3 = m_vertexList[vertexIndex].position.z;

	float minimumX = min(x1, min(x2, x3));
	if(minimumX > (positionX + radius))
	{
		return false;
	}

	float maximumX = max(x1, max(x2, x3));
	if(maximumX < (positionX - radius))
	{
		return false;
	}

	float minimumZ = min(z1, min(z2, z3));
	if(minimumZ > (positionZ + radius))
	{
		return false;
	}

	float maximumZ = max(z1, max(z2, z3));
	if(maximumZ < (positionZ - radius))
	{
		return false;
	}

	return true;
}


void QuadTree::ReleaseNode(NodeType* node)
{
	for(int i=0; i<4; i++)
	{
		if(node->nodes[i] != 0)
		{
			ReleaseNode(node->nodes[i]);
		}
	}

	for(int i=0; i<4; i++)
	{
		if(node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}
}


void QuadTree::checkFrustum(NodeType* node)
{
	int count = 0;
	for (int i = 0; i < 4; i++)
	{
		if (node->nodes[i] != 0)
		{
			if (!m_pCam->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f)))
			{
				count++;
				return;
			}
			else
			{
				checkFrustum(node->nodes[i]);
			}

		}
	}

	if (count != 0)
	{
		return;
	}
}
