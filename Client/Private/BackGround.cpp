#include "BackGround.h"
#include "GameInstance.h"

CBackGround::CBackGround(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBackGround::CBackGround(const CBackGround& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackGround::Init_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CBackGround::Tick(_float fTimeDelta)
{
	if (GetKeyState('S') & 0x8000)
	{
		m_iIndex = 1;
	}
	else if (GetKeyState('A') & 0x8000)
	{
		m_iIndex = 2;
	}
	else
	{
		m_iIndex = 0;
	}

	if (GetKeyState(VK_UP) & 0x8000)
	{
		m_pTranformCom->Go_Straight(fTimeDelta);
	}

	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		m_pTranformCom->Go_Backward(fTimeDelta);
	}

	if (GetKeyState('U') & 0x8000)
	{
		m_pTranformCom->Go_Up(fTimeDelta);
	}

	if (GetKeyState('J') & 0x8000)
	{
		m_pTranformCom->Go_Down(fTimeDelta);
	}

	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		m_pTranformCom->Turn(XMVectorSet(0.f, -1.f, 0.f, 0.f), fTimeDelta);
	}

	if (GetKeyState(VK_RIGHT) & 0x8000)
	{
		m_pTranformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}
}

void CBackGround::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::Priority, this);
}

HRESULT CBackGround::Render()
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

HRESULT CBackGround::Add_Components()
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

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Logo), TEXT("Prototype_Component_Texture_BackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		return E_FAIL;
	}

	CTransform::TRANSFORM_DESC Transform_Desc{};
	Transform_Desc.fSpeedPerSec = 10.f;
	Transform_Desc.fRotationPerSec = XMConvertToRadians(120.f);

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTranformCom, &Transform_Desc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBackGround::Bind_ShaderResources()
{
	_float4x4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, -5.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(30.f), static_cast<_float>(g_iWinSizeX) / g_iWinSizeY, 0.1f, 300.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTranformCom->Get_World_float4x4())))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_TexIndex", &m_iIndex, sizeof m_iIndex)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CBackGround* CBackGround::Create(_dev pDevice, _context pContext)
{
	CBackGround* pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
