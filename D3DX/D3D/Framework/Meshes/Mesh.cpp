#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh()
{
	
}

Mesh::~Mesh()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	SafeDelete(perFrame);

}

void Mesh::Update()
{
	perFrame->Update();


}

void Mesh::Render(UINT instanceCount)
{
	if (vertexBuffer == nullptr || indexBuffer == nullptr)
	{
		Create();

		vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexMesh));
		indexBuffer = new IndexBuffer(indices, indexCount);
	}

	perFrame->Render();
	vertexBuffer->Render();
	indexBuffer->Render();

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->DrawIndexedInstanced(0, pass, indexCount,instanceCount);// instanceCount->그려질개수
}

void Mesh::SetShader(Shader* shader)
{
	this->shader = shader;

	SafeDelete(perFrame);
	perFrame = new PerFrame(shader);

}
