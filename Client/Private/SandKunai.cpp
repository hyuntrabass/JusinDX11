#include "SandKunai.h"
#include "CommonTrail.h"

CSandKunai::CSandKunai(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSandKunai::CSandKunai(const CSandKunai& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSandKunai::Init_Prototype()
{
	return S_OK;
}

HRESULT CSandKunai::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (not pArg)
	{
		MSG_BOX("No Argument!");
	}

	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(reinterpret_cast<_float4*>(pArg)));
	m_pTransformCom->LookAt(dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_CenterPos());
	m_pTransformCom->Set_Speed(50.f);

	TRAIL_DESC TrailDesc{};
	TrailDesc.vColor = _float4(1.f, 0.f, 0.f, 1.f);
	TrailDesc.vPSize = _float2(0.01f, 0.01f);
	TrailDesc.iNumVertices = 20;

	m_pTrail = dynamic_cast<CCommonTrail*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &TrailDesc));

	return S_OK;
}

void CSandKunai::Tick(_float fTimeDelta)
{
	m_pTransformCom->Go_Straight(fTimeDelta);

	_float3 vPos{};
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(State::Pos));
	m_pTrail->Tick(vPos);

	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	m_pGameInstance->Attack_Player(m_pColliderCom, 7);
	if (m_pGameInstance->CheckCollision_Player(m_pColliderCom))
	{
		m_isDead = true;

		EffectInfo EffectInfo{};
		EffectInfo.vColor = _float4(1.f, 0.f, 0.f, 1.f);
		EffectInfo.fScale = 1.f;
		EffectInfo.iType = 2;
		XMStoreFloat4(&EffectInfo.vPos, m_pTransformCom->Get_State(State::Pos));
		m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Impact"), &EffectInfo);
	}

	if (m_fTimer > 2.f)
	{
		m_isDead = true;
	}

	m_fTimer += fTimeDelta;
}

void CSandKunai::Late_Tick(_float fTimeDelta)
{
	m_pTrail->Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
#ifdef _DEBUGG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG
}

HRESULT CSandKunai::Render()
{
	if (FAILED(Bind_Shader_Resources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			return E_FAIL;
		}

		m_pShaderCom->Begin(StaticPass_OutLine);

		m_pModelCom->Render(i);

		m_pShaderCom->Begin(StaticPass_Default);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CSandKunai::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Kunai"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Sphere;
	ColDesc.fRadius = 0.3f;
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSandKunai::Bind_Shader_Resources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	_float4 vColor{ 1.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof vColor)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSandKunai* CSandKunai::Create(_dev pDevice, _context pContext)
{
	CSandKunai* pInstance = new CSandKunai(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSandKunai");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSandKunai::Clone(void* pArg)
{
	CSandKunai* pInstance = new CSandKunai(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSandKunai");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSandKunai::Free()
{
	__super::Free();

	Safe_Release(m_pTrail);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
