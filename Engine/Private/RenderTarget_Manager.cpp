#include "RenderTarget_Manager.h"
#include "RenderTarget.h"

CRenderTarget_Manager::CRenderTarget_Manager(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget_Manager::Init()
{


	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat)
{
	if (Find_RenderTarget(strTargetTag))
	{
		MSG_BOX("Target Tag Already Exist!");
		return E_FAIL;
	}

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat);
	if (not pRenderTarget)
	{
		return E_FAIL;
	}

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (not pRenderTarget)
	{
		MSG_BOX("Can't Find RenderTarget!");
		return E_FAIL;
	}

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	if (pMRTList)
	{
		pMRTList->push_back(pRenderTarget);
	}
	else
	{
		list<CRenderTarget*> MRTList{};
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(const wstring& strMRTTag)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	if (not pMRTList)
	{
		MSG_BOX("Can't Find MRT List!");
		return E_FAIL;
	}

	m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pMainDepthStencilView);

	ID3D11RenderTargetView* pRTVs[8]{};

	_uint iNumViews{};

	for (auto& pRenderTarget : *pMRTList)
	{
		pRTVs[iNumViews++] = pRenderTarget->Get_RenderTargetView();
	}

	m_pContext->OMSetRenderTargets(iNumViews, pRTVs, m_pMainDepthStencilView);

	return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pMainDepthStencilView);

	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pMainDepthStencilView);

	return S_OK;
}

CRenderTarget* CRenderTarget_Manager::Find_RenderTarget(const wstring& strTargetTag)
{
	auto iter = m_RenderTargets.find(strTargetTag);
	if (iter == m_RenderTargets.end())
	{
		return nullptr;
	}

	return iter->second;
}

list<class CRenderTarget*>* CRenderTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
	auto iter = m_MRTs.find(strMRTTag);
	if (iter == m_MRTs.end())
	{
		return nullptr;
	}

	return &iter->second;
}

CRenderTarget_Manager* CRenderTarget_Manager::Create(_dev pDevice, _context pContext)
{
	CRenderTarget_Manager* pInstance = new CRenderTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CRenderTarget_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget_Manager::Free()
{
	for (auto& pair : m_MRTs)
	{
		for (auto& pRenderTargets : pair.second)
		{
			Safe_Release(pRenderTargets);
		}
		pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& pair : m_RenderTargets)
	{
		Safe_Release(pair.second);
	}
	m_RenderTargets.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
