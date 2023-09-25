#include "Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(_dev pDevice, _context pContext)
    : CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Init_Prototype()
{
    return S_OK;
}

HRESULT CRenderer::Init(void* pArg)
{
    return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RenderGroup eRenderGroup, CGameObject* pRenderObject)
{
    if (!pRenderObject)
    {
        MSG_BOX("pRenderObject is NULL");
        return E_FAIL;
    }

    m_RenderObject[ToIndex(eRenderGroup)].push_back(pRenderObject);

    Safe_AddRef(pRenderObject);

    return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
    for (size_t i = 0; i < ToIndex(RenderGroup::End); i++)
    {
        if (i == ToIndex(RenderGroup::Blend))
        {
            // sort
        }
        for (auto& pGameObject : m_RenderObject[i])
        {
            if (pGameObject)
            {
                pGameObject->Render();
            }
            
            Safe_Release(pGameObject);
        }

        m_RenderObject[i].clear();
    }

    return S_OK;
}

CRenderer* CRenderer::Create(_dev pDevice, _context pContext)
{
    CRenderer* pInstance = new CRenderer(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype()))
    {
        MSG_BOX("Failed to Create : CRenderer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
    AddRef();

    return this;
}

void CRenderer::Free()
{
    __super::Free();

    for (auto& ObjectList : m_RenderObject)
    {
        for (auto& pGameObject : ObjectList)
        {
            Safe_Release(pGameObject);
        }
        ObjectList.clear();
    }
}
