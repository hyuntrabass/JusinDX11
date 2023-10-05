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
	virtual HRESULT Init_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Init(void* pArg);

public:
	HRESULT Begin(_uint iPassIndex);

private:
	ID3DX11Effect* m_pEffect{ nullptr };
	_uint m_iNumPasses{};
	vector<ID3D11InputLayout*> m_InputLayouts{};

public:
	static CShader* Create(_dev pDevice, _context pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END