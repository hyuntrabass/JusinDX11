#include "Kunai.h"

CKunai::CKunai(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CKunai::CKunai(const CKunai& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CKunai::Init_Prototype()
{
	return S_OK;
}

HRESULT CKunai::Init(void* pArg)
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
	if (Info.strPrototypeTag.empty())
	{
		m_bFail = true;
	}
	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&Info.vPos));
	m_pTransformCom->LookAt(XMLoadFloat4(&Info.vLook));
	m_vTargetPos = Info.vLook;

	m_pTransformCom->Set_Speed(50.f);

	return S_OK;
}

void CKunai::Tick(_float fTimeDelta)
{
	if (m_bFail)
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		//m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

		if (m_fLifeTimer > 0.7f)
		{
			//m_isDead = true;
		}

		m_fLifeTimer += fTimeDelta;
	}
	else if (not m_hasStopped)
	{
		if (m_pTransformCom->Go_To(XMLoadFloat4(&m_vTargetPos), fTimeDelta, 0.5f))
		{
			m_hasStopped = true;
		}
	}

	//m_pGameInstance->Attack_Player(m_pColliderCom, 10);
	//m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

void CKunai::Late_Tick(_float fTimeDelta)
{
	if (m_TrailPosList.size() >= 50)
	{
		m_TrailPosList.pop_back();
	}
	_float3 vPos{};
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(State::Pos));
	m_TrailPosList.push_front(vPos);
	_float3 PosArray[50]{};
	_uint iIndex{};
	for (size_t i = 0; i < 50; i++)
	{
		PosArray[i] = m_TrailPosList.back();
	}
	for (auto& vPos : m_TrailPosList)
	{
		PosArray[iIndex++] = vPos;
	}
	
	m_pTrailBufferCom->Update(fTimeDelta, PosArray);

	__super::Compute_CamDistance();

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Blend, this);
}

HRESULT CKunai::Render()
{
	if (FAILED(Render_Effect()))
	{
		return E_FAIL;
	}

	if (FAILED(Bind_Shader_Resources()))
	{
		return E_FAIL;
	}

	for (size_t i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			return E_FAIL;
		}

		m_pShaderCom->Begin(StaticPass_Default);

		m_pModelCom->Render(i);
	}

	if (FAILED(Render_Trail()))
	{
		return E_FAIL;
	}

#ifdef _DEBUG
	//m_pColliderCom->Render();
#endif // _DEBUG

	return S_OK;
}

const _bool& CKunai::HasStoppe()
{
	return m_hasStopped;
}

HRESULT CKunai::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Circle"), TEXT("Com_EffectModel"), reinterpret_cast<CComponent**>(&m_pEffectModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Sphere07"), TEXT("Com_EffectMask"), reinterpret_cast<CComponent**>(&m_pEffectTexture))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Trail"), TEXT("Com_TrailShader"), reinterpret_cast<CComponent**>(&m_pTrailShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pTrailBufferCom))))
	{
		return E_FAIL;
	}

	//Collider_Desc ColDesc{};
	//ColDesc.eType = ColliderType::Sphere;
	//ColDesc.fRadius = 0.5f;
	//ColDesc.vCenter = _float3(0.f, 0.f, 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Take"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColDesc)))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CKunai::Bind_Shader_Resources()
{
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

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(m_pGameInstance->Get_CurrentLevelIndex(), 0);
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

	_float fNorTex{};
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNorTex", &fNorTex, sizeof _float)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CKunai::Render_Effect()
{
	_float fScale = 0.6f;

	_vector vLook = XMVector3Normalize(XMVector3Cross(XMLoadFloat4(&m_pGameInstance->Get_CameraPos()) - m_pTransformCom->Get_State(State::Pos), m_pGameInstance->Get_Transform_Inversed(D3DTS::View).r[0])) * fScale;
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * fScale;
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * fScale;
	_vector vPos = m_pTransformCom->Get_State(State::Pos) + XMVector3Normalize(m_pTransformCom->Get_State(State::Pos) - XMLoadFloat4(&m_pGameInstance->Get_CameraPos())) * 0.2f;

	_matrix vOffset(vRight, vUp, vLook, vPos);

	_float44 Offset{};

	XMStoreFloat4x4(&Offset, vOffset);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", Offset)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pEffectTexture->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
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

	_float4 vColor{ 0.2f, 0.2f, 1.f, 1.f };
	//XMStoreFloat4(&vColor, Colors::CornflowerBlue);
	m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

	m_pShaderCom->Begin(StaticPass_MaskEffect);

	m_pEffectModelCom->Render(0);

	return S_OK;
}

HRESULT CKunai::Render_Trail()
{
	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailShaderCom->Begin(0)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailBufferCom->Render()))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

CKunai* CKunai::Create(_dev pDevice, _context pContext)
{
	CKunai* pInstance = new CKunai(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CKunai");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKunai::Clone(void* pArg)
{
	CKunai* pInstance = new CKunai(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CKunai");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKunai::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pEffectModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pEffectTexture);
	Safe_Release(m_pTrailShaderCom);
	Safe_Release(m_pTrailBufferCom);
}
