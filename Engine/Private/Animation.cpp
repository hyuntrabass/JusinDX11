#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Init(ifstream& ModelFile)
{
    _uint iNameSize{};
    ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
    ModelFile.read(m_szName, iNameSize);

    ModelFile.read(reinterpret_cast<_char*>(&m_fDuration), sizeof _float);
    ModelFile.read(reinterpret_cast<_char*>(&m_fTickPerSec), sizeof _float);

    ModelFile.read(reinterpret_cast<_char*>(&m_iNumChannels), sizeof _uint);
    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(ModelFile);
        if (!pChannel)
        {
            return E_FAIL;
        }
        m_Channels.push_back(pChannel);
    }

    return S_OK;
}

CAnimation* CAnimation::Create(ifstream& ModelFile)
{
    CAnimation* pInstance = new CAnimation();

    if (FAILED(pInstance->Init(ModelFile)))
    {
        MSG_BOX("Failed to Create : CAnimation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnimation::Free()
{
}
