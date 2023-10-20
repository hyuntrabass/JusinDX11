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
	HRESULT Init_Prototype(const string& strFilePath);
	HRESULT Init(void* pArg) override;

private:
	const _char* m_pName{};

public:
	static CStatic_Mesh* Create(_dev pDevice, _context pContext, const string& strFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END