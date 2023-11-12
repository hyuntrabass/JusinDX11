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
		Attack_Aerial_BigSword_DownStrike,
		Attack_BigSword_BothHands_DoubleSlashing_Horizontally,
		Attack_BigSword_BothHandsSlashingHorizontally,
		Beaten_Left,
		Beaten_Right,
		BigSword_DashStep_Behind,
		BigSword_DashStep_Front,
		BigSword_Dodge_Run_Left,
		BigSword_Dodge_Run_Right,
		BigSword_HeightLand,
		BigSword_Idle_Loop,
		BigSword_Land,
		BigSword_Run_End,
		BigSword_Walk_End,
		ChakraJump_Charge_End,
		ChakraJump_Charge_Loop,
		ChakraJump_Charge_Start,
		ChakraJump_Vertical_Charge_End,
		ChakraJump_Vertical_Charge_Loop,
		ChakraJump_Vertical_Charge_Start,
		CharaSelect_Idle,
		DashStep_Behind,
		DashStep_Front,
		DashStep_Left,
		DashStep_Right,
		Dodge_Run_Left,
		Dodge_Run_Right,
		DoubleJump,
		Dying_Aerial_Fall_Behind_Loop,
		Dying_Blasted,
		Dying_Type01,
		etc_Appearance,
		etc_Hand_Aerial_Spray,
		etc_Hand_Spray,
		Fall_Front_Loop,
		Fall_Vertical_Loop,
		GetUp,
		GetUp_Type01,
		Guard_Aerial_GuardHit_Front,
		Guard_Aerial_GuardReverse,
		Guard_Aerial_Loop,
		Guard_BigSword_GuardReverse,
		Guard_GuardBreak_End,
		Guard_GuardBreak_Loop,
		Guard_GuardBreak_Start,
		Guard_GuardHit_Front,
		Guard_GuardHit_Large_Left,
		Guard_GuardHit_Large_Right,
		Guard_GuardHit_UpFront,
		Guard_GuardReverse,
		Guard_Hand_GuardReverse,
		Guard_Loop,
		Hand_DashStep_Front,
		Hand_HeightLand,
		Hand_Idle_Loop,
		Hand_Land,
		Hand_Run_End,
		Hand_Walk_End,
		HandSeal_WoodenDragon_Loop,
		HandSeal_WoodenDragon_Start,
		HeightLand,
		Idle_Loop,
		Jump_Front,
		Jump_Vertical,
		Kunai_DashStep_Front,
		Land,
		PaperBomb_Put,
		RecoveryChakra_End,
		RecoveryChakra_Loop,
		RecoveryChakra_Start,
		Rolling_GetUp,
		Run_End,
		Run_Loop,
		Run_TurnLeft180,
		Run_TurnLeft180_Start,
		Run_TurnLeft90_Start,
		Run_TurnRight180,
		Run_TurnRight180_Start,
		Run_TurnRight90_Start,
		Run_Type01_Loop,
		Sasuke_Attack_Aerial_SlashingHorizontally_Left,
		Sasuke_Attack_Aerial_SlashingUp,
		Sasuke_Attack_Aerial_TripleSlashing,
		Sasuke_Attack_Aerial_TurnKick_Left,
		Sasuke_Attack_DashSlashing_Right,
		Sasuke_Attack_JumpTurnKick_Left,
		Sasuke_Attack_SlashingHorizontally_Right,
		Sasuke_Attack_SlashingShoulder_Right,
		Sasuke_Attack_SlashingUp_Right,
		Sasuke_Attack_TurnKick_Right,
		Sasuke_Attack_TurnSlashingShoulder_Right,
		TakeMedicine,
		Throw_Aerial_Kunai_Left,
		Throw_Aerial_Kunai_Right,
		Throw_Aerial_Kunai_Type01_Front,
		Throw_Kunai_Front,
		Throw_Kunai_JumpTwist,
		Walk_End,
		Walk_Loop,
		Walk_TurnLeft180_Start,
		Walk_TurnLeft90_Start,
		Walk_TurnRight180_Start,
		Walk_TurnRight90_Start,
		Walk_Type01_Loop,
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
	CCustom_Model* m_pModelCom{ nullptr };

private:
	_bool m_isRunning{};
	_float m_fSliding{ 1.f };
	_uint m_iPartNum[PT_END]{};

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