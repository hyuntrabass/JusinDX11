#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_fTickPerSec(rhs.m_fTickPerSec)
	, m_iNumChannels(rhs.m_iNumChannels)
	//, m_Channels(rhs.m_Channels)
	//, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
	//, m_PrevTransformations(rhs.m_PrevTransformations)
{
	//strcpy_s(m_szName, rhs.m_szName);
	//for (auto& pChannel : m_Channels)
	//{
	//	Safe_AddRef(pChannel);
	//}
	for (auto& pPrototypeChannel : rhs.m_Channels)
	{
		CChannel* pChannel = pPrototypeChannel->Clone();

		m_Channels.push_back(pChannel);
	}
}

HRESULT CAnimation::Init(ifstream& ModelFile)
{
	_uint iNameSize{};
	ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
	ModelFile.read(m_szName, iNameSize);

	ModelFile.read(reinterpret_cast<_char*>(&m_fDuration), sizeof _float);
	ModelFile.read(reinterpret_cast<_char*>(&m_fTickPerSec), sizeof _float);

	ModelFile.read(reinterpret_cast<_char*>(&m_iNumChannels), sizeof _uint);

	//m_CurrentKeyFrames.resize(m_iNumChannels);
	//m_PrevTransformations.resize(m_iNumChannels);

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

_bool CAnimation::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fTimeDelta, _bool& isAnimChanged)
{
	if (isAnimChanged)
	{
		if (!m_isInterpolating)
		{
			m_fCurrentAnimPos = 0.f;
			m_isInterpolating = true;
		}

		m_fCurrentAnimPos += fTimeDelta;
	}
	else
	{
		if (m_isInterpolating)
		{
			m_fCurrentAnimPos = 0.f;
			m_isInterpolating = false;
		}

		m_fCurrentAnimPos += m_fTickPerSec * fTimeDelta;
	}

	if (m_fCurrentAnimPos >= m_fDuration)
	{
		m_isFinished = true;
		m_fCurrentAnimPos = 0.f;
	}
	else
	{
		m_isFinished = false;
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update_TransformationMatrix(Bones, m_fCurrentAnimPos, /*m_CurrentKeyFrames[i], m_PrevTransformations[i],*/ isAnimChanged);
	}

	return m_isFinished;
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

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
	{
		Safe_Release(pChannel);
	}
	m_Channels.clear();
}
