#include "FakeWood.h"

CFakeWood::CFakeWood(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CFakeWood::CFakeWood(const CFakeWood& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFakeWood::Init_Prototype()
{
	return S_OK;
}

HRESULT CFakeWood::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (not pArg)
	{
		MSG_BOX("No Argument!");
		return E_FAIL;
	}

	m_vPos = *reinterpret_cast<_float4*>(pArg);

	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&m_vPos));

	m_pTransformCom->Set_RotationPerSec(360.f);

	m_fGravity = 10.f;

	EffectInfo FxInfo{};
	FxInfo.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	FxInfo.fScale = 1.f;
	FxInfo.vPos = m_vPos;
	FxInfo.iType = 70;
	m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Smoke"), &FxInfo);

	return S_OK;
}

void CFakeWood::Tick(_float fTimeDelta)
{
	_vector vPos = XMLoadFloat4(&m_vPos);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vPos += vUp * m_fGravity * fTimeDelta * 0.6f;

	m_fGravity -= 19.8f * fTimeDelta;

	m_pTransformCom->Turn(XMVectorSet(0.f, 0.f, 1.f, 0.f), fTimeDelta);
	m_pTransformCom->Set_State(State::Pos, vPos);

	XMStoreFloat4(&m_vPos, vPos);

	if (m_vPos.y < -10.f)
	{
		m_isDead = true;
	}
}

void CFakeWood::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CFakeWood::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", 0, TextureType::Diffuse)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(StaticPass_Default)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pModelCom->Render(0)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFakeWood::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_FakeWood"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFakeWood::Bind_ShaderResources()
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

	return S_OK;
}

CFakeWood* CFakeWood::Create(_dev pDevice, _context pContext)
{
	CFakeWood* pInstance = new CFakeWood(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CFakeWood");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFakeWood::Clone(void* pArg)
{
	CFakeWood* pInstance = new CFakeWood(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CFakeWood");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFakeWood::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
