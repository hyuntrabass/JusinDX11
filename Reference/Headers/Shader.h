#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(_dev pDevice, _context pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Init_Prototype(const wstring& strShaderFilePath);
	virtual HRESULT Init(void* pArg);

private:
	ID3DX11Effect* m_pEffect{ nullptr };

public:
	static CShader* Create(_dev pDevice, _context pContext, const wstring& strShaderFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END