#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"

const PxVec3 VectorToPxVec3(XMVECTOR vVector)
{
	return PxVec3(vVector.m128_f32[0], vVector.m128_f32[1], vVector.m128_f32[2]);
}

const _vector PxExVec3ToVector(PxExtendedVec3 Src, float w)
{
	return XMVectorSet(static_cast<float>(Src.x), static_cast<float>(Src.y), static_cast<float>(Src.z), w);
}

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

_float3 CTransform::Get_Scale() const
{
	return _float3(XMVectorGetX(XMVector3Length(Get_State(State::Right)))
				   , XMVectorGetX(XMVector3Length(Get_State(State::Up)))
				   , XMVectorGetX(XMVector3Length(Get_State(State::Look))));
}

const _float& CTransform::Get_Speed() const
{
	return m_fSpeedPerSec;
}

const _bool& CTransform::Is_Jumping() const
{
	return m_isJumping;
}

PxController* CTransform::Get_Controller() const
{
	return m_pController;
}

_matrix CTransform::Get_World_Matrix() const
{
	return XMLoadFloat4x4(&m_WorldMatrix);
}

const _float44& CTransform::Get_World_float4x4() const
{
	return m_WorldMatrix;
}

_float44 CTransform::Get_World_Inverse_float4x4() const
{
	_float44 WorldInversed{};
	XMStoreFloat4x4(&WorldInversed, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
	return WorldInversed;
}

void CTransform::Set_Position(_float3 vPosition)
{
	if (m_pController)
	{

	PxExtendedVec3 NewPos{ static_cast<_double>(vPosition.x), static_cast<_double>(vPosition.y), static_cast<_double>(vPosition.z) };
	m_pController->setPosition(NewPos);
	}
	else
	{
		return;
	}
}

void CTransform::Set_State(State eState, _fvector vState)
{
	_matrix TransformMatrix = XMLoadFloat4x4(&m_WorldMatrix);
	TransformMatrix.r[ToIndex(eState)] = vState;
	XMStoreFloat4x4(&m_WorldMatrix, TransformMatrix);
}

void CTransform::Set_Scale(_float3 fScale)
{
	_vector vRight = XMVector3Normalize(Get_State(State::Right));
	_vector vUp = XMVector3Normalize(Get_State(State::Up));
	_vector vLook = XMVector3Normalize(Get_State(State::Look));

	Set_State(State::Right, vRight * fScale.x);
	Set_State(State::Up, vUp * fScale.y);
	Set_State(State::Look, vLook * fScale.z);
}

void CTransform::Set_Speed(_float fSpeed)
{
	m_fSpeedPerSec = fSpeed;
}

void CTransform::Set_RotationPerSec(_float fAngle)
{
	m_fRotationPerSec = XMConvertToRadians(fAngle);
}

void CTransform::Set_Controller(PxController* pController)
{
	m_pController = pController;
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

void CTransform::Gravity(_float fTimeDelta)
{
	_float Gravity{ -19.81f };
	if (m_fJumpForce > 0.f)
	{
		m_fJumpForce += Gravity * fTimeDelta;
		m_fGravity = m_fJumpForce;
		//m_fGravity = 25 * sin(40.f * XM_PI / 180.f) - 9.8f * fTimeDelta;
		//Gravity.y = m_fGravity;
	}
	else if (!m_CollisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_fGravity += Gravity * fTimeDelta;
	}

	m_CollisionFlags = m_pController->move(PxVec3(0.f, m_fGravity, 0.f) * fTimeDelta, 0.0001f, fTimeDelta, 0);
	if (m_CollisionFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		m_fJumpForce = 0.f;
		m_fGravity = 0.f;
		m_isJumping = false;
	}
	PxExtendedVec3 MovedPos = m_pController->getFootPosition();
	Set_State(State::Pos, PxExVec3ToVector(MovedPos, 1.f));
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	if (m_pController)
	{
		PxControllerFilters Filters{};
		Filters.mFilterData = &PxFilterData(0, 0, 0, 0);
		PxVec3 Disp = VectorToPxVec3(XMVector3Normalize(XMVectorSetY(Get_State(State::Look), 0.f)) * m_fSpeedPerSec * fTimeDelta);
		m_pController->move(Disp, 0.0001f, fTimeDelta, Filters);

		PxExtendedVec3 MovedPos = m_pController->getFootPosition();
		Set_State(State::Pos, PxExVec3ToVector(MovedPos, 1.f));
	}
	else
	{
		_vector vPos = Get_State(State::Pos);
		_vector vLook = Get_State(State::Look);

		vPos += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;
		Set_State(State::Pos, vPos);
	}

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

void CTransform::Jump(_float fJumpForce)
{
	m_isJumping = true;
	m_fJumpForce = fJumpForce;
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_float3 vScale = Get_Scale();

	_vector vLook = vTargetPos - Get_State(State::Pos);
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	Set_State(State::Right, XMVector3Normalize(vRight) * vScale.x);
	Set_State(State::Up, XMVector3Normalize(vUp) * vScale.y);
	Set_State(State::Look, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_Dir(_fvector vDir)
{
	_vector vTargetPos = Get_State(State::Pos) + vDir;
	LookAt(vTargetPos);
}

void CTransform::Move_to(_fvector vTargetPos, _float fTimeDelta, _float fMargin)
{
	_vector vPos = Get_State(State::Pos);

	_vector vDir = vTargetPos - vPos;

	_float fDist = XMVectorGetX(XMVector3Length(vDir));

	if (fDist < fMargin)
	{
		return;
	}

	vPos += vDir * m_fSpeedPerSec * fTimeDelta;

	Set_State(State::Pos, vPos);
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
	_float3 vScale = Get_Scale();

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix Rotation = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fAngle));

	vRight = XMVector3TransformNormal(vRight, Rotation);
	vUp = XMVector3TransformNormal(vUp, Rotation);
	vLook = XMVector3TransformNormal(vLook, Rotation);

	Set_State(State::Right, vRight);
	Set_State(State::Up, vUp);
	Set_State(State::Look, vLook);
}

void CTransform::Set_Rotation(_vector vQuaternion)
{
	_float3 vScale = Get_Scale();

	_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix Rotation = XMMatrixRotationQuaternion(vQuaternion);

	vRight = XMVector3TransformNormal(vRight, Rotation);
	vUp = XMVector3TransformNormal(vUp, Rotation);
	vLook = XMVector3TransformNormal(vLook, Rotation);

	Set_State(State::Right, vRight);
	Set_State(State::Up, vUp);
	Set_State(State::Look, vLook);
}

HRESULT CTransform::Bind_WorldMatrix(CShader* pShader, const _char* pVariableName)
{
	return pShader->Bind_Matrix(pVariableName, m_WorldMatrix);
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

	if (m_pController)
	{
		m_pController->release();
	}
}
