#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class CStatic_Mesh final : public CVIBuffer
{
private:
	CStatic_Mesh(_dev pDevice, _context pContext);
	CStatic_Mesh(const CStatic_Mesh& rhs);
	virtual ~CStatic_Mesh() = default;

public:
	const _uint& Get_MatIndex() const { return m_iMatIndex; }

public:
	HRESULT Init_Prototype(const string& strFilePath, streampos* iFilePos, _fmatrix OffsetMatrix);
	HRESULT Init(void* pArg) override;

public:
	_float3 Intersect_RayMesh(_fmatrix WorldMatrix);

private:
	_float3* m_pVerticesPos{ nullptr };
	_uint* m_pIndices{ nullptr };

	_char* m_pName{};
	_uint m_iMatIndex{};

public:
	static CStatic_Mesh* Create(_dev pDevice, _context pContext, const string& strFilePath, streampos* iFilePos, _fmatrix OffsetMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END