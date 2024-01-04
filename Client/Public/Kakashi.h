#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CKakashi final : public CGameObject
{
private:
	enum Animation
	{
		Kakashi_etc_Appearance,
		Kakashi_etc_Exit,
		Kakashi_etc_Tutorial_Loop,
		RecordedSequence_NN_Pawn_Player_Kakashi,
		RecordedSequence_NN_Pawn_Player_Kakashi_1,
		RecordedSequence_NN_Pawn_Player_Kakashi_7,
	};
private:
	CKakashi(_dev pDevice, _context pContext);
	CKakashi(const CKakashi& rhs);
	virtual ~CKakashi() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	class CUI_Manager* m_pUIManager{ nullptr };
	class CTrigger_Manager* m_pTriggerManager{ nullptr };

private:
	_float m_fTimer{};
	_uint m_iSceneNumber{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CKakashi* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END