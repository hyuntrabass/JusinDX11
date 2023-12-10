#include "EyeLight.h"

CEyeLight::CEyeLight(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CEyeLight::CEyeLight(const CEyeLight& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CEyeLight::Init_Prototype()
{
	return S_OK;
}

HRESULT CEyeLight::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CEyeLight::Tick(_float3 vPos, _float fTimeDelta)
{
	if (m_TrailPosList.size() >= 20)
	{
		m_TrailPosList.pop_back();
	}
	m_TrailPosList.push_front(vPos);

	m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
}

void CEyeLight::Late_Tick(_float fTimeDelta)
{
	_float3 PosArray[20]{};
	_float4 ColorArray[20]{};

	for (size_t i = 0; i < 20; i++)
	{
		XMStoreFloat3(&PosArray[i], m_pTransformCom->Get_State(State::Pos));
		ColorArray[i] = _float4(1.f, 0.f, 0.f, 1.f - static_cast<_float>(i) / 20.f);
	}

	_uint iIndex{};
	for (auto& vPos : m_TrailPosList)
	{
		PosArray[iIndex++] = vPos;
	}

	m_pTrailBufferCom->Update(PosArray, ColorArray);

	__super::Compute_CamDistance();
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
}

HRESULT CEyeLight::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(0)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEyeLight::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail_20"), TEXT("Com_TrailBuffer"), reinterpret_cast<CComponent**>(&m_pTrailBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Trail"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEyeLight::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CEyeLight* CEyeLight::Create(_dev pDevice, _context pContext)
{
	CEyeLight* pInstnace = new CEyeLight(pDevice, pContext);

	if (FAILED(pInstnace->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEyeLight");
		Safe_Release(pInstnace);
	}

	return pInstnace;
}

CGameObject* CEyeLight::Clone(void* pArg)
{
	CEyeLight* pInstance = new CEyeLight(*this);

	if (FAILED(pInstance->Init((pArg))))
	{
		MSG_BOX("Failed to Clone : CEyeLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEyeLight::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTrailBufferCom);
	Safe_Release(m_pShaderCom);
}
