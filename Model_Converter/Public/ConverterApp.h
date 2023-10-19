#pragma once
#include "ModelConverter_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(ModelConverter)

class CConverterApp final : public CBase
{
private:
	CConverterApp();
	virtual ~CConverterApp() = default;

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

private:
	class CImguiMgr* m_pImguiMgr{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };
	CRenderer* m_pRenderer{ nullptr };
	_float m_fTimeAcc{};
	_uint m_iFrameCount{};

private:
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Prototype_GameObject();

public:
	static CConverterApp* Create();
	virtual void Free() override;
};

END