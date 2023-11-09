#include "Sandman.h"

CSandman::CSandman(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSandman::CSandman(const CSandman& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSandman::Init_Prototype()
{
	return S_OK;
}

HRESULT CSandman::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if(rand() & 2)
	{
		m_pModelCom->Set_Animation(etc_Appearance_Type01);
	}
	else
	{
		m_pModelCom->Set_Animation(etc_Appearance_Type02);
	}

	if (pArg)
	{
		_float4 vPos = *reinterpret_cast<_float4*>(pArg);

		m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&vPos));
	}

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER);

	m_pTransformCom->Set_Speed(5.f);

	return S_OK;
}

void CSandman::Tick(_float fTimeDelta)
{
	if (m_pModelCom->IsAnimationFinished(m_pModelCom->Get_CurrentAnimationIndex()))
	{
		m_pModelCom->Set_Animation(rand() % 144);
		Move(fTimeDelta);
	}

	m_pTransformCom->Gravity(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CSandman::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::NonBlend, this);
}

HRESULT CSandman::Render()
{
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

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(0)))
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

void CSandman::Move(_float fTimeDelta)
{
}

HRESULT CSandman::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sandman"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSandman::Bind_ShaderResources()
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

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_CREATECHARACTER, 0);
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

CSandman* CSandman::Create(_dev pDevice, _context pContext)
{
	CSandman* pInstance = new CSandman(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSandman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSandman::Clone(void* pArg)
{
	CSandman* pInstance = new CSandman(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSandman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSandman::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
