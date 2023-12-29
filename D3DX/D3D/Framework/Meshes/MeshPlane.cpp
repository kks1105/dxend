#include "Framework.h"
#include "MeshPlane.h"

MeshPlane::MeshPlane( float tileU, float tileV)
	:  tileU(tileU)
	, tileV(tileV)
{
}

MeshPlane::~MeshPlane()
{

}

void MeshPlane::Create()
{
	UINT countX = 11;
	UINT countZ = 11;

	float w = (countX - 1) * 0.5f;
	float h = (countZ - 1) * 0.5f;

	vector<VertexMesh> v;
	for (UINT z = 0; z < countZ; z++)
	{
		for (UINT x = 0; x < countX; x++)
		{
			VertexMesh vertex;
			vertex.Position = Vector3((float)x - w, 0.f, (float)z - h);
			vertex.Normal = Vector3(0, 1, 0);
			vertex.Uv.x = (float)x / (float)(countX - 1) * tileU;
			vertex.Uv.y = (float)z / (float)(countZ - 1) * tileV;

			v.push_back(vertex);
		}
	}

	vertexCount = v.size();
	vertices = new VertexMesh[vertexCount];

	copy(v.begin(), v.end(), stdext::checked_array_iterator<VertexMesh*>(vertices, vertexCount));

	vector<UINT> i;
	for (UINT z = 0; z < countZ - 1; z++)
	{
		for (UINT x = 0; x < countX - 1; x++)
		{
			i.push_back(countX * z + x);
			i.push_back(countX * (z + 1) + x);
			i.push_back(countX * z + (x + 1));

			i.push_back(countX * z + (x + 1));
			i.push_back(countX * (z + 1) + x);
			i.push_back(countX * (z + 1) + (x + 1));
		}
	}

	indexCount = i.size();
	indices = new UINT[indexCount];

	copy(i.begin(), i.end(), stdext::checked_array_iterator<UINT*>(indices, indexCount));
}
