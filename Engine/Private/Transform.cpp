#include "Transform.h"

CTransform::CTransform(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_vector CTransform::Get_State(State eState) const
{
	return XMLoadFloat4x4(&m_WorldMatrix).r[ToIndex(eState)];
}

const _float4x4& CTransform::Get_World_float4x4() const
{
	return m_WorldMatrix;
}

void CTransform::Set_State(State eState, _fvector vState)
{
	_matrix TransformMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	TransformMatrix.r[ToIndex(eState)] = vState;
	XMStoreFloat4x4(&m_WorldMatrix, TransformMatrix);
}

void CTransform::Set_Scale(_float fScale)
{
}

HRESULT CTransform::Init_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Init(void* pArg)
{
	if (pArg)
	{
		TRANSFORM_DESC Desc = *(TRANSFORM_DESC*)pArg;

		m_fSpeedPerSec = Desc.fSpeedPerSec;
		m_fRotationPerSec = Desc.fRotationPerSec;
	}

	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector vPos = Get_State(State::Pos);
	_vector vLook = Get_State(State::Look);

	vPos += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(State::Pos, vPos);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector vPos = Get_State(State::Pos);
	_vector vLook = Get_State(State::Look);

	vPos -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	Set_State(State::Pos, vPos);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPos = Get_State(State::Pos);
	_vector vRight = Get_State(State::Right);

	vPos -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(State::Pos, vPos);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPos = Get_State(State::Pos);
	_vector vRight = Get_State(State::Right);

	vPos += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(State::Pos, vPos);
}

void CTransform::Go_Up(_float fTimeDelta)
{
	_vector vPos = Get_State(State::Pos);
	_vector vUp = Get_State(State::Up);

	vPos += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(State::Pos, vPos);
}

void CTransform::Go_Down(_float fTimeDelta)
{
	_vector vPos = Get_State(State::Pos);
	_vector vUp = Get_State(State::Up);

	vPos -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	Set_State(State::Pos, vPos);
}

void CTransform::LookAt(const _float3& vTargetPos)
{
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix Rotation = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	_vector vRight = XMVector3TransformNormal(Get_State(State::Right), Rotation);
	_vector vUp = XMVector3TransformNormal(Get_State(State::Up), Rotation);
	_vector vLook = XMVector3TransformNormal(Get_State(State::Look), Rotation);

	Set_State(State::Right, vRight);
	Set_State(State::Up, vUp);
	Set_State(State::Look, vLook);
}

void CTransform::Rotation(_fvector vAxis, _float fAngle)
{
}

CTransform* CTransform::Create(_dev pDevice, _context pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
