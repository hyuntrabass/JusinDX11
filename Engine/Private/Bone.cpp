#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Init(ifstream& ModelFile)
{
	return S_OK;
}

CBone* CBone::Create(ifstream& ModelFile)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Init(ModelFile)))
	{
		MSG_BOX("Failed to Create : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBone::Free()
{
}
