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
}
