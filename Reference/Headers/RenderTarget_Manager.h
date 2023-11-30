#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderTarget_Manager final : public CBase
{
private:
	CRenderTarget_Manager(_dev pDevice, _context pContext);
	virtual ~CRenderTarget_Manager() = default;

public:
	HRESULT Init();
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);

	HRESULT Begin_MRT(const wstring& strMRTTag);
	HRESULT End_MRT();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

private:
	ID3D11RenderTargetView* m_pBackBufferRTV{ nullptr };
	ID3D11DepthStencilView* m_pMainDepthStencilView{ nullptr };

	map<const wstring, class CRenderTarget*> m_RenderTargets{};
	map<const wstring, list<class CRenderTarget*>> m_MRTs{};

private:
	class CRenderTarget* Find_RenderTarget(const wstring& strTargetTag);
	list<class CRenderTarget*>* Find_MRT(const wstring& strMRTTag);

public:
	static CRenderTarget_Manager* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END