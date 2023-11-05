#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

enum Animation
{
	PlayerAnim_Aerial_Dash_End,
	PlayerAnim_Aerial_Dash_Loop,
	PlayerAnim_Aerial_Dash_Start,
	PlayerAnim_Attack_Aerial_DownStrike,
	PlayerAnim_Attack_Aerial_Punch,
	PlayerAnim_Attack_BodyBlow,
	PlayerAnim_Attack_DoubleTurnKick_Type2,
	PlayerAnim_Attack_DownStrike,
	PlayerAnim_Attack_JumpSpinKick,
	PlayerAnim_Attack_Kunai_SlashShoulderTurnKick_Left,
	PlayerAnim_Attack_Kunai_Uppercut,
	PlayerAnim_Attack_Palm,
	PlayerAnim_Attack_PalmJumpSpinKick,
	PlayerAnim_Attack_supa_1,
	PlayerAnim_Attack_supa_2,
	PlayerAnim_Attack_TurnKick_Left,
	PlayerAnim_ChakraJump_Charge_End,
	PlayerAnim_ChakraJump_Charge_Loop,
	PlayerAnim_ChakraJump_Charge_Start,
	PlayerAnim_ChakraJump_Charge_Vertical_End,
	PlayerAnim_ChakraJump_Charge_Vertical_Loop,
	PlayerAnim_ChakraJump_Charge_Vertical_Start,
	PlayerAnim_DoubleJump,
	PlayerAnim_Idle_Loop,
	PlayerAnim_Jump_Front,
	PlayerAnim_Jump_Vertical,
	PlayerAnim_Land,
	PlayerAnim_Run_End,
	PlayerAnim_Run_Loop,
	PlayerAnim_Walk_End,
	PlayerAnim_Walk_Loop,
	PlayerAnim_WallJump_Loop,
};

class CPlayer final : public CGameObject
{
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