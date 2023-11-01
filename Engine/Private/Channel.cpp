#include "Channel.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Init(ifstream& ModelFile)
{
	_uint iNameSize{};
	ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
	ModelFile.read(m_szName, iNameSize);

	ModelFile.read(reinterpret_cast<_char*>(&m_iNumkeyFrames), sizeof _uint);

	KEYFRAME KeyFrame{};

	for (size_t i = 0; i < m_iNumkeyFrames; i++)
	{
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.vScale), sizeof _float4);
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.vRotation), sizeof _float4);
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.vPosition), sizeof _float4);
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.fTime), sizeof _float);

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

CChannel* CChannel::Create(ifstream& ModelFile)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Init(ModelFile)))
	{
		MSG_BOX("Failed to Create : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
}
