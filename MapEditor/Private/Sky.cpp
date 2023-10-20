#include "Sky.h"

CSky::CSky(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSky::CSky(const CSky& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSky::Init_Prototype()
{
	return S_OK;
}

HRESULT CSky::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_Scale(_float3(0.001f, 0.001f, 0.001f));

	return S_OK;
}

void CSky::Tick(_float fTimeDelta)
{
}

void CSky::Late_Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&m_pGameInstance->Get_CameraPos()));
	m_pRendererCom->Add_RenderGroup(RenderGroup::Priority, this);
}

HRESULT CSky::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(1)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModel->Render()))
	{
		return E_FAIL;
	}
	//if (FAILED(m_pVIBufferCom->Render()))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CSky::Add_Components()
{
	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Sphere"), TEXT("Com_Model"), (CComponent**)&m_pModel)))
	{
		return E_FAIL;
	}

	//if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSky::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSky* CSky::Create(_dev pDevice, _context pContext)
{
	CSky* pInstance = new CSky(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSky::Clone(void* pArg)
{
	CSky* pInstance = new CSky(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pModel);
	Safe_Release(m_pTextureCom);
	//Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
