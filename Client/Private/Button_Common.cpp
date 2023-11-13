#include "Button_Common.h"
#include "UI_Manager.h"

CButton_Common::CButton_Common(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CButton_Common::CButton_Common(const CButton_Common& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CButton_Common::Init_Prototype()
{
	return S_OK;
}

HRESULT CButton_Common::Init(void* pArg)
{
	if (!pArg)
	{
		return E_FAIL;
	}

	ButtonInfo Info = *reinterpret_cast<ButtonInfo*>(pArg);

	m_fSizeX = 740 * 0.75f;
	m_fSizeY = 90 * 0.75f;

	m_fX = Info.vPos.x;
	m_fY = Info.vPos.y;
	m_fDepth = Info.fDepth;
	m_iButtonType = Info.iButtonType;
	m_strButtonTag = Info.strText;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	m_pUI_Manager->Register_Button(m_pGameInstance->Get_CurrentLevelIndex(), m_strButtonTag);
	
	return S_OK;
}

void CButton_Common::Tick(_float fTimeDelta)
{
	if (m_pUI_Manager->is_Activated())
	{
		m_iIndex = 1;
		m_Color = Colors::Gold;
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
		{
			m_pUI_Manager->Set_ButtonState(LEVEL_LOGO, m_strButtonTag, true);
		}
	}
	else
	{
		m_Color = Colors::White;
		m_iIndex = 0;
	}
}

void CButton_Common::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::UI, this);
}

HRESULT CButton_Common::Render()
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
	m_pGameInstance->Render_Text(TEXT("Font_Malang"), m_strButtonTag, _float2(m_fX, m_fY), 0.7f, m_Color);
	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pShaderCom->Begin(0)))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pVIBufferCom->Render()))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CButton_Common::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_StartBtn"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CButton_Common::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResourceArray(m_pShaderCom, "g_TextureArray")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TexIndex", &m_iIndex, sizeof m_iIndex)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CButton_Common* CButton_Common::Create(_dev pDevice, _context pContext)
{
	CButton_Common* pInstance = new CButton_Common(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CButton_Common");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CButton_Common::Clone(void* pArg)
{
	CButton_Common* pInstance = new CButton_Common(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CButton_Common");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CButton_Common::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
