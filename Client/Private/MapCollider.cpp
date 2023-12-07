#include "MapCollider.h"

CMapCollider::CMapCollider(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CMapCollider::CMapCollider(const CMapCollider& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMapCollider::Init_Prototype()
{
	return S_OK;
}

HRESULT CMapCollider::Init(void* pArg)
{
	if (pArg)
	{
		ObjectInfo Info = *(ObjectInfo*)pArg;

		m_strPrototypeTag = Info.strPrototypeTag;

		m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&Info.vPos));
		m_pTransformCom->LookAt_Dir(XMLoadFloat4(&Info.vLook));
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CMapCollider::Tick(_float fTimeDelta)
{
}

void CMapCollider::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonLight, this);
}

#ifdef _DEBUG
HRESULT CMapCollider::Render()
{
	if ((m_pGameInstance->Get_CameraModeIndex() != CM_DEBUG && !m_pGameInstance->Key_Pressing(DIK_O)) ||
		(m_pGameInstance->Get_CameraModeIndex() == CM_DEBUG && m_pGameInstance->Key_Pressing(DIK_O)))
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(StaticPass_COLMesh)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}
#endif // _DEBUG


#ifdef _DEBUG
HRESULT CMapCollider::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
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

	_float4 vColor{ 0.3f, 0.8f, 0.3f, 0.5f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _float4)))
	{
		return E_FAIL;
	}

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, 0);
	if (!pLightDesc)
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
#endif // _DEBUG


HRESULT CMapCollider::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), m_strPrototypeTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CMapCollider* CMapCollider::Create(_dev pDevice, _context pContext)
{
	CMapCollider* pInstance = new CMapCollider(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMapCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapCollider::Clone(void* pArg)
{
	CMapCollider* pInstance = new CMapCollider(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMapCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
#endif // _DEBUG
	Safe_Release(m_pModelCom);
}
