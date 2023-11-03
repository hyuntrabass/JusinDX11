#include "Player.h"

CPlayer::CPlayer(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Init_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pModelCom->Set_Animation(0);

	m_pTransformCom->Set_Speed(5.f);

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	Move(fTimeDelta);
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::NonBlend, this);
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
		}

		_float fNorTex = 0.f;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			fNorTex = 0.f;
		}
		else
		{
			fNorTex = 1.f;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fNorTex", &fNorTex, sizeof _float)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(0)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

void CPlayer::Move(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CameraModeIndex() == ToIndex(CameraMode::Debug))
	{
		return;
	}
	_bool hasMoved{};
	_vector vCamLook = XMLoadFloat4(&m_pGameInstance->Get_CameraLook());
	_vector vCamRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vCamLook);
	_vector vDirection{};

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		vDirection += vCamLook;
		hasMoved = true;
	}
	else if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		vDirection -= vCamLook;
		hasMoved = true;
	}
	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		vDirection += vCamRight;
		hasMoved = true;
	}
	else if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		vDirection -= vCamRight;
		hasMoved = true;
	}

	if (hasMoved)
	{
		m_pTransformCom->Look_At_Dir(vDirection);
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_pModelCom->Set_Animation(2);
	}
	else
	{
		m_pModelCom->Set_Animation(0);
	}
}

HRESULT CPlayer::Add_Components()
{
	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		return E_FAIL;
	}
	
	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(ToIndex(Level_ID::Static), TEXT("Prototype_Model_Pain"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(ToIndex(Level_ID::CreateCharacter), 0);
	if (!pLightDesc)
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof _float4)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CPlayer* CPlayer::Create(_dev pDevice, _context pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
