#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CKurama final : public CGameObject
{
private:
	enum Anim
	{
		KuraDef_Attack_DoubleHandScratch,
		KuraDef_Attack_DoubleLowTurnKick,
		KuraDef_Attack_DoubleTurnKick,
		KuraDef_Attack_HugeBite,
		KuraDef_Attack_HugeHandScratch,
		KuraDef_Attack_JumpCrush,
		KuraDef_Attack_SwingingDownScratch_Right,
		KuraDef_Attack_UppercutScratch_Left,
		KuraDef_Beaten_ChanceTimeHit_Left,
		KuraDef_Beaten_ChanceTimeHit_Right,
		KuraDef_Beaten_Down,
		KuraDef_Beaten_Down_Loop,
		KuraDef_Beaten_DownHit_Left,
		KuraDef_Beaten_DownHit_Right,
		KuraDef_Beaten_Type01,
		KuraDef_Beaten_Type02,
		KuraDef_Dying_ChanceTime,
		KuraDef_Dying_Down,
		KuraDef_Dying_Type01,
		KuraDef_Dying_Type01_Loop,
		KuraDef_etc_Appearance,
		KuraDef_etc_ChanceTime_End,
		KuraDef_etc_ChanceTime_Loop,
		KuraDef_etc_ChanceTime_Start,
		KuraDef_etc_Leave_Type01,
		KuraDef_etc_Suggest,
		KuraDef_etc_Suggest_Large,
		KuraDef_Fall_Front_Loop,
		KuraDef_GetUp,
		KuraDef_Idle_Loop,
		KuraDef_Idle_TurnLeft180,
		KuraDef_Idle_TurnRight180,
		KuraDef_Jump_Front,
		KuraDef_Land,
		KuraDef_Ninjutsu_Aerial_TailedBeastBomb,
		KuraDef_Ninjutsu_ConsecutiveTailedBeastBomb,
		KuraDef_Ninjutsu_Roar,
		KuraDef_Ninjutsu_TailedBeastBomb,
		KuraDef_Run_End,
		KuraDef_Run_Loop,
		Kurama_Attack_KickUp,
		Kurama_Attack_MowDown_Left,
		Kurama_Attack_MowDown_Right,
		Kurama_HandSeal_RecoveryChakra_End,
		Kurama_HandSeal_RecoveryChakra_Loop,
		Kurama_HandSeal_RecoveryChakra_Start,
		Kurama_Ninjutsu_Aerial_FakeFoxBlast,
		Kurama_Ninjutsu_Aerial_TailedBeastBomb,
		Kurama_Ninjutsu_ConsecutiveFakeFoxBlast,
		Kurama_Ninjutsu_ConsecutiveTailedBeastBomb,
		Kurama_Ninjutsu_FakeFoxBlast,
		Kurama_Ninjutsu_FakeFoxBlast_Charge,
		Kurama_Ninjutsu_FakeFoxBlast_Charge_Loop,
		Kurama_Ninjutsu_Roar,
		Kurama_Ninjutsu_TailedBeastBomb,
	};

	enum State_Char
	{
		State_Idle,
		State_Attack,
		State_Skill,
		State_Roar,
		State_Beaten,
		State_Die,
		State_Standby
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

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };

private:
	_bool HasInitiated{};
	State_Char m_eState{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	void Artificial_Intelligence(_float fTimeDelta);

	void Change_State();

public:
	static CKurama* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END