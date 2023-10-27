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
	const _uint& Get_NumMeshes() const;

public:
	HRESULT Init_Prototype(const string& strFilePath, _fmatrix OffsetMatrix);
	HRESULT Init(void* pArg) override;

public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType);
	HRESULT Render(_uint iMeshIndex);
	_bool Intersect_RayModel(_fmatrix WorldMatrix, _float4* pPickPos);

private:
	_uint m_iNumMeshes{};
	vector<class CMesh*> m_Meshes{};

	_uint m_iNumMaterials{};
	vector<Model_Material> m_Materials{};

public:
	static CModel* Create(_dev pDevice, _context pContext, const string& strFilePath, _fmatrix OffsetMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END