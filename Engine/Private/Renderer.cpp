#include "Renderer.h"
#include "BlendObject.h"
#include "OrthographicObject.h"

CRenderer::CRenderer(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Init_Prototype()
{
	_uint iNumViewPorts{ 1 };

	D3D11_VIEWPORT ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM)))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM)))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
	//{
	//	return E_FAIL;
	//}

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

	m_RenderObject[ToIndex(eRenderGroup)].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	m_RenderObject[RG_Blend].sort([](CGameObject* pSrc, CGameObject* pDst)
	{
		return dynamic_cast<CBlendObject*>(pSrc)->Get_CamDistance() > dynamic_cast<CBlendObject*>(pDst)->Get_CamDistance();
	});

	m_RenderObject[RG_UI].sort([](CGameObject* pSrc, CGameObject* pDst)
	{
		return dynamic_cast<COrthographicObject*>(pSrc)->Get_Depth() > dynamic_cast<COrthographicObject*>(pDst)->Get_Depth();
	});

	for (size_t i = 0; i < RG_End; i++)
	{
		//if (i == RG_NonBlend)
		//{
		//	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		//	{
		//		return E_FAIL;
		//
		//	}
		//}
		for (auto& pGameObject : m_RenderObject[i])
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

		m_RenderObject[i].clear();

		//if (i == RG_NonBlend)
		//{
		//	if (FAILED(m_pGameInstance->End_MRT()))
		//	{
		//		return E_FAIL;
		//	}
		//}
	}

	return S_OK;
}

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

	for (auto& ObjectList : m_RenderObject)
	{
		for (auto& pGameObject : ObjectList)
		{
			Safe_Release(pGameObject);
		}
		ObjectList.clear();
	}
}
