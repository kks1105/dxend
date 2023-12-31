#pragma once

class MeshCylinder : public Mesh
{
public:
	MeshCylinder( float radius, float height, UINT stackCount, UINT sliceCount);
	MeshCylinder( float topRadius, float bottomRadius, float height, UINT stackCount, UINT sliceCount);
	~MeshCylinder();

private:
	void Create() override;

private:
	void BuildTopCap(vector<VertexMesh>& vertices, vector<UINT>& indices);
	void BuildBottomCap(vector<VertexMesh>& vertices, vector<UINT>& indices);

private:
	float topRadius;
	float bottomRadius;
	float height;


	UINT stackCount;
	UINT sliceCount;
};
