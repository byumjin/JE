#ifndef SHADERSTRUCTURE_H
#define SHADERSTRUCTURE_H

#include "../Common/D3DUtil.h"
#include "../Common/Effect/Effects.h"

#define MAX_VERTEX 1000000
#define MAX_UV_SET 4

struct Vertex
{
	Vertex::Vertex()
	{
		Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		Color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		//TexCoord = NULL; //XMFLOAT2(2.0f, 2.0f);
		Normal = TangentU = BiNormalV = Position;
		SmoothingID = 0;
		OriginalIndex = 0;
		UVSetCount = 0;
	}


	Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT3& b, const XMFLOAT4& c, const XMFLOAT2& uv, const UINT& smoothingID)
		: Position(p), Normal(n), TangentU(t), BiNormalV(b), Color(c), SmoothingID(smoothingID)
	{		
		/*
		else
		{
		TexCoord[0] = XMFLOAT2(u, v);
		}
		*/
	
	}
	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float bx, float by, float bz,
		float cr, float cg, float cb, float ca,
		float u, float v,
		UINT smoothingID)
		: Position(px, py, pz), Normal(nx, ny, nz),
		TangentU(tx, ty, tz), BiNormalV(bx, by, bz),
		Color(cr, cg, cb, ca),
		SmoothingID(smoothingID)
		{
			
			/*
			else
			{
				TexCoord[0] = XMFLOAT2(u, v);
			}
			*/
		}

	
	XMFLOAT3 Position;	
	XMFLOAT4 Color;
	XMFLOAT2 TexCoord;
	XMFLOAT3 Normal;
	XMFLOAT3 TangentU;
	XMFLOAT3 BiNormalV;
	UINT SmoothingID;
	UINT OriginalIndex;
	UINT UVSetCount;
	XMFLOAT2 TexCoordSet[MAX_UV_SET];

};

struct Triangle
{
	unsigned short index[3];
	UINT PolygonIndex;
};

struct MeshData
{
	std::vector<Vertex> Vertices;
	std::vector<UINT> Indices;
};

class InputLayoutDesc
{
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC VertexInputDesc[6];
	static const D3D11_INPUT_ELEMENT_DESC BRDFL_VID[3];
	static const D3D11_INPUT_ELEMENT_DESC Line_VID[1];
	static const D3D11_INPUT_ELEMENT_DESC GIZMO_VID[4];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device, Effects* pEffects);
	static void DestroyAll();

	static ID3D11InputLayout* VertexInputDesc;
	static ID3D11InputLayout* BRDFL_VID;
	static ID3D11InputLayout* Line_VID;
	static ID3D11InputLayout* GIZMO_VID;
};

#endif