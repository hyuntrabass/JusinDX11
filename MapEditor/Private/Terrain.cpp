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
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (pArg)
	{
		m_pPos = (_float*)pArg;
	}

	m_pTransformCom->Set_RotationPerSec(200.f);

	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{
}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(1)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTerrain::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TT_Terrain]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"), TEXT("Com_Texture_Cursor"), reinterpret_cast<CComponent**>(&m_pTextureCom[TT_Cursor]))))
	{
		return E_FAIL;
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

	if (FAILED(m_pTextureCom[TT_Terrain]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom[TT_Cursor]->Bind_ShaderResource(m_pShaderCom, "g_Texture_Cursor")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCursorPos", m_pPos, sizeof _float * 3)))
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

	for (size_t i = 0; i < TT_End; i++)
	{
		Safe_Release(m_pTextureCom[i]);
	}
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
