#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CKurama final : public CGameObject
{
private:
	enum Anim
	{
		Anim_Attack_DoubleHandScratch,
		Anim_Attack_DoubleLowTurnKick,
		Anim_Attack_DoubleSledgehammer,
		Anim_Attack_DoubleTurnKick,
		Anim_Attack_HugeBite,
		Anim_Attack_HugeHandScratch,
		Anim_Attack_JumpCrush,
		Anim_Attack_KickUp,
		Anim_Attack_MowDown_Left,
		Anim_Attack_MowDown_Right,
		Anim_Attack_SwingingDownScratch_Right,
		Anim_Attack_UppercutScratch_Left,
		Anim_Beaten_ChanceTimeHit_Left,
		Anim_Beaten_ChanceTimeHit_Right,
		Anim_Beaten_Down,
		Anim_Beaten_Down_Loop,
		Anim_Beaten_DownHit_Left,
		Anim_Beaten_DownHit_Right,
		Anim_Beaten_Type01,
		Anim_Beaten_Type02,
		Anim_Dying_ChanceTime,
		Anim_Dying_Down,
		Anim_Dying_Type01,
		Anim_Dying_Type01_Loop,
		Anim_etc_Appearance,
		Anim_etc_ChanceTime_End,
		Anim_etc_ChanceTime_Loop,
		Anim_etc_ChanceTime_Start,
		Anim_etc_Leave,
		Anim_etc_Suggest,
		Anim_etc_Suggest_Large,
		Anim_Fall_Front_Loop,
		Anim_GetUp,
		Anim_HandSeal_RecoveryChakra_End,
		Anim_HandSeal_RecoveryChakra_Loop,
		Anim_HandSeal_RecoveryChakra_Start,
		Anim_Idle_Loop,
		Anim_Idle_TurnLeft180,
		Anim_Idle_TurnRight180,
		Anim_Jump_Front,
		Anim_Land,
		Anim_Ninjutsu_Aerial_FakeFoxBlast,
		Anim_Ninjutsu_Aerial_TailedBeastBomb,
		Anim_Ninjutsu_ConsecutiveFakeFoxBlast,
		Anim_Ninjutsu_ConsecutiveTailedBeastBomb,
		Anim_Ninjutsu_FakeFoxBlast,
		Anim_Ninjutsu_FakeFoxBlast_Charge,
		Anim_Ninjutsu_FakeFoxBlast_Charge_Loop,
		Anim_Ninjutsu_Roar,
		Anim_Ninjutsu_TailedBeastBomb,
		Anim_Run_End,
		Anim_Run_Loop,
	};

	enum State_Char
	{
		State_None,
		State_Initiation,
		State_Idle,
		State_LookAt,
		State_Attack,
		State_ComboAttack,
		State_ComboAttack2,
		State_Bomb,
		State_MiniBomb,
		State_Blast,
		State_MiniBlast,
		State_Roar,
		State_Warp,
		State_Beaten,
		State_Die,
	};

private:
	CKurama(_dev pDevice, _context pContext);
	CKurama(const CKurama& rhs);
	virtual ~CKurama() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_Damage(_int iDamage, _uint iDamageType) override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	CCollider* m_pCollider_Hit{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

	CGameObject* m_pEffect{ nullptr };

private:
	_float3 m_vAppearPoints[5]{};

	class CCommonTrail* m_pEyeLights[2]{};
	class CCommonTrail* m_pFingerLights[10]{};

	ANIM_DESC m_AnimationDesc{};

	_float3 m_vOriginalLook{};

	_uint m_iPosIndex{};
	_bool m_hasInitiated{};
	_uint m_iSuperArmor{};

	State_Char m_eState{};
	State_Char m_ePrevState{};
	
	_float m_fTimer{};
	_bool m_hasShot{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	void Artificial_Intelligence(_float fTimeDelta);

	void Init_State();
	void Tick_State(_float fTimeDelta);

public:
	static CKurama* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END