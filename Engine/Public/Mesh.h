#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(_dev pDevice, _context pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	const _uint& Get_MatIndex() const { return m_iMatIndex; }

public:
	HRESULT Init_Prototype(ifstream& ModelFile, ModelType eType, _fmatrix OffsetMatrix);
	HRESULT Init(void* pArg) override;

public:
	HRESULT Ready_StaticMesh(ifstream& ModelFile, _fmatrix OffsetMatrix);
	HRESULT Ready_AnimMesh(ifstream& ModelFile);
	_bool Intersect_RayMesh(_fmatrix WorldMatrix, _float4* pPickPos);

private:
	_float3* m_pVerticesPos{ nullptr };
	_float3* m_pVerticesNor{ nullptr };
	_ulong* m_pIndices{ nullptr };

	_char* m_pName{};
	_uint m_iMatIndex{};

public:
	static CMesh* Create(_dev pDevice, _context pContext, ifstream& ModelFile, ModelType eType, _fmatrix OffsetMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END