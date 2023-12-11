#include "3D_Cursor.h"
#include "ImguiMgr.h"

C3D_Cursor::C3D_Cursor(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

C3D_Cursor::C3D_Cursor(const C3D_Cursor& rhs)
	: CGameObject(rhs)
{
}

void C3D_Cursor::Set_Position(_float3 vPos)
{
	m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
}

HRESULT C3D_Cursor::Init_Prototype()
{
	return S_OK;
}

HRESULT C3D_Cursor::Init(void* pArg)
{
	m_pImgui_Manager = reinterpret_cast<CImguiMgr*>(pArg);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void C3D_Cursor::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&m_pImgui_Manager->Get_Position()));
}

void C3D_Cursor::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
}

HRESULT C3D_Cursor::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_COLMesh)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Render(0)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT C3D_Cursor::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sphere"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT C3D_Cursor::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
	{
		return E_FAIL;
	}

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

	_float4 vColor{ 0.8f, 0.f, 0.f, 0.5f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _float4)))
	{
		return E_FAIL;
	}

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));
	if (!pLightDesc)
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof _float4)))
	{
		return E_FAIL;
	}

	return S_OK;
}

C3D_Cursor* C3D_Cursor::Create(_dev pDevice, _context pContext)
{
	C3D_Cursor* pInstance = new C3D_Cursor(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : C3D_Cursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* C3D_Cursor::Clone(void* pArg)
{
	C3D_Cursor* pInstance = new C3D_Cursor(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : C3D_Cursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C3D_Cursor::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pImgui_Manager);
}
