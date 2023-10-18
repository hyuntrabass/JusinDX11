#pragma once
#include "Base.h"

BEGIN(Engine)

class CMyFont final : public CBase
{
private:
	CMyFont(_dev pDevice, _context pContext);
	virtual ~CMyFont() = default;

public:
	HRESULT Init(const wstring& strFilePath);
	HRESULT Render(const wstring& strText);

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	SpriteFont* m_pFont{ nullptr };

public:
	static CMyFont* Create(_dev pDevice, _context pContext, const wstring& strFilePath);
	virtual void Free() override;
};

END