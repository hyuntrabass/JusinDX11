#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	struct ParticleDesc
	{
		_float3 vMinPos{}, vMaxPos;
		_float2 vSpeedRange{};
		_float2 vLifeTime{};
		_float2 vScaleRange{};
		//_float3 vDir{};
	};
protected:
	CVIBuffer_Instancing(_dev pDevice, _context pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render() override;

protected:
	D3D11_BUFFER_DESC m_InstancingBufferDesc{};
	D3D11_SUBRESOURCE_DATA m_InstancingInitialData{};

	ID3D11Buffer* m_pVBInstance{ nullptr };

	_uint m_iNumInstances{};
	_uint m_iIndexCountPerInstance{};
	_uint m_iInstanceStride{};

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END