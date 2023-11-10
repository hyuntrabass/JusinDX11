#include "Player.h"
#include "PhysX_Manager.h"

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

	m_pModelCom->Set_Animation(0, true);
	//m_pModelCom->Set_Animation(etc_Appearance);

	m_pTransformCom->Set_Speed(5.f);

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_PLAYER);
	//m_pGameInstance->Init_PhysX_MoveableObject(m_pTransformCom);

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CameraModeIndex() == CM_DEBUG)
	{
		return;
	}

	if (m_pGameInstance->Get_CurrentLevelIndex() != LEVEL_CREATECHARACTER)
	{
		if (m_pModelCom->IsAnimationFinished(etc_Exit))
		{
			m_pModelCom->Set_Animation(Idle_Loop, true);
		}
		//m_pGameInstance->Apply_PhysX(m_pTransformCom);

		Move(fTimeDelta);

		//m_pGameInstance->Update_PhysX(m_pTransformCom);
	}
	else
	{
		m_pModelCom->Set_Animation(0, true);
		Customize(fTimeDelta);
		//if (m_pModelCom->IsAnimationFinished(etc_Appearance))
		//{
		//	m_pModelCom->Set_Animation(Idle_Loop, true);
		//}
	}

	m_pModelCom->Play_Animation(fTimeDelta);

}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::NonBlend, this);
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
	GET_CURSOR_POINT(pt);
	_vector Pos = m_pTransformCom->Get_State(State::Pos);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD());
	cout << "Cursor X : " << pt.x << endl;
	cout << "Cursor Y : " << pt.y << endl;
	cout << endl;
	cout << "PlayerPos X :" << Pos.m128_f32[0] << endl;
	cout << "PlayerPos Y :" << Pos.m128_f32[1] << endl;
	cout << "PlayerPos Z :" << Pos.m128_f32[2] << endl;
	cout << endl;
#endif // _DEBUG

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		//{
		//}

		//_float fNorTex = 0.f;
		//if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		//{
		//	fNorTex = 0.f;
		//}
		//else
		//{
		//	fNorTex = 1.f;
		//}

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_fNorTex", &fNorTex, sizeof _float)))
		//{
		//	return E_FAIL;
		//}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(1)))
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
	_bool hasMoved{};
	_vector vCamLook = XMLoadFloat4(&m_pGameInstance->Get_CameraLook());
	vCamLook.m128_f32[1] = 0.f;
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

	if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		m_pTransformCom->Set_Speed(15.f);
		m_isRunning = true;
	}
	else
	{
		m_pTransformCom->Set_Speed(5.f);
		m_isRunning = false;
	}

	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		m_pTransformCom->Jump(10.f);
		if (hasMoved)
		{
			m_pModelCom->Set_Animation(Jump_Front);
		}
		else
		{
			m_pModelCom->Set_Animation(Jump_Vertical);
		}
	}

	_uint iCurrentAnimIndex = m_pModelCom->Get_CurrentAnimationIndex();

	if (hasMoved /*&& iCurrentAnimIndex != Land*/)
	{
		m_pTransformCom->Look_At_Dir(vDirection);
		m_pTransformCom->Go_Straight(fTimeDelta);

		if (!m_pTransformCom->Is_Jumping())
		{
			if (iCurrentAnimIndex != Jump_Front &&
				iCurrentAnimIndex != Jump_Vertical &&
				iCurrentAnimIndex != Fall_Vertical_Loop &&
				iCurrentAnimIndex != Fall_Front_Loop)
			{
				if (m_isRunning)
				{
					m_pModelCom->Set_Animation(Run_Loop, true);
					m_fSliding = 1.f;
				}
				else
				{
					m_pModelCom->Set_Animation(Walk_Loop, true);
				}
			}
		}
	}
	else if (!m_pTransformCom->Is_Jumping())
	{
		iCurrentAnimIndex = m_pModelCom->Get_CurrentAnimationIndex();

		if (iCurrentAnimIndex == Run_Loop || iCurrentAnimIndex == Run_End)
		{
			if (m_pModelCom->IsAnimationFinished(Run_End))
			{
				m_pModelCom->Set_Animation(Idle_Loop, true);
			}
			else
			{
				m_pModelCom->Set_Animation(Run_End);
				m_pTransformCom->Set_Speed(15.f);
				if (m_fSliding > 0.f)
				{
					m_fSliding -= 0.02f;
				}
				m_pTransformCom->Go_Straight(fTimeDelta * m_fSliding);
			}
		}

		if (iCurrentAnimIndex == Walk_Loop || iCurrentAnimIndex == Walk_End)
		{
			if (m_pModelCom->IsAnimationFinished(Walk_End))
			{
				m_pModelCom->Set_Animation(Idle_Loop, true);
			}
			else
			{
				m_pModelCom->Set_Animation(Walk_End);
			}
		}
	}

	iCurrentAnimIndex = m_pModelCom->Get_CurrentAnimationIndex();

	if (iCurrentAnimIndex == Jump_Vertical || iCurrentAnimIndex == Fall_Vertical_Loop)
	{
		if (m_pModelCom->IsAnimationFinished(Jump_Vertical))
		{
			m_pModelCom->Set_Animation(Fall_Vertical_Loop, true);
		}
		if (!m_pTransformCom->Is_Jumping())
		{
			m_pModelCom->Set_Animation(Land);
		}
	}
	else if (iCurrentAnimIndex == Jump_Front || iCurrentAnimIndex == Fall_Front_Loop)
	{
		if (m_pModelCom->IsAnimationFinished(Jump_Front))
		{
			m_pModelCom->Set_Animation(Fall_Front_Loop, true); 
		}
		if (!m_pTransformCom->Is_Jumping())
		{
			m_pModelCom->Set_Animation(Land);
		}
	}
	else if (iCurrentAnimIndex == Land)
	{
		if (m_pModelCom->IsAnimationFinished(Land))
		{
			m_pModelCom->Set_Animation(Idle_Loop, true);
		}
	}

	if (m_pGameInstance->Key_Down(DIK_LCONTROL))
	{
		m_pModelCom->Set_Animation(Attack_Aerial_DownStrike);
	}
	if (m_pModelCom->IsAnimationFinished(Attack_Aerial_DownStrike))
	{
		m_pModelCom->Set_Animation(Land);
	}

	m_pTransformCom->Gravity(fTimeDelta);
}

void CPlayer::Customize(_float fTimeDelta)
{
	if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		_long dwMouseMove;

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -0.3f);
		}
	}

	if (m_pGameInstance->Mouse_Pressing(DIM_MBUTTON))
	{
		_long dwMouseMove;

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -0.3f);
		}
	}

	if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
	{
		_float3 fScale = m_pTransformCom->Get_Scale();
		XMStoreFloat3(&fScale, XMLoadFloat3(&fScale) * 1.25f);
		m_pTransformCom->Set_Scale(fScale);
	}
	else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
	{
		_float3 fScale = m_pTransformCom->Get_Scale();
		XMStoreFloat3(&fScale, XMLoadFloat3(&fScale) * 0.8f);
		m_pTransformCom->Set_Scale(fScale);
	}

	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		m_pModelCom->Set_Animation(etc_Exit);
	}
}

HRESULT CPlayer::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Test"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Pain"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//{
	//	return E_FAIL;
	//}

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

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_CREATECHARACTER, 0);
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
