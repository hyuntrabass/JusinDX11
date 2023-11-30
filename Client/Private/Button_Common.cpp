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

const _bool& CButton_Common::Is_Pushed() const
{
	return m_isPushed;
}

const _bool CButton_Common::Is_Empty() const
{
	return m_strButtonText.empty();
}

const _bool& CButton_Common::Is_Activated() const
{
	return m_isActivated;
}

void CButton_Common::Push(_bool isPushed)
{
	m_isPushed = isPushed;
}

void CButton_Common::Activate_Button(_bool isActivate)
{
	m_isActivated = isActivate;
}

void CButton_Common::Set_ButtonText(const wstring& strText)
{
	m_strButtonText = strText;
}

HRESULT CButton_Common::Init_Prototype()
{
	return S_OK;
}

HRESULT CButton_Common::Init(void* pArg)
{
	if (not pArg)
	{
		return E_FAIL;
	}

	ButtonInfo Info = *reinterpret_cast<ButtonInfo*>(pArg);

	*Info.ppButton = this;

	m_fX = Info.vPos.x;
	m_fY = Info.vPos.y;
	m_fDepth = Info.fDepth;
	m_iButtonType = Info.iButtonType;
	m_strButtonText = Info.strText;

	switch (m_iButtonType)
	{
	case 0:
		m_fSizeX = 740 * 0.75f;
		m_fSizeY = 90 * 0.75f;
		m_fTextScale = 0.7f;
		break;
	case 1:
		m_fSizeX = 740 * 0.65f;
		m_fSizeY = 90 * 0.65f;
		m_fTextScale = 0.65f;
		break;
	}

	if (FAILED(Add_Components()))
	{
		*Info.ppButton = nullptr;
		return E_FAIL;
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CButton_Common::Tick(_float fTimeDelta)
{
	if (m_isActivated)
	{
		m_iIndex = 1;

		switch (m_iButtonType)
		{
		case 0:
			m_Color = Colors::Gold;
			break;
		case 1:
			m_Color = Colors::Black;
			break;
		}
	}
	else
	{
		switch (m_iButtonType)
		{
		case 0:
			m_Color = Colors::White;
			break;
		case 1:
			m_Color = Colors::Black;
			break;
		}
		m_iIndex = 0;
	}
}

void CButton_Common::Late_Tick(_float fTimeDelta)
{
	m_isPushed = false;
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CButton_Common::Render()
{
	if (m_strButtonText.size() == 0)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	switch (m_iButtonType)
	{
	case 0:
		if (FAILED(m_pShaderCom->Begin(VTPass_Button)))
		{
			return E_FAIL;
		}
		break;
	case 1:
		if (FAILED(m_pShaderCom->Begin(VTPass_UI)))
		{
			return E_FAIL;
		}
		break;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	m_pGameInstance->Render_Text(TEXT("Font_Malang"), m_strButtonText, _float2(m_fX, m_fY), m_fTextScale, m_Color);

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

	switch (m_iButtonType)
	{
	case 0:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StartBtn"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			return E_FAIL;
		}
		break;
	case 1:
		if (FAILED(__super::Add_Component(LEVEL_CREATECHARACTER, TEXT("Prototype_Component_Texture_CustomBtn"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			return E_FAIL;
		}
		break;
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

	switch (m_iButtonType)
	{
	case 0:
		if (FAILED(m_pTextureCom->Bind_ShaderResourceArray(m_pShaderCom, "g_TextureArray")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_TexIndex", &m_iIndex, sizeof m_iIndex)))
		{
			return E_FAIL;
		}
		break;
	case 1:
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iIndex)))
		{
			return E_FAIL;
		}
		break;
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
