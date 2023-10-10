#include "Logo.h"

CLogo::CLogo(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CLogo::CLogo(const CLogo& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CLogo::Init_Prototype()
{
	return S_OK;
}

HRESULT CLogo::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 1024;
	m_fSizeY = 256;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 2;

	m_pTransformCom->Set_RotationPerSec(540.f);

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CLogo::Tick(_float fTimeDelta)
{
	if (m_fAngle < 360.f)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_fAngle);
		m_fAngle += 540.f * fTimeDelta;
	}
	else
	{
		m_fAngle = 360.f;
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_fAngle);
	}
}

void CLogo::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::UI, this);
}

HRESULT CLogo::Render()
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

HRESULT CLogo::Add_Components()
{
	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Logo), TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLogo::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLogo* CLogo::Create(_dev pDevice, _context pContext)
{
	CLogo* pInstance = new CLogo(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLogo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLogo::Clone(void* pArg)
{
	CLogo* pInstance = new CLogo(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLogo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogo::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
