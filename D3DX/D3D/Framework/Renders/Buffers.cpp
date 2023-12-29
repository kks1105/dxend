#include "Framework.h"
#include "Buffers.h"

//-----------------------------------------------------------------------------
// VertexBuffer
//-----------------------------------------------------------------------------
VertexBuffer::VertexBuffer(void* data, UINT count, UINT stride, UINT slot, bool bCpuWrite, bool bGpuWrite)
	: data(data)
	, count(count)
	, stride(stride)
	, slot(slot)
	, bCpuWrite(bCpuWrite)
	, bGpuWrite(bGpuWrite)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = stride * count;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (bCpuWrite == false && bGpuWrite == false)
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
	}
	else if (bCpuWrite == true && bGpuWrite == false)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (bCpuWrite == false && bGpuWrite == true)
	{
		desc.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = data;
	
	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &buffer));
}

VertexBuffer::~VertexBuffer()
{
	SafeRelease(buffer);
}

void VertexBuffer::Render()
{
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
}

//-----------------------------------------------------------------------------
// IndexBuffer
//-----------------------------------------------------------------------------
IndexBuffer::IndexBuffer(void* data, UINT count)
	: data(data)
	, count(count)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(UINT) * count;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = data;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &buffer));
}

IndexBuffer::~IndexBuffer()
{
	SafeRelease(buffer);
}

void IndexBuffer::Render()
{
	D3D::GetDC()->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}

//-----------------------------------------------------------------------------
// ConstantBuffer
//-----------------------------------------------------------------------------
ConstantBuffer::ConstantBuffer(void* data, UINT dataSize)
	: data(data)
	, dataSize(dataSize)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Check(D3D::GetDevice()->CreateBuffer(&desc, nullptr, &buffer));
}

ConstantBuffer::~ConstantBuffer()
{
	SafeRelease(buffer);
}

void ConstantBuffer::Render()
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, data, dataSize);
	}
	D3D::GetDC()->Unmap(buffer, 0);
}

//---------------------------------------
//Compute Buffer(Super)
//---------------------

ComputeBuffer::~ComputeBuffer()
{
	SafeRelease(input);
	SafeRelease(srv);

	SafeRelease(output);
	SafeRelease(uav);

}

void ComputeBuffer::CreateBuffer()
{
	CreateInput();
	CreateSRV();

	CreateOutput();
	CreateUAV();


}


//---------------------------------------
//RawBuffer(C++), ByteAddress(HLSL)
//---------------------

RawBuffer::RawBuffer(void* inputData, UINT inputByte, UINT outputByte)
	:inputData(inputData),inputByte(inputByte),outputByte(outputByte)
{
	CreateBuffer();//부모꺼

}

RawBuffer::~RawBuffer()
{
}

void RawBuffer::CreateInput()
{
	if (inputByte < 1) return;

	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = inputByte;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;//srv에 태울수잇는버퍼가됨
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;//컴퓨트인지로우인지결정해줌 여긴로우
	desc.Usage = D3D11_USAGE_DYNAMIC;//다이나믹으로 (권한따로임)
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//쓰기권한주기

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = inputData;

	Check(D3D::GetDevice()->CreateBuffer(&desc, inputData != nullptr ? &subResource : nullptr, &buffer));

	input = buffer;

}

void RawBuffer::CreateSRV()
{
	if (inputByte < 1) return;

	ID3D11Buffer* buffer = (ID3D11Buffer*)input;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;//void* 
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	srvDesc.BufferEx.NumElements = desc.ByteWidth / 4;

	Check(D3D::GetDevice()->CreateShaderResourceView(buffer,&srvDesc,&srv));//inputbuffer srv에태우기 부모의 srv에저장
}

void RawBuffer::CreateOutput()
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = outputByte;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;


	Check(D3D::GetDevice()->CreateBuffer(&desc,nullptr, &buffer));


	output = buffer; //부모의 아웃풋에저장
}

void RawBuffer::CreateUAV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)output;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;//void* 
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	uavDesc.Buffer.NumElements = desc.ByteWidth / 4;

	Check(D3D::GetDevice()->CreateUnorderedAccessView(buffer, &uavDesc, &uav));

	
}

void RawBuffer::CopyToInput(void* data)
{
	//인풋 정보 맵으로
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(input, 0, D3D11_MAP_WRITE_DISCARD,0, &subResource);
	{
		memcpy(subResource.pData, data, inputByte);//서브리소스에있는거 데이터로 인풋

	}
	D3D::GetDC()->Unmap(input, 0);

}

void RawBuffer::CopyFromOutput(void* data)
{

	//아웃풋 정보 맵으로
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(output, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(data,subResource.pData, outputByte); //데이터에있는거 서브리소스로 아웃풋

	}
	D3D::GetDC()->Unmap(output, 0);

}

//---------------------------------------
//textureBuffer
//---------------------

