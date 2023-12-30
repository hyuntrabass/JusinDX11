#include "Hit.h"

CHit::CHit(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CHit::CHit(const CHit& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CHit::Init_Prototype()
{
	return S_OK;
}

HRESULT CHit::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_vNumSize = { 108 * 0.7f, 128 * 0.7f };
	m_vSmallHitSize = { 256, 72 };
	m_vHitSize = { m_vSmallHitSize.x * 1.5f, m_vSmallHitSize.y * 1.5f };
	m_vFxSize[0] = { 256.f, 256.f };
	m_vFxSize[1] = { 256.f, 256.f };

	m_fDepth = 0.5f;
	m_iHP = 1;
	m_fAlpha = 0.f;
	m_fFxAlpha = 1.f;
	m_fSizeRatio = 1.f;

	return S_OK;
}

void CHit::Tick(_float fTimeDelta)
{
	if (m_fTimer > 2.f)
	{
		m_iHP = 0;
	}

	if (m_iHP > 0)
	{
		m_fAlpha = Lerp(0.f, 1.f, m_fTimer * 10.f);
		m_fFxAlpha = Lerp(1.f, 0.7f, m_fTimer);
		m_fSizeRatio = Lerp(1.f, 1.5f, m_fTimer * 10.f);
		if (m_fTimer > 0.1f)
		{
			m_fSizeRatio = Lerp(1.5f, 1.f, m_fTimer * 5.f - 0.1f);
		}
	}
	else
	{
		if (m_fAlpha < 0.f)
		{
			m_isDead = true;
		}
		m_fAlpha -= fTimeDelta;
		m_fFxAlpha = Lerp(0.f, 0.7f, m_fAlpha);
	}

	XMStoreFloat4x4(&m_Rotation, XMMatrixTranslation(-320, 0.f, 0.f) *
					XMMatrixRotationZ(XMConvertToRadians(30.f)) *
					XMMatrixScaling(m_fSizeRatio, m_fSizeRatio, m_fSizeRatio) *
					XMMatrixTranslation(320.f, 0.f, 0.f));

	m_fTimer += fTimeDelta;
	m_fFxRotation += fTimeDelta;
}

void CHit::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CHit::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (m_iCombo > 1)
	{
		m_fSizeX = m_vNumSize.x;
		m_fSizeY = m_vNumSize.y;
		m_fX = (g_iWinSizeX >> 2) * 3.f - (m_vNumSize.x * 0.5f);
		m_fY = g_iWinSizeY >> 1;
		__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

		if (FAILED(Bind_ShaderResources()))
		{
			return E_FAIL;
		}

		if (FAILED(m_pNumTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCombo % 10)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(VTPass_Hit)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pVIBufferCom->Render()))
		{
			return E_FAIL;
		}

		// 10자리 수.
		if (m_iCombo > 9)
		{
			m_fX = (g_iWinSizeX >> 2) * 3.f - (m_vNumSize.x * 0.5f) - m_vNumSize.x * 0.75f;
			m_fY = g_iWinSizeY >> 1;
			__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

			if (FAILED(Bind_ShaderResources()))
			{
				return E_FAIL;
			}

			if (FAILED(m_pNumTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (m_iCombo % 100) / 10)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(VTPass_Hit)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pVIBufferCom->Render()))
			{
				return E_FAIL;
			}
			
			// 100자리 수.
			if (m_iCombo > 99)
			{
				m_fX = (g_iWinSizeX >> 2) * 3.f - (m_vNumSize.x * 0.5f) - m_vNumSize.x * 1.5f;
				m_fY = g_iWinSizeY >> 1;
				__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

				if (FAILED(Bind_ShaderResources()))
				{
					return E_FAIL;
				}

				if (FAILED(m_pNumTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCombo / 100)))
				{
					return E_FAIL;
				}

				if (FAILED(m_pShaderCom->Begin(VTPass_Hit)))
				{
					return E_FAIL;
				}

				if (FAILED(m_pVIBufferCom->Render()))
				{
					return E_FAIL;
				}
			}
		}

		m_fSizeX = m_vSmallHitSize.x;
		m_fSizeY = m_vSmallHitSize.y;
		m_fX = (g_iWinSizeX >> 2) * 3.f + (m_vSmallHitSize.x * 0.45f);
		m_fY = (g_iWinSizeY >> 1) + (m_vNumSize.y * 0.65f) - (m_vSmallHitSize.y * 0.5f);
		__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

		if (FAILED(Bind_ShaderResources()))
		{
			return E_FAIL;
		}

		if (FAILED(m_pSmallHitsTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		{
			return E_FAIL;
		}
	}
	else
	{
		m_fSizeX = m_vHitSize.x;
		m_fSizeY = m_vHitSize.y;
		m_fX = (g_iWinSizeX >> 2) * 3.f;
		m_fY = g_iWinSizeY >> 1;
		__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

		if (FAILED(Bind_ShaderResources()))
		{
			return E_FAIL;
		}

		if (FAILED(m_pHitTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		{
			return E_FAIL;
		}
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Hit)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_fSizeX = m_vFxSize[0].x;
	m_fSizeY = m_vFxSize[0].y;
	m_fX = (g_iWinSizeX >> 2) * 3.f;
	m_fY = g_iWinSizeY >> 1;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	_float44 FxWorld{};
	XMStoreFloat4x4(&FxWorld, XMMatrixRotationZ(XMConvertToRadians(90.f) * m_fFxRotation) * m_pTransformCom->Get_World_Matrix());
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", FxWorld)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fFxAlpha, sizeof m_fFxAlpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pFxTextureCom[0]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	_float4 vColor = { 1.f, 1.f, 0.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof vColor)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Mask_ColorAlpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

_uint CHit::Add_Hit()
{
	m_fTimer = {};
	m_iHP = 1;
	m_fAlpha = 1.f;
	return ++m_iCombo;
}

HRESULT CHit::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HitNum"), TEXT("Com_Texture_Num"), reinterpret_cast<CComponent**>(&m_pNumTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Hit"), TEXT("Com_Texture_Hit"), reinterpret_cast<CComponent**>(&m_pHitTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Hits"), TEXT("Com_Texture_Hits"), reinterpret_cast<CComponent**>(&m_pSmallHitsTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HitsFX_1"), TEXT("Com_Texture_Fx1"), reinterpret_cast<CComponent**>(&m_pFxTextureCom[0]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HitsFX_2"), TEXT("Com_Texture_Fx2"), reinterpret_cast<CComponent**>(&m_pFxTextureCom[1]))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHit::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	//{
	//	return E_FAIL;
	//}

	_float44 World{};
	XMStoreFloat4x4(&World, m_pTransformCom->Get_World_Matrix() * XMLoadFloat4x4(&m_Rotation));
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", World)))
	{

	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof m_fAlpha)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CHit* CHit::Create(_dev pDevice, _context pContext)
{
	CHit* pInstance = new CHit(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHit::Clone(void* pArg)
{
	CHit* pInstance = new CHit(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHit::Free()
{
	__super::Free();

	Safe_Release(m_pNumTextureCom);
	Safe_Release(m_pHitTextureCom);
	Safe_Release(m_pSmallHitsTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
