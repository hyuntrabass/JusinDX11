#pragma once
#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(_dev pDevice, _context pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Init();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

private:
	map<const wstring, class CMyFont*> m_Fonts{};

public:
	static CFont_Manager* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END