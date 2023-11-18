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

const _bool& CAnimation::IsFinished()
{
	return m_isFinished;
}

void CAnimation::ResetFinished()
{
	m_isFinished = false;
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

void CAnimation::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fTimeDelta, _bool& isAnimChanged, const _bool& isLoop, const _bool& bSkipInterpolation)
{
	if (isAnimChanged)
	{
		if (bSkipInterpolation)
		{
			m_fCurrentAnimPos = 0.f;
			isAnimChanged = false;
		}
		else if (!m_isInterpolating)
		{
			m_fCurrentAnimPos = 0.f;
			m_isInterpolating = true;
		}
		else
		{
			m_fCurrentAnimPos += fTimeDelta;
		}
	}
	else
	{
		if (m_isInterpolating)
		{
			m_isInterpolating = false;
		}
		m_fCurrentAnimPos += m_fTickPerSec * fTimeDelta;

		if (m_fCurrentAnimPos >= m_fDuration)
		{
			if (isLoop)
			{
				m_fCurrentAnimPos = 0.f;
			}
			else
			{
				m_isFinished = true;
			}
		}
		else
		{
			m_isFinished = false;
		}
	}

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Update_TransformationMatrix(Bones, m_fCurrentAnimPos, isAnimChanged);
	}
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
