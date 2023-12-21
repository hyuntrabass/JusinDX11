#include "Terrain.h"

CTerrain::CTerrain(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Init_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Init(void* pArg)
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

	m_pModelCom->Apply_TransformToActor(m_pTransformCom->Get_World_Matrix());

	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
	//m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Shadow, this);
}

HRESULT CTerrain::Render()
{
	if ((m_pGameInstance->Get_CameraModeIndex() == CM_DEBUG && !m_pGameInstance->Key_Pressing(DIK_O)) ||
		(m_pGameInstance->Get_CameraModeIndex() != CM_DEBUG && m_pGameInstance->Key_Pressing(DIK_O)))
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
		_uint iPassIndex{};

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			_float4 vFailColor{ 1.f, 0.f, 0.f, 1.f };
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vFailColor, sizeof _float4)))
			{
				return E_FAIL;
			}
			iPassIndex = StaticPass_SingleColorFx;
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (m_strPrototypeTag == TEXT("Prototype_Model_SM_ENV_KNFRST_WireMesh_B.mo") or
			m_strPrototypeTag == TEXT("Prototype_Model_MERGED_SM_ENV_KNVLLG_FarTree_A1.mo") or
			m_strPrototypeTag == TEXT("Prototype_Model_SM_ENV_KNVLLG_RockWall_Set_01.mo"))
		{
			iPassIndex = StaticPass_AlphaTestMeshes;
		}

		//if (FAILED(m_pShaderCom->Begin(StaticPass_OutLine)))
		//{
		//	return E_FAIL;
		//}

		//if (FAILED(m_pModelCom->Render(i)))
		//{
		//	return E_FAIL;
		//}

		if (FAILED(m_pShaderCom->Begin(iPassIndex)))
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

HRESULT CTerrain::Render_Shadow()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_Light_ViewProjMatrix(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Light_Main"), m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pShaderCom->Begin(StaticPass_Shadow)))
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

HRESULT CTerrain::Add_Components()
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
		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strPrototypeTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraFar(), sizeof _float)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CTerrain* CTerrain::Create(_dev pDevice, _context pContext)
{
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	CTerrain* pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
