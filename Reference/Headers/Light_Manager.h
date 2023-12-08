#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	LIGHT_DESC* Get_LightDesc(_uint iLevelIndex, _uint iIndex);

public:
	HRESULT Init(_uint iNumLevels);
	HRESULT Add_Light(_uint iLevelIndex, const LIGHT_DESC& LightDesc);
	void Clear(_uint iLevelIndex);
	HRESULT Render(_uint iLevelIndex, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	_uint m_iNumLevels{};
	list<class CLight*>* m_pLights{};

public:
	static CLight_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END