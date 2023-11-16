#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

enum PART_TYPE
{
	PT_HEAD,
	PT_FACE,
	PT_UPPER_BODY,
	PT_LOWER_BODY,
	PT_END
};

struct BODYPART_DESC
{
	PART_TYPE eType{};
	_uint iNumVariations{};
	pair<_uint, _bool>* Animation{};

	CTransform* pParentTransform{ nullptr };
};

enum class Player_State
{
	Idle,
	Walk,
	Run,
	Jump,
	DoubleJump,
	Beaten,
	Attack,
};

class CPlayer final : public CGameObject
{
private:
	enum Animation
	{
		Aerial_Dash_End,
		Aerial_Dash_Loop,
		Aerial_Dash_Start,
		Aerial_RecoveryChakra_End,
		Aerial_RecoveryChakra_Loop,
		Aerial_RecoveryChakra_Start,
		Aerial_TakeMedicine,
		Beaten_BellyDown_Small_End,
		Beaten_BellyDown_Small_Loop,
		Beaten_BellyDown_Small_Start,
		Beaten_Burn_Type01,
		Beaten_Kunai,
		Beaten_Left,
		Beaten_Paralysis,
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
		Boruto_etc_Win_Type01_End,
		Boruto_etc_Win_Type01_Loop,
		Boruto_etc_Win_Type01_Start,
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
		Hand_DashStep_Front,
		Hand_HeightLand,
		Hand_Idle_Loop,
		Hand_Land,
		Hand_Run_End,
		Hand_Walk_End,
		HeightLand,
		Idle_Loop,
		Jump_Front,
		Jump_Vertical,
		Kunai_DashStep_Front,
		Kunai_HeightLand,
		Kunai_Idle_Loop,
		Kunai_Land,
		Kunai_Run_End,
		Kunai_Walk_End,
		Land,
		Land_toChidoriCharge,
		Land_toD23NJ4Charge,
		Land_toRasengunCharge,
		Land_toRecoveryChakra,
		Ninjutsu_Aerial_LaughingMonk,
		Ninjutsu_Aerial_LightningBlade_Charge_Loop,
		Ninjutsu_Aerial_LightningBlade_Charge_Start,
		Ninjutsu_Aerial_LightningBlade_End,
		Ninjutsu_Aerial_LightningBlade_Loop,
		Ninjutsu_Aerial_LightningBlade_Start,
		Ninjutsu_LightningBlade_Attack_End,
		Ninjutsu_LightningBlade_Charge_Lv2toLv3,
		Ninjutsu_LightningBlade_Charge_Lv2toLv3_Conect_toRun,
		Ninjutsu_LightningBlade_Charge_Lv2toLv3_Loop,
		Ninjutsu_LightningBlade_Run_Loop,
		Ninjutsu_TrueRasenShuriken,
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
	Player_State m_eState{};
	_bool m_isRunning{};
	_float m_fSliding{ 1.f };
	pair<_uint, _bool> m_Animation{};

	class CBodyPart* m_pBodyParts[PT_END]{};
	_uint m_iPartNum[PT_END]{};

	_float m_fInterpolationRatio{};
	_bool m_isInterpolating{};
	_float3 m_vOriginalLook{};

private:
	void Move(_float fTimeDelta);
	void Customize(_float fTimeDelta);

private:
	HRESULT Ready_Parts();

public:
	static CPlayer* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END