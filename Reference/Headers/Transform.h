#pragma once
#include "Component.h"

BEGIN(Engine)

enum class State
{
	Right,
	Up,
	Look,
	Pos,
	End
};

class ENGINE_DLL CTransform final : public CComponent
{
public:
	using TRANSFORM_DESC = struct tagTransformDesc
	{
		_float fSpeedPerSec{};
		_float fRotationPerSec{};
	};

private:
	CTransform(_dev pDevice, _context pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	const _float4x4& Get_World_float4x4() const;
	const _float4x4& Get_World_Inverse_float4x4() const;
	_vector Get_State(State eState) const;
	const _float3& Get_Scale() const;

	void Set_State(State eState, _fvector vState);
	void Set_Scale(_float3 fScale);
	void Set_Speed(_float fSpeed);
	void Set_RotationPerSec(_float fAngle);

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Go_Up(_float fTimeDelta);
	void Go_Down(_float fTimeDelta);
	void Look_At(_fvector vTargetPos);
	void Move_to(_fvector vTargetPos, _float fTimeDelta, _float fMargin = 0.1f);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Rotation(_fvector vAxis, _float fAngle);

public:
	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pVariableName);

private:
	_float4x4 m_WorldMatrix{};

	_float m_fSpeedPerSec{10.f};
	_float m_fRotationPerSec{XMConvertToRadians(90.f)};

public:
	static CTransform* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END