#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(_dev pDevice, _context pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	HRESULT Init_Prototype(const string& strFilePath);
	HRESULT Init(void* pArg) override;

public:
	HRESULT Render();

private:
	_uint m_iNumMeshes{};
	vector<class CStatic_Mesh*> m_Meshes{};

public:
	static CModel* Create(_dev pDevice, _context pContext, const string& strFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END