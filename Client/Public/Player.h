#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
	enum Animation
	{
		Aerial_Dash_End,
		Aerial_Dash_Loop,
		Aerial_Dash_Start,
		Attack_Aerial_DownStrike,
		Attack_Aerial_Hand_Punch,
		Attack_BodyBlow,
		Attack_DoubleTurnKick_Type02,
		Attack_DownStrike,
		Attack_JumpSpinKick,
		Attack_Kunai_SlashingShoulderTurnKick_Left,
		Attack_Kunai_Uppercut,
		Attack_Palm,
		Attack_PalmJumpSpinKick,
		Attack_supa_01,
		Attack_supa_02,
		Attack_TurnKick_Left,
		Beaten_Aerial_BlowOff_toFall,
		Beaten_Aerial_Conect_toFall,
		Beaten_Aerial_D45NJ3_Loop,
		Beaten_Aerial_D45NJ3_Start,
		Beaten_Aerial_D51NJ3_Loop,
		Beaten_Aerial_D51NJ3_Start,
		Beaten_Aerial_DownToFloor_Conect_toStand,
		Beaten_Aerial_Large_Front_Conect_toFall,
		Beaten_Aerial_ReaperDeathSeal,
		Beaten_Aerial_Small_Conect_toFall,
		Beaten_Aerial_Stagger_Belly,
		Beaten_Aerial_UniversalPull_Loop,
		Beaten_Aerial_UniversalPull_Start,
		Beaten_BellyDown_Large_End,
		Beaten_BellyDown_Large_Loop,
		Beaten_BellyDown_Large_Start,
		Beaten_BellyDown_Middle_End,
		Beaten_BellyDown_Middle_Start,
		Beaten_BellyDown_Small_End,
		Beaten_BellyDown_Small_Loop,
		Beaten_BellyDown_Small_Start,
		Beaten_Bind_Loop,
		Beaten_Blasted,
		Beaten_BlowOff_ChaseAttack,
		Beaten_Burn_Type01,
		Beaten_D29NJ1_End,
		Beaten_D29NJ1_Loop,
		Beaten_D29NJ1_LostEnd,
		Beaten_D29NJ1_Start,
		Beaten_D43NJ1,
		Beaten_D45NJ3_End,
		Beaten_D45NJ3_Loop,
		Beaten_D45NJ3_Start,
		Beaten_D51NJ3_Loop,
		Beaten_D51NJ3_Start,
		Beaten_D55NJ2,
		Beaten_Down_Loop,
		Beaten_Down_Sleep_Type01_Loop,
		Beaten_Down_Type01_Loop,
		Beaten_DownHit,
		Beaten_DownToFloor,
		Beaten_DownToFloor_Type01,
		Beaten_DownToWall,
		Beaten_Earthquake_Loop,
		Beaten_ElectricShock_End,
		Beaten_ElectricShock_Loop,
		Beaten_Fall_Behind_Loop,
		Beaten_Fall_ChaseAttack_Behind_Loop,
		Beaten_Fall_ChaseAttack_Loop,
		Beaten_Fall_Front_Loop,
		Beaten_Frozen_Loop,
		Beaten_GuardReverseBlowOff,
		Beaten_Hide_Loop,
		Beaten_Kamui_Loop,
		Beaten_Kunai,
		Beaten_Left,
		Beaten_Left_Head,
		Beaten_Left_Low,
		Beaten_LowFront_Chin,
		Beaten_OneHitDown_Long,
		Beaten_OneHitDown_Short,
		Beaten_Paralysis,
		Beaten_ReaperDeathSeal,
		Beaten_Restrict_Loop,
		Beaten_Right,
		Beaten_ShadowPossession_Loop,
		Beaten_SpinBlowOff,
		Beaten_SpinBlowOff_Behind,
		Beaten_SpinBlowOff_Bound_Conect_toFall,
		Beaten_SpinBlowOff_Bound_End,
		Beaten_SpinBlowOff_Bound_Loop,
		Beaten_SpinBlowOff_Bound_Start,
		Beaten_SpinBlowOff_Conect_toDown,
		Beaten_SpinBlowOff_Conect_toStand,
		Beaten_SpinBlowOff_D52NJ3,
		Beaten_SpinBlowOff_Large_Up,
		Beaten_Stagger_Belly,
		Beaten_Struggle_Loop,
		Beaten_Struggle_Start,
		Beaten_UpFront_Head,
		Beaten_UpLeft_Shoulder,
		ChakraJump_Charge_End,
		ChakraJump_Charge_Loop,
		ChakraJump_Charge_Start,
		ChakraJump_Vertical_Charge_End,
		ChakraJump_Vertical_Charge_Loop,
		ChakraJump_Vertical_Charge_Start,
		Dodge_Behind,
		Dodge_Front,
		Dodge_Left,
		Dodge_Left_Type01,
		Dodge_Right,
		Dodge_Right_Type01,
		DoubleJump,
		Dying_Aerial_Fall_Behind_Loop,
		Dying_Blasted,
		Dying_Type01,
		etc_Appearance,
		etc_Exit,
		etc_Win_Type01_Loop,
		Fall_Front_Loop,
		Fall_Vertical_Loop,
		GetUp,
		GetUp_Type01,
		Guard_Aerial_GuardHit_Front,
		Guard_Aerial_GuardReverse,
		Guard_Aerial_Loop,
		Guard_GuardBreak_End,
		Guard_GuardBreak_Loop,
		Guard_GuardBreak_Start,
		Guard_GuardHit_Front,
		Guard_GuardReverse,
		Guard_Loop,
		HeightLand,
		Idle_Loop,
		Jump_Front,
		Jump_Vertical,
		Land,
		Land_toRecoveryChakra,
		Ninjutsu_Aerial_AlmightyPush_Start,
		Ninjutsu_Aerial_PlanetaryDevastation_Start,
		Ninjutsu_Aerial_UniversalPull,
		Ninjutsu_AlmightyPush_Start,
		Ninjutsu_PlanetaryDevastation_Attack,
		Ninjutsu_PlanetaryDevastation_End,
		Ninjutsu_PlanetaryDevastation_Loop,
		Ninjutsu_UniversalPull,
		Rolling_GetUp,
		Run_End,
		Run_Loop,
		Run_TurnLeft180,
		Run_TurnLeft180_Start,
		Run_TurnLeft90_Start,
		Run_TurnRight180,
		Run_TurnRight180_Start,
		Run_TurnRight90_Start,
		Throw_Aerial_Kunai,
		Throw_Aerial_Kunai_Left,
		Throw_Aerial_Kunai_Right,
		Throw_Kunai,
		Throw_Kunai_JumpTwist,
		Walk_End,
		Walk_Loop,
		Walk_TurnLeft180_Start,
		Walk_TurnLeft90_Start,
		Walk_TurnRight180_Start,
		Walk_TurnRight90_Start,
		WallJump_Loop,
		WallStop,
		WireJump_Ready,
		End
	};

private:
	CPlayer(_dev pDevice, _context pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

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
	_bool m_isRunning{};
	_float m_fSliding{ 1.f };

private:
	void Move(_float fTimeDelta);
	void Customize(_float fTimeDelta);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END