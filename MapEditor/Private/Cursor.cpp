#include "Cursor.h"

CCursor::CCursor(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CCursor::CCursor(const CCursor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCursor::Init_Prototype()
{
	return S_OK;
}

HRESULT CCursor::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (pArg)
	{
		m_pPos = (_float*)pArg;
	}

	m_pTransformCom->Set_Scale(_float3(0.1f, 5.f, 0.1f));

	return S_OK;
}

void CCursor::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(State::Pos, XMVectorSet(m_pPos[0], m_pPos[1], m_pPos[2], 1.f));
}

void CCursor::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::NonBlend, this);
}

HRESULT CCursor::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(0)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCursor::Add_Components()
{
	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Texture_Dummy"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCursor::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CCursor* CCursor::Create(_dev pDevice, _context pContext)
{
	CCursor* pInstance = new CCursor(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCursor::Clone(void* pArg)
{
	CCursor* pInstance = new CCursor(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCursor::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
