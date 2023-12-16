#pragma once

namespace Engine
{
	// 이넘 클래스를 인덱스로 쓸 수 있게 uint로 캐스팅 해주는 함수
	template<class T>
	inline constexpr unsigned int ToIndex(const T & ENUM)
	{
		return static_cast<unsigned int>(ENUM);
	}

	template<typename T>
	void Safe_Delete(T & Temp)
	{
		if (Temp)
		{
			delete Temp;
			Temp = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_AddRef(T & Temp)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != Temp)
		{
			dwRefCnt = Temp->AddRef();
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_Release(T & Temp)
	{
		unsigned long dwRefCnt = 0;

		if (Temp)
		{
			dwRefCnt = Temp->Release();
			if (0 == dwRefCnt)
				Temp = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	void Safe_Delete_Array(T & Temp)
	{
		if (Temp)
		{
			delete[] Temp;
			Temp = nullptr;
		}
	}

	template<typename T>
	T Lerp(T Src, T Dst, float Alpha)
	{
		if (Alpha < 0.f)
		{
			Alpha = 0.f;
		}
		if (Alpha > 1.f)
		{
			Alpha = 1.f;
		}
		return Src * (1.f - Alpha) + Dst * Alpha;
	}

	const PxVec3 ENGINE_DLL VectorToPxVec3(FXMVECTOR vVector);
	const XMVECTOR ENGINE_DLL PxExVec3ToVector(PxExtendedVec3 Src, float w = 0.f);
	const PxVec3 ENGINE_DLL PxExVec3ToPxVec3(PxExtendedVec3 Src);
	const XMVECTOR ENGINE_DLL PxVec3ToVector(PxVec3 Src, float w = 0.f);
	const PxExtendedVec3 ENGINE_DLL PxVec3ToPxExVec3(PxVec3 Src);
}
