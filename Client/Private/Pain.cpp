#include "Pain.h"

CPain::CPain(_dev pDevice, _context pContext)
    : CGameObject(pDevice, pContext)
{
}

CPain::CPain(const CPain& rhs)
    : CGameObject(rhs)
{
}

HRESULT CPain::Init_Prototype()
{
    return S_OK;
}

HRESULT CPain::Init(void* pArg)
{
    if (FAILED(Add_Components()))
    {
        return E_FAIL;
    }

    m_pTransformCom->Set_State(State::Pos, XMVectorSet(100.f, 45.f, -75.f, 1.f));

    m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER);
    m_pGameInstance->Register_CollisionObject(this, m_pCollider_Hit);

    m_AnimationDesc.fAnimSpeedRatio = 3.f;
    m_eState = State_Idle;

    m_iHP = 300;

    return S_OK;
}

void CPain::Tick(_float fTimeDelta)
{
    Artificial_Intelligence(fTimeDelta);

    Init_State();
    TIck_State(fTimeDelta);

    if (m_pModelCom->IsAnimationFinished(m_AnimationDesc.iAnimIndex))
    {
        m_AnimationDesc.iAnimIndex++;
        if (m_AnimationDesc.iAnimIndex >= Anim_End)
        {
            m_AnimationDesc.iAnimIndex = 0;
        }
        m_pModelCom->Set_Animation(m_AnimationDesc);
    }


    m_pTransformCom->Gravity(fTimeDelta);

    _matrix ColliderOffset = XMMatrixTranslation(0.f, 0.8f, 0.f);
    m_pCollider_Hit->Update(/*ColliderOffset * */m_pTransformCom->Get_World_Matrix());
    m_fTimer += fTimeDelta;
}

void CPain::Late_Tick(_float fTimeDelta)
{
    m_pCollider_Hit->Intersect(reinterpret_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Collider_Attack"))));

    m_pModelCom->Play_Animation(fTimeDelta);

    if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 20.f) and m_eState != State_None)
    {
        m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

    #ifdef _DEBUG
        m_pRendererCom->Add_DebugComponent(m_pCollider_Hit);
    #endif // _DEBUG
    }
}

HRESULT CPain::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        return E_FAIL;
    }

    _bool hasNormal{};
    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
        {

        }

        if (SUCCEEDED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
        {
            hasNormal = true;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &hasNormal, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Begin(AnimPass_OutLine)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pModelCom->Render(i)))
        {
            return E_FAIL;
        }

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

void CPain::Set_Damage(_int iDamage)
{
    m_iHP -= iDamage;

    m_eState = State_Beaten;
}

HRESULT CPain::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STAGE1, TEXT("Prototype_Model_Pain"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        return E_FAIL;
    }

    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
    Safe_AddRef(m_pPlayerTransform);

    Collider_Desc ColDesc{};
    ColDesc.eType = ColliderType::AABB;
    ColDesc.vExtents = _float3(0.35f, 0.75f, 0.35f);
    ColDesc.vCenter = _float3(0.f, ColDesc.vExtents.y, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Take"), reinterpret_cast<CComponent**>(&m_pCollider_Hit), &ColDesc)))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CPain::Bind_ShaderResources()
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraFar(), sizeof _float)))
    {
        return E_FAIL;
    }

    return S_OK;
}

void CPain::Artificial_Intelligence(_float fTimeDelta)
{
    _vector vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
    _vector vMyPos = m_pTransformCom->Get_State(State::Pos);
    _float fDistToPlayer = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));

}

void CPain::Init_State()
{
    if (m_eState != m_ePrevState)
    {
        switch (m_eState)
        {
        case Client::CPain::State_None:
            break;
        case Client::CPain::State_Initiation:
            break;
        case Client::CPain::State_Idle:
            break;
        case Client::CPain::State_LookAt:
            break;
        case Client::CPain::State_ComboAttack:
            break;
        case Client::CPain::State_Push:
            break;
        case Client::CPain::State_Pull:
            break;
        case Client::CPain::State_Sommon:
            break;
        case Client::CPain::State_Beaten:
            break;
        case Client::CPain::State_Die:
            break;
        default:
            break;
        }
    }
}

void CPain::TIck_State(_float fTimeDelta)
{
    //m_AnimationDesc = {};
    switch (m_eState)
    {
    case Client::CPain::State_None:
        break;
    case Client::CPain::State_Initiation:
        break;
    case Client::CPain::State_Idle:
        break;
    case Client::CPain::State_LookAt:
        break;
    case Client::CPain::State_ComboAttack:
        break;
    case Client::CPain::State_Push:
        break;
    case Client::CPain::State_Pull:
        break;
    case Client::CPain::State_Sommon:
        break;
    case Client::CPain::State_Beaten:
        break;
    case Client::CPain::State_Die:
        break;
    default:
        break;
    }
}

CPain* CPain::Create(_dev pDevice, _context pContext)
{
    CPain* pInstance = new CPain(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype()))
    {
        MSG_BOX("Failed to Create : CPain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPain::Clone(void* pArg)
{
    CPain* pInstance = new CPain(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Clone : CPain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPain::Free()
{
    __super::Free();

    Safe_Release(m_pCollider_Hit);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pPlayerTransform);
}
