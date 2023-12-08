#include "Renderer.h"
#include "BlendObject.h"
#include "OrthographicObject.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"

CRenderer::CRenderer(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Init_Prototype()
{
	_uint iNumViewPorts{ 1 };

	D3D11_VIEWPORT ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
	{
		return E_FAIL;
	}

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (not m_pVIBuffer)
	{
		return E_FAIL;
	}

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (not m_pShader)
	{
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Diffuse"), _float2(50.f, 50.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Normal"), _float2(50.f, 150.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Depth"), _float2(50.f, 250.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Shade"), _float2(150.f, 50.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Specular"), _float2(150.f, 150.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

HRESULT CRenderer::Init(void* pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RenderGroup eRenderGroup, CGameObject* pRenderObject)
{
	if (!pRenderObject)
	{
		MSG_BOX("pRenderObject is NULL");
		return E_FAIL;
	}

	m_RenderObjects[ToIndex(eRenderGroup)].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Render_Priority()))
	{
		MSG_BOX("Failed to Render : Priority");
		return E_FAIL;
	}
	if (FAILED(Render_NonBlend()))
	{
		MSG_BOX("Failed to Render : NonBlend");
		return E_FAIL;
	}
	if (FAILED(Render_LightAcc()))
	{
		MSG_BOX("Failed to Render : LightAcc");
		return E_FAIL;
	}
	if (FAILED(Render_Deferred()))
	{
		MSG_BOX("Failed to Render : Deferred");
		return E_FAIL;
	}
	if (FAILED(Render_NonLight()))
	{
		MSG_BOX("Failed to Render : NonLight");
		return E_FAIL;
	}
	if (FAILED(Render_Blend()))
	{
		MSG_BOX("Failed to Render : Blend");
		return E_FAIL;
	}
	if (FAILED(Render_UI()))
	{
		MSG_BOX("Failed to Render : UI");
		return E_FAIL;
	}

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
	{
		MSG_BOX("Failed to Render Debug");
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugComponent)
{
	m_DebugComponents.push_back(pDebugComponent);
	Safe_AddRef(pDebugComponent);

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RG_Priority])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Priority].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
	{
		return E_FAIL;
	}

	for (auto& pGameObject : m_RenderObjects[RG_NonBlend])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_NonBlend].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Lights"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_NormalTexture", TEXT("Target_Normal"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraFar(), sizeof _float)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inversed_Float4x4(TransformType::View))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inversed_Float4x4(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Lights(m_pGameInstance->Get_CurrentLevelIndex(), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_ShadeTexture", TEXT("Target_Shade"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Begin(DefPass_Deferred)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RG_NonLight])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_NonLight].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	m_RenderObjects[RG_Blend].sort([](CGameObject* pSrc, CGameObject* pDst)
	{
		return dynamic_cast<CBlendObject*>(pSrc)->Get_CamDistance() > dynamic_cast<CBlendObject*>(pDst)->Get_CamDistance();
	});

	for (auto& pGameObject : m_RenderObjects[RG_Blend])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Blend].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	m_RenderObjects[RG_UI].sort([](CGameObject* pSrc, CGameObject* pDst)
	{
		return dynamic_cast<COrthographicObject*>(pSrc)->Get_Depth() > dynamic_cast<COrthographicObject*>(pDst)->Get_Depth();
	});

	for (auto& pGameObject : m_RenderObjects[RG_UI])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_UI].clear();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& pComponent : m_DebugComponents)
	{
		pComponent->Render();
		Safe_Release(pComponent);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	return S_OK;
}
#endif // _DEBUG


CRenderer* CRenderer::Create(_dev pDevice, _context pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
		{
			Safe_Release(pGameObject);
		}
		ObjectList.clear();
	}
}