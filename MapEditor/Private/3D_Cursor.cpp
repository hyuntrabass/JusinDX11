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

HRESULT C3D_Cursor::Init_Prototype()
{
	return S_OK;
}

HRESULT C3D_Cursor::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void C3D_Cursor::Tick(_float fTimeDelta)
{
}

void C3D_Cursor::Late_Tick(_float fTimeDelta)
{
}

HRESULT C3D_Cursor::Render()
{
	return S_OK;
}

HRESULT C3D_Cursor::Add_Components()
{
	return E_NOTIMPL;
}

HRESULT C3D_Cursor::Bind_ShaderResources()
{
	return E_NOTIMPL;
}

C3D_Cursor* C3D_Cursor::Create(_dev pDevice, _context pContext)
{
	return nullptr;
}

CGameObject* C3D_Cursor::Clone(void* pArg)
{
	return nullptr;
}

void C3D_Cursor::Free()
{
}
