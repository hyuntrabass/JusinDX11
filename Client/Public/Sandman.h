#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSandman final : public CGameObject
{
private:
	enum Animation
	{
		Aerial_Dash_End,
		Aerial_Dash_Loop,
		Aerial_Dash_Start,
		AimWalk_Behind_End,
		AimWalk_Behind_Loop,
		AimWalk_Behind_Start,
		AimWalk_Front_End,
		AimWalk_Front_Loop,
		AimWalk_Front_Start,
		AimWalk_Left_End,
		AimWalk_Left_Loop,
		AimWalk_Left_Start,
		AimWalk_Right_End,
		AimWalk_Right_Loop,
		AimWalk_Right_Start,
		Attack_Kunai_MoonKick_Middle,
		Attack_Kunai_SlashingHorizontally_Left,
		Attack_Kunai_SmallJumpSlashingShoulder_Left,
		Attack_SlashingHorizontally,
		Attack_SlashingShoulder,
		Attack_StepSlashingHorizontally,
		Beaten_Aerial_BlowOff_toFall,
		Beaten_Aerial_Conect_toFall,
		Beaten_Aerial_D51NJ3_Loop,
		Beaten_Aerial_D51NJ3_Start,
		Beaten_Aerial_DownToFloor_Conect_toStand,
		Beaten_Aerial_Large_Front_Conect_toFall,
		Beaten_Aerial_Small_Conect_toFall,
		Beaten_Aerial_Stagger_Belly,
		Beaten_Aerial_UniversalPull_Behind_End,
		Beaten_Aerial_UniversalPull_Loop,
		Beaten_Aerial_UniversalPull_Start,
		Beaten_BellyDown_Large_End,
		Beaten_BellyDown_Large_Loop,
		Beaten_BellyDown_Large_Start,
		Beaten_BellyDown_Middle_End,
		Beaten_BellyDown_Middle_Loop,
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
		Beaten_HideBlowOff,
		Beaten_Kamui_Loop,
		Beaten_Kunai,
		Beaten_Left,
		Beaten_Left_Head,
		Beaten_Left_Low,
		Beaten_LowFront_Chin,
		Beaten_OneHitDown_Long,
		Beaten_OneHitDown_Short,
		Beaten_Paralysis,
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
		DashStep_Behind,
		DashStep_Front,
		DashStep_Left,
		DashStep_Right,
		Dodge_Run_Left,
		Dodge_Run_Right,
		Dying_Aerial_Fall_Behind_Loop,
		Dying_Blasted,
		Dying_Pose_Type01,
		Dying_Pose_Type02,
		Dying_Type01,
		etc_Appearance_Type01,
		etc_Appearance_Type02,
		etc_CallHelp,
		etc_Exit,
		Fall_Front_Loop,
		Fall_Vertical_Loop,
		GetUp,
		GetUp_Type01,
		Guard_GuardBreak_End,
		Guard_GuardBreak_Loop,
		Guard_GuardBreak_Start,
		Guard_GuardHit_Front,
		Guard_GuardReverse,
		Guard_Loop,
		Idle_Type01_Loop,
		Idle_Type02_Loop,
		Idle_Type03_Loop,
		Land,
		Ninjutsu_Healing_End,
		Ninjutsu_Healing_Loop,
		Ninjutsu_Healing_Start,
		Ninjutsu_MudWall,
		Ninjutsu_SandShower,
		Ninjutsu_WindBomb,
		Rolling_GetUp,
		Run_End,
		Run_Loop,
		Throw_Aerial_Kunai,
		Throw_Aerial_Kunai_Left,
		Throw_Aerial_Kunai_Right,
		Throw_Kunai,
		Walk_End,
		Walk_Loop,
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
	_bool m_isRunning{};
	_float m_fSliding{ 1.f };

private:
	void Move(_float fTimeDelta);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSandman* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END