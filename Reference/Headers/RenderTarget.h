#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(_dev pDevice, _context pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RenderTargetView();

public:
	HRESULT Init(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat);

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

	ID3D11Texture2D* m_pTexture{ nullptr };
	ID3D11RenderTargetView* m_pRenderTargetView{ nullptr };
	ID3D11ShaderResourceView* m_pShaderResourceView{ nullptr };

public:
	static CRenderTarget* Create(_dev pDevice, _context pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat);
	virtual void Free() override;
};

END