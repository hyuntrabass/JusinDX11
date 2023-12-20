#include "Aim.h"

CAim::CAim(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CAim::CAim(const CAim& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CAim::Init_Prototype()
{
	return S_OK;
}

HRESULT CAim::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 512.f;
	m_fSizeY = 512.f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_fDepth = 0.5f;
	m_iHP = 1;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CAim::Tick(_float fTimeDelta)
{
	_float fRotationPerSec{ 360.f };
	_float fMaxRotationAngle{ 90.f };

	fTimeDelta = fTimeDelta / m_pGameInstance->Get_TimeRatio();

	if (m_iHP > 0)
	{
		if (m_fAngle < fMaxRotationAngle)
		{
			m_fAppearance = m_fAngle / fMaxRotationAngle;
			m_fScale = Lerp(1.f, 0.5f, m_fAppearance);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_fAngle);
			m_fAngle += fRotationPerSec * fTimeDelta;
		}
		else
		{
			m_fScale = 0.5f;
			m_fAngle = fMaxRotationAngle;
			m_fAppearance = 1.f;
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_fAngle);
		}
	}
	else
	{
		if (m_fAngle > 0.f)
		{
			m_fAppearance = m_fAngle / fMaxRotationAngle;
			m_fScale = Lerp(0.01f, 0.5f, m_fAppearance);
			//m_fScale = Lerp(1.f, 0.5f, m_fAppearance);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_fAngle);
			m_fAngle -= fRotationPerSec * fTimeDelta;
		}
		else
		{
			m_isDead = true;
		}

	}

	m_pTransformCom->Set_Scale(_float3(m_fSizeX * m_fScale, m_fSizeY * m_fScale, 1.f));
}

void CAim::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CAim::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Mask_Color)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAim::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Aim"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAim::Bind_ShaderResources()
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

	_float4 vColor{ 0.f, 1.f, 0.7f, m_fAppearance };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _float4)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CAim* CAim::Create(_dev pDevice, _context pContext)
{
	CAim* pInstance = new CAim(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CAim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAim::Clone(void* pArg)
{
	CAim* pInstance = new CAim(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CAim");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAim::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
