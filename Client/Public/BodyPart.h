#pragma once
#include "Client_Define.h"
#include "PartObject.h"

BEGIN(Client)

enum PART_TYPE
{
	PT_HEAD,
	PT_FACE,
	PT_UPPER_BODY,
	PT_LOWER_BODY,
	PT_END
};

struct BODYPART_DESC
{
	PART_TYPE eType{};
	_uint iNumVariations{};
	pair<_uint, _bool>* Animation{};

	CTransform* pParentTransform{ nullptr };
};

class CBodyPart final : public CPartObject
{
private:
	CBodyPart(_dev pDevice, _context pContext);
	CBodyPart(const CBodyPart& rhs);
	virtual ~CBodyPart() = default;

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_float fTimeDelta) override;
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

public:
	_bool IsAnimationFinished(_uint iAnimIndex);
	_uint Get_CurrentAnimationIndex();

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	vector<CModel*> m_Models{ nullptr };

private:
	PART_TYPE m_eType{};
	_uint m_iNumVariations{};
	_uint m_iSelectedModelIndex{};
	pair<_uint, _bool>* m_Animation{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBodyPart* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END