#include "Kakashi.h"
#include "UI_Manager.h"
#include "Trigger_Manager.h"

CKakashi::CKakashi(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CKakashi::CKakashi(const CKakashi& rhs)
	: CGameObject(rhs)
{
}

HRESULT CKakashi::Init_Prototype()
{
	return S_OK;
}

HRESULT CKakashi::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, XMVectorSet(11.4f, 30.8f, 118.8f, 1.f));
	m_pTransformCom->LookAt_Dir(XMVectorSet(0.96f, 0.f, 0.27785f, 0.f));

	m_pUIManager = CUI_Manager::Get_Instance();
	m_pTriggerManager = CTrigger_Manager::Get_Instance();

	return S_OK;
}

void CKakashi::Tick(_float fTimeDelta)
{
	if (m_fTimer > 2.f and m_iSceneNumber == 0)
	{
		ANIM_DESC Anim{};
		Anim.iAnimIndex = Kakashi_etc_Appearance;
		m_pModelCom->Set_Animation(Anim);
		m_iSceneNumber++;

		m_pGameInstance->StopSound(SCH_JUMP);
		m_pGameInstance->Play_Sound(TEXT("Jump"), SCH_JUMP, 0.4f);
	}

	if (m_pModelCom->IsAnimationFinished(Kakashi_etc_Appearance))
	{
		ANIM_DESC Anim{};
		Anim.iAnimIndex = Kakashi_etc_Tutorial_Loop;
		Anim.isLoop = true;
		m_pModelCom->Set_Animation(Anim);

	}

	if (m_iSceneNumber == 1 and m_fTimer > 3.f)
	{
		m_pGameInstance->StopSound(SCH_NPC);
		m_pGameInstance->Play_Sound(TEXT("SecondMap_Kakashi_Dialog1"), SCH_NPC);
		m_pUIManager->Set_SubTitle(TEXT("지금부터 기초 훈련을 시작한다."));

		m_fTimer = {};
		m_iSceneNumber++;
	}

	if (m_iSceneNumber == 2 and m_fTimer > 5.f)
	{
		m_pGameInstance->StopSound(SCH_NPC);
		m_pGameInstance->Play_Sound(TEXT("SecondMap_KakashiRemote_Dialog0"), SCH_NPC);
		m_pUIManager->Set_SubTitle(TEXT("일단은 기본적인 움직임을 보자. 준비가 되면 오른쪽의 언덕을 딛고 날 따라와."));

		m_fTimer = {};
		m_iSceneNumber++;
	}

	if (m_iSceneNumber == 3 and not m_pTriggerManager->Hasto_PlayScene())
	{
		ANIM_DESC Anim{};
		Anim.iAnimIndex = RecordedSequence_NN_Pawn_Player_Kakashi;
		m_pModelCom->Set_Animation(Anim);
		m_pTransformCom->Set_State(State::Pos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pTransformCom->LookAt_Dir(XMVectorSet(0.f, 0.f, 1.f, 0.f));
		m_pUIManager->Set_SubTitle(TEXT(""));

		m_iSceneNumber++;
		m_fTimer = {};
	}

	if (m_iSceneNumber == 4 and m_pTriggerManager->Is_TriggerPulled(LEVEL_FOREST, 1))
	{
		ANIM_DESC Anim{};
		Anim.iAnimIndex = Kakashi_etc_Tutorial_Loop;
		Anim.isLoop = true;
		m_pModelCom->Set_Animation(Anim);
		m_pTransformCom->Set_State(State::Pos, XMVectorSet(-15.8f, 51.6f, 102.0f, 1.f));
		m_pTransformCom->LookAt_Dir(XMVectorSet(0.96f, 0.f, 0.27785f, 0.f));

		m_iSceneNumber++;
		m_fTimer = {};
	}

	if (m_iSceneNumber == 5 and m_fTimer > 1.f)
	{
		m_pGameInstance->StopSound(SCH_NPC);
		m_pGameInstance->Play_Sound(TEXT("FirstMap_Kakashi_Dialog2"), SCH_NPC);
		m_pUIManager->Set_SubTitle(TEXT("좋아. 기본적인 차크라 컨트롤은 되는구만."));

		m_iSceneNumber++;
		m_fTimer = {};
	}

	if (m_iSceneNumber == 6 and m_fTimer > 4.f)
	{
		m_pGameInstance->StopSound(SCH_NPC);
		m_pGameInstance->Play_Sound(TEXT("FristMap_KakashiRemote0"), SCH_NPC);
		m_pUIManager->Set_SubTitle(TEXT("미안하지만 내가 좀 바빠서 가야겠다. 나머지 훈련은 알아서 해라! 미안!"));

		m_iSceneNumber++;
		m_fTimer = {};
	}

	if (m_iSceneNumber == 7 and not m_pTriggerManager->Hasto_PlayScene())
	{
		m_pUIManager->Set_SubTitle(TEXT(""));

		m_iSceneNumber++;
		m_fTimer = {};
	}

	if (m_pGameInstance->Key_Down(DIK_F2))
	{
		ANIM_DESC Anim{};
		Anim.iAnimIndex = RecordedSequence_NN_Pawn_Player_Kakashi_1;
		m_pModelCom->Set_Animation(Anim);
		m_pTransformCom->Set_State(State::Pos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pTransformCom->LookAt_Dir(XMVectorSet(0.f, 0.f, 1.f, 0.f));
	}
	if (m_pGameInstance->Key_Down(DIK_F3))
	{
		ANIM_DESC Anim{};
		Anim.iAnimIndex = RecordedSequence_NN_Pawn_Player_Kakashi_7;
		m_pModelCom->Set_Animation(Anim);
		m_pTransformCom->Set_State(State::Pos, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_pTransformCom->LookAt_Dir(XMVectorSet(0.f, 0.f, 1.f, 0.f));
	}

	if (m_pModelCom->IsAnimationFinished(RecordedSequence_NN_Pawn_Player_Kakashi_1))
	{
		ANIM_DESC Anim{};
		Anim.iAnimIndex = Kakashi_etc_Tutorial_Loop;
		Anim.isLoop = true;
		m_pModelCom->Set_Animation(Anim);
		m_pTransformCom->Set_State(State::Pos, XMVectorSet(11.56f, 40.282f, -65.7948f, 1.f));
		m_pTransformCom->LookAt_Dir(XMVectorSet(-0.56f, 0.f, 0.82357f, 0.f));
	}

	m_fTimer += fTimeDelta;
	if (m_iSceneNumber)
	{
		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CKakashi::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
}

HRESULT CKakashi::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		//if (FAILED(m_pShaderCom->Begin(AnimPass_OutLine)))
		//{
		//	return E_FAIL;
		//}

		//if (FAILED(m_pModelCom->Render(i)))
		//{
		//	return E_FAIL;
		//}

		if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
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

HRESULT CKakashi::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Kakashi"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CKakashi::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CKakashi* CKakashi::Create(_dev pDevice, _context pContext)
{
	CKakashi* pInstance = new CKakashi(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CKakashi");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKakashi::Clone(void* pArg)
{
	CKakashi* pInstance = new CKakashi(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CKakashi");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKakashi::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
