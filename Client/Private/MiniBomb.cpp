#include "MiniBomb.h"

CMiniBomb::CMiniBomb(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CMiniBomb::CMiniBomb(const CMiniBomb& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniBomb::Init_Prototype()
{
	return S_OK;
}

HRESULT CMiniBomb::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (not pArg)
	{
		MSG_BOX("No Argument");
		return E_FAIL;
	}

	ObjectInfo Info = *reinterpret_cast<ObjectInfo*>(pArg);

	//m_pTransformCom->Set_Scale(_float3(0.5f, 0.5f, 0.5f));
	Info.vPos.w = 1.f;
	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&Info.vPos));
	m_pTransformCom->LookAt(XMLoadFloat4(&Info.vLook));

	m_pTransformCom->Set_Speed(100.f);
	m_pTransformCom->Set_RotationPerSec(360.f *2.f);

	return S_OK;
}

void CMiniBomb::Tick(_float fTimeDelta)
{
	if (m_fLifeTimer > 10.f)
	{
		m_isDead = true;
	}

	m_pTransformCom->Turn(XMVector3Normalize(m_pTransformCom->Get_State(State::Look)), fTimeDelta);

	m_fLifeTimer += fTimeDelta;

	m_pTransformCom->Go_Straight(fTimeDelta);

	m_pGameInstance->Attack_Player(m_pColliderCom, 10);
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

void CMiniBomb::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
}

HRESULT CMiniBomb::Render()
{
	if (FAILED(Bind_Shader_Resources()))
	{
		return E_FAIL;
	}

	m_pShaderCom->Begin(StaticPass_SingleColorFx);

	m_pModelCom->Render(0);
	m_pEffectModelCom->Render(0);

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CMiniBomb::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sphere"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_FlyingSphere"), TEXT("Com_EffectModel"), reinterpret_cast<CComponent**>(&m_pEffectModelCom))))
	{
		return E_FAIL;
	}

	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Sphere;
	ColDesc.fRadius = 0.5f;
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Take"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMiniBomb::Bind_Shader_Resources()
{
	_float4 vBaseColor{ 0.03f, 0.f, 0.5f, 0.8f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vBaseColor, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_BOSSSTAGE, 0);
	if (not pLightDesc)
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof _float4)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CMiniBomb* CMiniBomb::Create(_dev pDevice, _context pContext)
{
	CMiniBomb* pInstance = new CMiniBomb(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMiniBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniBomb::Clone(void* pArg)
{
	CMiniBomb* pInstance = new CMiniBomb(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMiniBomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniBomb::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pEffectModelCom);
	Safe_Release(m_pColliderCom);
}
