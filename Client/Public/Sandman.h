#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSandman final : public CGameObject
{
private:
	enum Animation
	{
		Anim_Aerial_Dash_End,
		Anim_Aerial_Dash_Loop,
		Anim_Aerial_Dash_Start,
		Anim_AimWalk_Behind_End,
		Anim_AimWalk_Behind_Loop,
		Anim_AimWalk_Behind_Start,
		Anim_AimWalk_Front_End,
		Anim_AimWalk_Front_Loop,
		Anim_AimWalk_Front_Start,
		Anim_AimWalk_Left_End,
		Anim_AimWalk_Left_Loop,
		Anim_AimWalk_Left_Start,
		Anim_AimWalk_Right_End,
		Anim_AimWalk_Right_Loop,
		Anim_AimWalk_Right_Start,
		Anim_Attack_DownStrike,
		Anim_Attack_Kick,
		Anim_Attack_Punch_Left,
		Anim_Attack_Punch_Right,
		Anim_Attack_Rush,
		Anim_Attack_SideDoubleSlashing,
		Anim_Attack_TurnSlashing,
		Anim_Beaten_Aerial_BlowOff_toFall,
		Anim_Beaten_Aerial_Conect_toFall,
		Anim_Beaten_Aerial_D51NJ3_Loop,
		Anim_Beaten_Aerial_D51NJ3_Start,
		Anim_Beaten_Aerial_DownToFloor_Conect_toStand,
		Anim_Beaten_Aerial_Large_Front_Conect_toFall,
		Anim_Beaten_Aerial_Small_Conect_toFall,
		Anim_Beaten_Aerial_Stagger_Belly,
		Anim_Beaten_Aerial_UniversalPull_Behind_End,
		Anim_Beaten_Aerial_UniversalPull_Front_End,
		Anim_Beaten_Aerial_UniversalPull_Loop,
		Anim_Beaten_Aerial_UniversalPull_Start,
		Anim_Beaten_BellyDown_Large_End,
		Anim_Beaten_BellyDown_Large_Loop,
		Anim_Beaten_BellyDown_Large_Start,
		Anim_Beaten_BellyDown_Middle_End,
		Anim_Beaten_BellyDown_Middle_Loop,
		Anim_Beaten_BellyDown_Middle_Start,
		Anim_Beaten_BellyDown_Small_End,
		Anim_Beaten_BellyDown_Small_Loop,
		Anim_Beaten_BellyDown_Small_Start,
		Anim_Beaten_Bind_Loop,
		Anim_Beaten_Blasted,
		Anim_Beaten_BlowOff_ChaseAttack,
		Anim_Beaten_Burn_Type01,
		Anim_Beaten_D45NJ3_End,
		Anim_Beaten_D45NJ3_Loop,
		Anim_Beaten_D45NJ3_Start,
		Anim_Beaten_D51NJ3_Loop,
		Anim_Beaten_D51NJ3_Start,
		Anim_Beaten_D55NJ2,
		Anim_Beaten_Down_Loop,
		Anim_Beaten_Down_Sleep_Type01_Loop,
		Anim_Beaten_Down_Type01_Loop,
		Anim_Beaten_DownHit,
		Anim_Beaten_DownToFloor,
		Anim_Beaten_DownToFloor_Type01,
		Anim_Beaten_DownToWall,
		Anim_Beaten_Earthquake_Loop,
		Anim_Beaten_ElectricShock_End,
		Anim_Beaten_ElectricShock_Loop,
		Anim_Beaten_Fall_Behind_Loop,
		Anim_Beaten_Fall_ChaseAttack_Behind_Loop,
		Anim_Beaten_Fall_ChaseAttack_Loop,
		Anim_Beaten_Fall_Front_Loop,
		Anim_Beaten_Frozen_Loop,
		Anim_Beaten_GuardReverseBlowOff,
		Anim_Beaten_Hide_Loop,
		Anim_Beaten_HideBlowOff,
		Anim_Beaten_Kamui_Loop,
		Anim_Beaten_Kunai,
		Anim_Beaten_Left,
		Anim_Beaten_Left_Head,
		Anim_Beaten_Left_Low,
		Anim_Beaten_LowFront_Chin,
		Anim_Beaten_OneHitDown_Long,
		Anim_Beaten_OneHitDown_Short,
		Anim_Beaten_Paralysis,
		Anim_Beaten_Restrict_Loop,
		Anim_Beaten_Right,
		Anim_Beaten_ShadowPossession_Loop,
		Anim_Beaten_SpinBlowOff,
		Anim_Beaten_SpinBlowOff_Behind,
		Anim_Beaten_SpinBlowOff_Bound_Conect_toFall,
		Anim_Beaten_SpinBlowOff_Bound_End,
		Anim_Beaten_SpinBlowOff_Bound_Loop,
		Anim_Beaten_SpinBlowOff_Bound_Start,
		Anim_Beaten_SpinBlowOff_Conect_toDown,
		Anim_Beaten_SpinBlowOff_Conect_toStand,
		Anim_Beaten_SpinBlowOff_D52NJ3,
		Anim_Beaten_SpinBlowOff_Large_Up,
		Anim_Beaten_Stagger_Belly,
		Anim_Beaten_Struggle_Loop,
		Anim_Beaten_Struggle_Start,
		Anim_Beaten_UpFront_Head,
		Anim_Beaten_UpLeft_Shoulder,
		Anim_DashStep_Far_Left,
		Anim_DashStep_Far_Right,
		Anim_DashStep_Left,
		Anim_DashStep_Right,
		Anim_Dying_Aerial_Fall_Behind_Loop,
		Anim_Dying_Blasted,
		Anim_Dying_Type01,
		Anim_etc_Appearance_Type01,
		Anim_etc_Appearance_Type02,
		Anim_etc_Appearance_Type03,
		Anim_etc_Exit,
		Anim_Fall_Front_Loop,
		Anim_Fall_Vertical_Loop,
		Anim_GetUp,
		Anim_GetUp_Type01,
		Anim_Guard_GuardBreak_End,
		Anim_Guard_GuardBreak_Loop,
		Anim_Guard_GuardBreak_Start,
		Anim_Guard_GuardHit_Front,
		Anim_Guard_GuardReverse,
		Anim_Guard_Loop,
		Anim_HandSeal_RecoveryChakra_Loop,
		Anim_HandSeal_RecoveryChakra_Start,
		Anim_Idle_Type02_Loop,
		Anim_Land,
		Anim_RangeAttack_SlashingShoulder,
		Anim_Rolling_GetUp,
		Anim_Run_End,
		Anim_Run_Loop,
		Anim_Throw_Aerial_Kunai,
		Anim_Throw_Aerial_Kunai_Left,
		Anim_Throw_Aerial_Kunai_Right,
		Anim_Throw_Kunai,
		Anim_Walk_End,
		Anim_Walk_Loop,
		End
	};

	enum State_Char
	{
		State_Idle,
		State_Run,
		State_Walk,
		State_Attack,
		State_Die,
	};
private:
	CSandman(_dev pDevice, _context pContext);
	CSandman(const CSandman& rhs);
	virtual ~CSandman() = default;

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

private:
	State_Char m_eCurrState{};
	State_Char m_ePrevState{};

	_float4 m_vOriginPos{};
	_float4 m_vTargetDir{};

	_float m_fWalkSpeed{};
	_float m_fRunSpeed{};
	_float m_fAttackRange{};
	_float m_fSearchRange{};
	_float m_fAttTime{};

	_uint m_iPrevAnimIndex{};
	_bool m_isRunning{};
	_float m_fSliding{ 1.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	void Artificial_Intelligence(_float fTimeDelta);
	void Idle(_float fTimeDelta);
	void Move(_float fTimeDelta);

	void Change_State();

public:
	static CSandman* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END