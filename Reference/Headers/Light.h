#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
	CLight();
	virtual ~CLight() = default;

public:
	const LIGHT_DESC* Get_LightDesc() const;

public:
	HRESULT Init(const LIGHT_DESC& LightDeesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	LIGHT_DESC m_LightDesc{};

public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

END