TextureBuffer::TextureBuffer(ID3D11Texture2D* src)
{
	//ID3D11Texture2D 바로안보내는이유 :텍스쳐에포함된 해상도에따른 밉매핑 정보는 쉐이더로 보내면안됨 

	D3D11_TEXTURE2D_DESC srcDesc;
	src->GetDesc(&srcDesc);

	
	width = srcDesc.Width; //텍스쳐원본에있는거
	height = srcDesc.Height;
	arraySize = srcDesc.ArraySize;
	format = srcDesc.Format;

	D3D11_TEXTURE2D_DESC desc; 
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width;//넣을애들
	desc.Height = height;
	desc.ArraySize = arraySize;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;

	ID3D11Texture2D* texture = nullptr;
	Check(D3D::GetDevice()->CreateTexture2D(&desc,nullptr,&texture));
	D3D::GetDC()->CopyResource(texture, src);//텍스쳐에서서브리소스복사해서가져오기

	input = texture;

	CreateBuffer();

	((ID3D11Texture2D*)output)->GetDesc(&desc); //텍스쳐아웃풋 desc
	desc.Usage = D3D11_USAGE_STAGING;//cpu gpu 읽기쓰기 사용가능
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;//cpu는읽기만가능하게 쓰기안주기
	desc.BindFlags = 0; //이넘으로되있는데 0은없어서 선택안함과같음
	desc.MiscFlags = 0;

	Check(D3D::GetDevice()->CreateTexture2D(&desc, nullptr, &result));//아웃풋 에넣을애라고해야하나 만들기

}

TextureBuffer::~TextureBuffer()
{
	SafeRelease(result);

}

void TextureBuffer::CreateSRV()
{
	ID3D11Texture2D* texture = (ID3D11Texture2D*)input;

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = arraySize;

	Check(D3D::GetDevice()->CreateShaderResourceView(texture,&srvDesc,&srv));

}

void TextureBuffer::CreateOutput()
{
	ID3D11Texture2D* texture = nullptr;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = arraySize;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Check(D3D::GetDevice()->CreateTexture2D(&desc, nullptr,&texture));

	output = texture;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1; //최대퀄리티
	srvDesc.Texture2DArray.ArraySize = arraySize;

	Check(D3D::GetDevice()->CreateShaderResourceView(texture,&srvDesc,&outputSRV )); //쉐이더리소스뷰 저장할애 버퍼 저장할곳

}

void TextureBuffer::CreateUAV()
{
	ID3D11Texture2D* texture = (ID3D11Texture2D*)output;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;//받아온거포맷은 지정되잇음 이걸로 
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = arraySize;

	Check(D3D::GetDevice()->CreateUnorderedAccessView(texture, &uavDesc, &uav)); //저장할애 버퍼 저장할곳



}

void TextureBuffer::CopyToInput(ID3D11Texture2D* texture)
{

	//인풋
	D3D::GetDC()->CopyResource(input, texture);

}

ID3D11Texture2D* TextureBuffer::CopyFromOutput()
{
	//아웃풋

	D3D::GetDC()->CopyResource(result, output);

	return result;
}


//-----------------------------------------------------------------------------
//StructuredBuffer
//-----------------------------------------------------------------------------

StructuredBuffer::StructuredBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride, UINT outputCount)
	: inputData(inputData)
	, inputStride(inputStride)
	, inputCount(inputCount)
	, outputStride(outputStride)
	, outputCount(outputCount)
{
	if (outputStride == 0 || outputCount == 0)
	{
		this->outputStride = inputStride;
		this->outputCount = inputCount;
	}

	CreateBuffer();
}

StructuredBuffer::~StructuredBuffer()
{
}

void StructuredBuffer::CreateInput()
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = InputByteWidth();
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; 
	desc.StructureByteStride = inputStride;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = inputData;

	Check(D3D::GetDevice()->CreateBuffer(&desc, inputData != nullptr ? &subResource : nullptr, &buffer));



	input = buffer;


}

void StructuredBuffer::CreateSRV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)input;


	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	desc.Format = DXGI_FORMAT_UNKNOWN; //포맷
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;//버퍼ex사용가능
	desc.BufferEx.NumElements = inputCount;


	Check(D3D::GetDevice()->CreateShaderResourceView(buffer,&desc,&srv));


}

void StructuredBuffer::CreateOutput()
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = OutputByteWidth();
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = outputStride;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;


	Check(D3D::GetDevice()->CreateBuffer(&desc, nullptr, &buffer));

	output = buffer;

}

void StructuredBuffer::CreateUAV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)output;

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.NumElements = outputCount;

	Check(D3D::GetDevice()->CreateUnorderedAccessView(buffer, &desc, &uav));
}

void StructuredBuffer::CopyToInput(void* data)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(input, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, data, InputByteWidth());
	}
	D3D::GetDC()->Unmap(input, 0);
}

void StructuredBuffer::CopyFromOutput(void* data)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(output, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(data, subResource.pData, OutputByteWidth());
	}
	D3D::GetDC()->Unmap(output, 0);
}
