#pragma once
#include "Base.h"

BEGIN(Engine)

enum class D3DTS
{
	View,
	Proj,
	End
};

class CPipeLine final : public CBase
{
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	_float4 Get_CameraPos() const;
	_float4x4 Get_Transform_Float4x4(D3DTS eState) const;
	_float4x4 Get_Transform_Inversed_Float4x4(D3DTS eState) const;
	_matrix Get_Transform(D3DTS eState) const;
	_matrix Get_Transform_Inversed(D3DTS eState) const;

	void Set_Transform(D3DTS eState, const _float4x4& TransformMatrix);
	void Set_Transform(D3DTS eState, _fmatrix TransformMatrix);

public:
	HRESULT Init();
	void Tick();

private:
	_float4 m_vCameraPos{};
	_float4x4 m_TransformMatrix[ToIndex(D3DTS::End)]{};
	_float4x4 m_TransformMatrix_Inversed[ToIndex(D3DTS::End)]{};

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END