#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CPain final : public CGameObject
{
private:
	enum Animation
	{
		Anim_Aerial_Dash_End,
		Anim_Aerial_Dash_Loop,
		Anim_Aerial_Dash_Start,
		Anim_Aerial_RecoveryChakra_End,
		Anim_Aerial_RecoveryChakra_Loop,
		Anim_Aerial_RecoveryChakra_Start,
		Anim_Attack_Aerial_DownStrike,
		Anim_Attack_Aerial_Hand_Punch,
		Anim_Attack_BodyBlow,
		Anim_Attack_DoubleTurnKick_Type02,
		Anim_Attack_DownStrike,
		Anim_Attack_JumpSpinKick,
		Anim_Attack_Kunai_SlashingShoulderTurnKick_Left,
		Anim_Attack_Kunai_Uppercut,
		Anim_Attack_Palm,
		Anim_Attack_PalmJumpSpinKick,
		Anim_Attack_supa_01,
		Anim_Attack_supa_02,
		Anim_Attack_TurnKick_Left,
		Anim_Beaten_Aerial_BlowOff_toFall,
		Anim_Beaten_Aerial_Conect_toFall,
		Anim_Beaten_Aerial_D45NJ3_Loop,
		Anim_Beaten_Aerial_D45NJ3_Start,
		Anim_Beaten_Aerial_D51NJ3_Loop,
		Anim_Beaten_Aerial_D51NJ3_Start,
		Anim_Beaten_Aerial_DownToFloor_Conect_toStand,
		Anim_Beaten_Aerial_Large_Front_Conect_toFall,
		Anim_Beaten_Aerial_ReaperDeathSeal,
		Anim_Beaten_Aerial_Small_Conect_toFall,
		Anim_Beaten_Aerial_Stagger_Belly,
		Anim_Beaten_Aerial_UniversalPull_Loop,
		Anim_Beaten_Aerial_UniversalPull_Start,
		Anim_Beaten_BellyDown_Large_End,
		Anim_Beaten_BellyDown_Large_Loop,
		Anim_Beaten_BellyDown_Large_Start,
		Anim_Beaten_BellyDown_Middle_End,
		Anim_Beaten_BellyDown_Middle_Start,
		Anim_Beaten_BellyDown_Small_End,
		Anim_Beaten_BellyDown_Small_Loop,
		Anim_Beaten_BellyDown_Small_Start,
		Anim_Beaten_Bind_Loop,
		Anim_Beaten_Blasted,
		Anim_Beaten_BlowOff_ChaseAttack,
		Anim_Beaten_Burn_Type01,
		Anim_Beaten_D29NJ1_End,
		Anim_Beaten_D29NJ1_Loop,
		Anim_Beaten_D29NJ1_LostEnd,
		Anim_Beaten_D29NJ1_Start,
		Anim_Beaten_D43NJ1,
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
		Anim_Beaten_Kamui_Loop,
		Anim_Beaten_Kunai,
		Anim_Beaten_Left,
		Anim_Beaten_Left_Head,
		Anim_Beaten_Left_Low,
		Anim_Beaten_LowFront_Chin,
		Anim_Beaten_OneHitDown_Long,
		Anim_Beaten_OneHitDown_Short,
		Anim_Beaten_Paralysis,
		Anim_Beaten_ReaperDeathSeal,
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
		Anim_ChakraJump_Charge_End,
		Anim_ChakraJump_Charge_Loop,
		Anim_ChakraJump_Charge_Start,
		Anim_ChakraJump_Vertical_Charge_End,
		Anim_ChakraJump_Vertical_Charge_Loop,
		Anim_ChakraJump_Vertical_Charge_Start,
		Anim_Dodge_Behind,
		Anim_Dodge_Front,
		Anim_Dodge_Left,
		Anim_Dodge_Left_Type01,
		Anim_Dodge_Right,
		Anim_Dodge_Right_Type01,
		Anim_DoubleJump,
		Anim_Dying_Aerial_Fall_Behind_Loop,
		Anim_Dying_Blasted,
		Anim_Dying_Type01,
		Anim_etc_Appearance,
		Anim_etc_Exit,
		Anim_etc_Win_Type01_Loop,
		Anim_Fall_Front_Loop,
		Anim_Fall_Vertical_Loop,
		Anim_GetUp,
		Anim_GetUp_Type01,
		Anim_Guard_Aerial_GuardHit_Front,
		Anim_Guard_Aerial_GuardReverse,
		Anim_Guard_Aerial_Loop,
		Anim_Guard_GuardBreak_End,
		Anim_Guard_GuardBreak_Loop,
		Anim_Guard_GuardBreak_Start,
		Anim_Guard_GuardHit_Front,
		Anim_Guard_GuardReverse,
		Anim_Guard_Loop,
		Anim_HandSeal_RecoveryChakra_End,
		Anim_HandSeal_RecoveryChakra_Loop,
		Anim_HandSeal_RecoveryChakra_Start,
		Anim_HeightLand,
		Anim_Idle_Loop,
		Anim_Idle_Loop001,
		Anim_Jump_Front,
		Anim_Jump_Vertical,
		Anim_Land,
		Anim_Land001,
		Anim_Land_toRecoveryChakra,
		Anim_Lobby_GetSet_Loop,
		Anim_Lobby_Win_Loop,
		Anim_Lobby_Win_Loop001,
		Anim_Ninjutsu_Aerial_AlmightyPush_Start,
		Anim_Ninjutsu_Aerial_PlanetaryDevastation_Start,
		Anim_Ninjutsu_Aerial_UniversalPull,
		Anim_Ninjutsu_AlmightyPush_Start,
		Anim_Ninjutsu_PlanetaryDevastation_Attack,
		Anim_Ninjutsu_PlanetaryDevastation_End,
		Anim_Ninjutsu_PlanetaryDevastation_Loop,
		Anim_Ninjutsu_UniversalPull,
		Anim_RangeAttack_Punch,
		Anim_RecoveryChakra_End,
		Anim_RecoveryChakra_Loop,
		Anim_RecoveryChakra_Start,
		Anim_Rolling_GetUp,
		Anim_Run_End,
		Anim_Run_Loop,
		Anim_Run_TurnLeft180,
		Anim_Run_TurnLeft180_Start,
		Anim_Run_TurnLeft90_Start,
		Anim_Run_TurnRight180,
		Anim_Run_TurnRight180_Start,
		Anim_Run_TurnRight90_Start,
		Anim_Throw_Aerial_Kunai,
		Anim_Throw_Aerial_Kunai_Left,
		Anim_Throw_Aerial_Kunai_Right,
		Anim_Throw_Kunai,
		Anim_Throw_Kunai_JumpTwist,
		Anim_Walk_End,
		Anim_Walk_Loop,
		Anim_Walk_TurnLeft180_Start,
		Anim_Walk_TurnLeft90_Start,
		Anim_Walk_TurnRight180_Start,
		Anim_Walk_TurnRight90_Start,
		Anim_WallJump_Loop,
		Anim_WallStop,
		Anim_WireJump_Ready,
		Anim_End
	};

	enum State_Char
	{
		State_None,
		State_Initiation,
		State_Idle,
		State_LookAt,
		State_ComboAttack,
		State_Push,
		State_Pull,
		State_Sommon,
		State_Beaten,
		State_Die,
	};
private:
	CPain(_dev pDevice, _context pContext);
	CPain(const CPain& rhs);
	virtual ~CPain() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float fTimeDelta) override;
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;
	virtual void Set_Damage(_int iDamage) override;
private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	CCollider* m_pCollider_Hit{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

private:
	ANIM_DESC m_AnimationDesc{};

	_float3 m_vOriginalLook{};

	_bool m_hasInitiated{};

	State_Char m_eState{};
	State_Char m_ePrevState{};

	_float m_fTimer{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	void Artificial_Intelligence(_float fTimeDelta);

	void Init_State();
	void TIck_State(_float fTimeDelta);

public:
	static CPain* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END