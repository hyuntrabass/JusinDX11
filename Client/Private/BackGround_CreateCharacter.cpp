#include "BackGround_CreateCharacter.h"
#include "GameInstance.h"

CBackGround_CreateCharacter::CBackGround_CreateCharacter(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CBackGround_CreateCharacter::CBackGround_CreateCharacter(const CBackGround_CreateCharacter& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CBackGround_CreateCharacter::Init_Prototype()
{
	return S_OK;
}

HRESULT CBackGround_CreateCharacter::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY, 1.f);

	return S_OK;
}

void CBackGround_CreateCharacter::Tick(_float fTimeDelta)
{
}

void CBackGround_CreateCharacter::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::Priority, this);
}

HRESULT CBackGround_CreateCharacter::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBackGround_CreateCharacter::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_CREATECHARACTER, TEXT("Prototype_Component_Texture_BackGround_CreateCharacter"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBackGround_CreateCharacter::Bind_ShaderResources()
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

CBackGround_CreateCharacter* CBackGround_CreateCharacter::Create(_dev pDevice, _context pContext)
{
	CBackGround_CreateCharacter* pInstance = new CBackGround_CreateCharacter(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBackGround_CreateCharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround_CreateCharacter::Clone(void* pArg)
{
	CBackGround_CreateCharacter* pInstance = new CBackGround_CreateCharacter(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBackGround_CreateCharacter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround_CreateCharacter::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
