#include "Dummy.h"

CDummy::CDummy(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CDummy::CDummy(const CDummy& rhs)
	: CBlendObject(rhs)
{
}

void CDummy::Select(const _bool& isSelected)
{
	m_isSelected = isSelected;
}

void CDummy::Modify(_fvector vPos, _fvector vLook)
{
	m_pTransformCom->Set_State(State::Pos, vPos);
	m_pTransformCom->LookAt_Dir(vLook);
	XMStoreFloat4(&m_Info.vPos, vPos);
	XMStoreFloat4(&m_Info.vLook, vLook);
}

void CDummy::Get_State(_float4& vPos, _float4& vLook)
{
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(State::Pos));
	XMStoreFloat4(&vLook, m_pTransformCom->Get_State(State::Look));
}

HRESULT CDummy::WriteFile(ofstream& File, _bool WriteWData, _uint iWData)
{
	if (WriteWData)
	{
		m_Info.vPos.w = iWData;
	}
	size_t iNameSize = m_Info.Prototype.size() * sizeof(_tchar) + sizeof(_tchar);
	File.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
	File.write(reinterpret_cast<const _char*>(m_Info.Prototype.data()), iNameSize);
	File.write(reinterpret_cast<const _char*>(&m_Info.eType), sizeof ItemType);
	File.write(reinterpret_cast<const _char*>(&m_Info.iStageIndex), sizeof _uint);
	File.write(reinterpret_cast<const _char*>(&m_Info.vPos), sizeof _float4);
	File.write(reinterpret_cast<const _char*>(&m_Info.vLook), sizeof _float4);

	return S_OK;
}

HRESULT CDummy::Init_Prototype()
{
	return S_OK;
}

HRESULT CDummy::Init(void* pArg)
{
	m_Info = *(DummyInfo*)pArg;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (m_Info.ppDummy)
	{
		*m_Info.ppDummy = this;
		m_Info.ppDummy = nullptr;
	}

	if (m_Info.Prototype == L"Prototype_Model_SM_ENV_KNFRST_WireMesh_B.mo" ||
		m_Info.Prototype == L"Prototype_Model_MERGED_SM_ENV_KNVLLG_FarTree_A1.mo" ||
		m_Info.Prototype == L"Prototype_Model_SM_ENV_KNVLLG_RockWall_Set_01.mo")
	{
		m_iShaderPass = StaticPass_AlphaTestMeshes;
	}

	if (m_Info.Prototype == L"Prototype_Model_Pain")
	{
		m_Animation.iAnimIndex = (rand() % 2) + 1;
		m_Animation.isLoop = true;
	}

	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&m_Info.vPos));
	m_pTransformCom->LookAt_Dir(XMLoadFloat4(&m_Info.vLook));

	return S_OK;
}

void CDummy::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CurrentLevelIndex() != m_Info.iStageIndex)
	{
		m_isDead = true;
	}

	if (m_Info.pImguiMgr)
	{
		if (m_Info.pImguiMgr->ComputePickPos())
		{
			_float4 vPickPos{};
			if (m_pModelCom->Intersect_RayModel(m_pTransformCom->Get_World_Matrix(), &vPickPos))
			{
				m_Info.pImguiMgr->SetPos(vPickPos, this);
			}
		}
		if (m_Info.pImguiMgr->ComputeSelection())
		{
			_float4 vPickPos{};
			if (m_pModelCom->Intersect_RayModel(m_pTransformCom->Get_World_Matrix(), &vPickPos))
			{
				m_Info.pImguiMgr->Select(vPickPos, this);
			}
		}
	}

	if (m_Info.Prototype == L"Prototype_Model_Kurama" || m_Info.Prototype == L"Prototype_Model_Pain")
	{
		if (m_pGameInstance->Key_Pressing(DIK_UP))
		{
			//m_pTransformCom->Go_Straight(fTimeDelta);
			m_pModelCom->Set_Animation(m_Animation);
			//m_pModelCom->Set_Animation(1);
		}
		else
		{
			m_pModelCom->Set_Animation(m_Animation);
		}

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CDummy::Late_Tick(_float fTimeDelta)
{
	if (m_Info.eType == ItemType::COLMap)
	{
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonLight, this);
	}
	else
	{
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
	}
}

HRESULT CDummy::Render()
{
	if (m_Info.eType == ItemType::COLMap)
	{
		if (not m_pGameInstance->Key_Pressing(DIK_O))
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
	}
	else
	{
		//if (m_pGameInstance->Key_Pressing(DIK_O))
		//{
		//	return S_OK;
		//}

		if (FAILED(Bind_ShaderResources()))
		{
			return E_FAIL;
		}

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_float fNorTex = 0.f;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
			{
				fNorTex = 0.f;
			}
			else
			{
				fNorTex = 1.f;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fNorTex", &fNorTex, sizeof _float)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelected", &m_isSelected, sizeof _bool)))
			{
				return E_FAIL;
			}

			if (m_Info.Prototype == L"Prototype_Model_Kurama" || m_Info.Prototype == L"Prototype_Model_Pain")
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
				{
					return E_FAIL;
				}
			}

			if (FAILED(m_pShaderCom->Begin(m_iOutLineShaderPass)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pModelCom->Render(i)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CDummy::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
	if (m_Info.Prototype == L"Prototype_Model_Kurama" || m_Info.Prototype == L"Prototype_Model_Pain")
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
		m_iShaderPass = AnimPass_Default;
		m_iOutLineShaderPass = AnimPass_OutLine;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
		m_iShaderPass = StaticPass_Default;
		m_iOutLineShaderPass = StaticPass_OutLine;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Info.Prototype, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy::Bind_ShaderResources()
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

	if (m_Info.eType == ItemType::COLMap)
	{
		_float4 vColor{ 0.3f, 0.8f, 0.3f, 0.5f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _float4)))
		{
			return E_FAIL;
		}

		const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));
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
	}

	return S_OK;
}

CDummy* CDummy::Create(_dev pDevice, _context pContext)
{
	CDummy* pInstance = new CDummy(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy::Clone(void* pArg)
{
	CDummy* pInstance = new CDummy(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
