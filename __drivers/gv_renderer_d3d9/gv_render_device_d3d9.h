#pragma once
namespace gv
{
#define D3D9_RS_MAX D3DRS_BLENDOPALPHA + 1
#define D3DCOLORWRITEENABLE_ALL                            \
	(D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | \
	 D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA)

#define DX9_MAX_STREAM 8
#define DX9_MAX_TEXTURE 16
#define DX9_MAX_SAMPLER 16
#define D3DSAMP_MAX 14
inline DWORD DX9FloatToDWORD(FLOAT fValue)
{
	return *(DWORD*)&fValue;
}

class gv_Direct3DDevice9 : public IDirect3DDevice9
{
public:
	// We need d3d so that we'd use a pointer to Direct3D9 instead of the original
	// IDirect3D9 implementor
	// in functions like GetDirect3D9
	gv_Direct3DDevice9()
	{
		m_device = NULL;
		m_d3d = NULL;
		m_VertexShader = NULL;
		m_PixelShader = NULL;
	}
	~gv_Direct3DDevice9()
	{
		UnInitialize();
	}

	void Initialize(IDirect3DDevice9* device, IDirect3D9* d3d)
	{
		m_d3d = d3d;
		m_device = device;
		SetDefaultRenderStates();
	}

	void UnInitialize()
	{
		if (m_device != NULL)
		{
			m_device->Release();
			m_device = NULL;
		}

		if (m_d3d)
		{
			m_d3d->Release();
			m_d3d = NULL;
		}
	}

	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)
	(THIS_ REFIID riid, void** ppvObj)
	{
		return m_device->QueryInterface(riid, ppvObj);
	}

	STDMETHOD_(ULONG, AddRef)
	(THIS)
	{
		return m_device->AddRef();
	}

	STDMETHOD_(ULONG, Release)
	(THIS)
	{
		ULONG count = m_device->Release();
		if (0 == count)
			delete this;

		return count;
	}

	/*** IDirect3DDevice9 methods ***/
	STDMETHOD(TestCooperativeLevel)
	(THIS)
	{
		return m_device->TestCooperativeLevel();
	}

	STDMETHOD_(UINT, GetAvailableTextureMem)
	(THIS)
	{
		return m_device->GetAvailableTextureMem();
	}

	STDMETHOD(EvictManagedResources)
	(THIS)
	{
		return m_device->EvictManagedResources();
	}

	STDMETHOD(GetDirect3D)
	(THIS_ IDirect3D9** ppD3D9)
	{
		// Let the device validate the incoming pointer for us
		HRESULT hr = m_device->GetDirect3D(ppD3D9);
		if (SUCCEEDED(hr))
			*ppD3D9 = m_d3d;

		return hr;
	}

	STDMETHOD(GetDeviceCaps)
	(THIS_ D3DCAPS9* pCaps)
	{
		return m_device->GetDeviceCaps(pCaps);
	}

	STDMETHOD(GetDisplayMode)
	(THIS_ UINT iSwapChain, D3DDISPLAYMODE* pMode)
	{
		return m_device->GetDisplayMode(iSwapChain, pMode);
	}

	STDMETHOD(GetCreationParameters)
	(THIS_ D3DDEVICE_CREATION_PARAMETERS* pParameters)
	{
		return m_device->GetCreationParameters(pParameters);
	}

	STDMETHOD(SetCursorProperties)
	(THIS_ UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
	{
		return m_device->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
	}

	STDMETHOD_(void, SetCursorPosition)
	(THIS_ int X, int Y, DWORD Flags)
	{
		m_device->SetCursorPosition(X, Y, Flags);
	}

	STDMETHOD_(BOOL, ShowCursor)
	(THIS_ BOOL bShow)
	{
		return m_device->ShowCursor(bShow);
	}

	STDMETHOD(CreateAdditionalSwapChain)
	(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters,
	 IDirect3DSwapChain9** pSwapChain)
	{
		return m_device->CreateAdditionalSwapChain(pPresentationParameters,
												   pSwapChain);
	}

	STDMETHOD(GetSwapChain)
	(THIS_ UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
	{
		return m_device->GetSwapChain(iSwapChain, pSwapChain);
	}

	STDMETHOD_(UINT, GetNumberOfSwapChains)
	(THIS)
	{
		return m_device->GetNumberOfSwapChains();
	}

	STDMETHOD(Reset)
	(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		return m_device->Reset(pPresentationParameters);
	}

	STDMETHOD(Present)
	(THIS_ CONST RECT* pSourceRect, CONST RECT* pDestRect,
	 HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
	{
		return m_device->Present(pSourceRect, pDestRect, hDestWindowOverride,
								 pDirtyRegion);
	}

	STDMETHOD(GetBackBuffer)
	(THIS_ UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type,
	 IDirect3DSurface9** ppBackBuffer)
	{
		return m_device->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
	}

	STDMETHOD(GetRasterStatus)
	(THIS_ UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
	{
		return m_device->GetRasterStatus(iSwapChain, pRasterStatus);
	}

	STDMETHOD(SetDialogBoxMode)
	(THIS_ BOOL bEnableDialogs)
	{
		return m_device->SetDialogBoxMode(bEnableDialogs);
	}

	STDMETHOD_(void, SetGammaRamp)
	(THIS_ UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp)
	{
		return m_device->SetGammaRamp(iSwapChain, Flags, pRamp);
	}

	STDMETHOD_(void, GetGammaRamp)
	(THIS_ UINT iSwapChain, D3DGAMMARAMP* pRamp)
	{
		return m_device->GetGammaRamp(iSwapChain, pRamp);
	}

	STDMETHOD(CreateTexture)
	(THIS_ UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format,
	 D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
	{
		return m_device->CreateTexture(Width, Height, Levels, Usage, Format, Pool,
									   ppTexture, pSharedHandle);
	}

	STDMETHOD(CreateVolumeTexture)
	(THIS_ UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage,
	 D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture,
	 HANDLE* pSharedHandle)
	{
		return m_device->CreateVolumeTexture(Width, Height, Depth, Levels, Usage,
											 Format, Pool, ppVolumeTexture,
											 pSharedHandle);
	}

	STDMETHOD(CreateCubeTexture)
	(THIS_ UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format,
	 D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
	{
		return m_device->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool,
										   ppCubeTexture, pSharedHandle);
	}

	STDMETHOD(CreateVertexBuffer)
	(THIS_ UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool,
	 IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
	{
		return m_device->CreateVertexBuffer(Length, Usage, FVF, Pool,
											ppVertexBuffer, pSharedHandle);
	}

	STDMETHOD(CreateIndexBuffer)
	(THIS_ UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
	 IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
	{
		return m_device->CreateIndexBuffer(Length, Usage, Format, Pool,
										   ppIndexBuffer, pSharedHandle);
	}

	STDMETHOD(CreateRenderTarget)
	(THIS_ UINT Width, UINT Height, D3DFORMAT Format,
	 D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable,
	 IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
	{
		return m_device->CreateRenderTarget(Width, Height, Format, MultiSample,
											MultisampleQuality, Lockable, ppSurface,
											pSharedHandle);
	}

	STDMETHOD(CreateDepthStencilSurface)
	(THIS_ UINT Width, UINT Height, D3DFORMAT Format,
	 D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard,
	 IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
	{
		return m_device->CreateDepthStencilSurface(
			Width, Height, Format, MultiSample, MultisampleQuality, Discard,
			ppSurface, pSharedHandle);
	}

	STDMETHOD(UpdateSurface)
	(THIS_ IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect,
	 IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint)
	{
		return m_device->UpdateSurface(pSourceSurface, pSourceRect,
									   pDestinationSurface, pDestPoint);
	}

	STDMETHOD(UpdateTexture)
	(THIS_ IDirect3DBaseTexture9* pSourceTexture,
	 IDirect3DBaseTexture9* pDestinationTexture)
	{
		return m_device->UpdateTexture(pSourceTexture, pDestinationTexture);
	}

	STDMETHOD(GetRenderTargetData)
	(THIS_ IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
	{
		return m_device->GetRenderTargetData(pRenderTarget, pDestSurface);
	}

	STDMETHOD(GetFrontBufferData)
	(THIS_ UINT iSwapChain, IDirect3DSurface9* pDestSurface)
	{
		return m_device->GetFrontBufferData(iSwapChain, pDestSurface);
	}

	STDMETHOD(StretchRect)
	(THIS_ IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect,
	 IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect,
	 D3DTEXTUREFILTERTYPE Filter)
	{
		return m_device->StretchRect(pSourceSurface, pSourceRect, pDestSurface,
									 pDestRect, Filter);
	}

	STDMETHOD(ColorFill)
	(THIS_ IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
	{
		return m_device->ColorFill(pSurface, pRect, color);
	}

	STDMETHOD(CreateOffscreenPlainSurface)
	(THIS_ UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool,
	 IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
	{
		return m_device->CreateOffscreenPlainSurface(Width, Height, Format, Pool,
													 ppSurface, pSharedHandle);
	}

	STDMETHOD(SetRenderTarget)
	(THIS_ DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
	{
		return m_device->SetRenderTarget(RenderTargetIndex, pRenderTarget);
	}

	STDMETHOD(GetRenderTarget)
	(THIS_ DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
	{
		return m_device->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
	}

	STDMETHOD(SetDepthStencilSurface)
	(THIS_ IDirect3DSurface9* pNewZStencil)
	{
		return m_device->SetDepthStencilSurface(pNewZStencil);
	}

	STDMETHOD(GetDepthStencilSurface)
	(THIS_ IDirect3DSurface9** ppZStencilSurface)
	{
		return m_device->GetDepthStencilSurface(ppZStencilSurface);
	}

	STDMETHOD(BeginScene)
	(THIS)
	{
		return m_device->BeginScene();
	}

	STDMETHOD(EndScene)
	(THIS)
	{
		SetDefaultRenderStates();
		/*
            // Draw a custom quad to the screen
            struct TLVertex
            {
                    float x, y, z, rhw;
                    DWORD color;
            };

            TLVertex vertices[] =
            {
                    { -0.5f, -0.5f, 0.0f, 1.0f, 0xffff0000 },
                    { 99.5f, -0.5f, 0.0f, 1.0f, 0xff00ff00 },
                    { -0.5f, 99.5f, 0.0f, 1.0f, 0xff0000ff },
                    { 99.5f, 99.5f, 0.0f, 1.0f, 0xffff0000 }
            };
            SetVertexShader(0);
            SetPixelShader(0);
            SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
            SetRenderState(D3DRS_ZENABLE, false);
            SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
            SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
            SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
            DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(TLVertex));
            SetRenderState(D3DRS_ZENABLE, true);
    */
		// SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		return m_device->EndScene();
	}

	STDMETHOD(Clear)
	(THIS_ DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color,
	 float Z, DWORD Stencil)
	{
		return m_device->Clear(Count, pRects, Flags, Color, Z, Stencil);
	}

	STDMETHOD(SetTransform)
	(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
	{
		return m_device->SetTransform(State, pMatrix);
	}

	STDMETHOD(GetTransform)
	(THIS_ D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
	{
		return m_device->GetTransform(State, pMatrix);
	}

	STDMETHOD(MultiplyTransform)
	(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
	{
		return m_device->MultiplyTransform(State, pMatrix);
	}

	STDMETHOD(SetViewport)
	(THIS_ CONST D3DVIEWPORT9* pViewport)
	{
		m_viewport.set(pViewport->X, pViewport->Y, pViewport->X + pViewport->Width,
					   pViewport->Y + pViewport->Height);
		m_viewport_d3d = *pViewport;
		return m_device->SetViewport(pViewport);
	}

	STDMETHOD(GetViewport)
	(THIS_ D3DVIEWPORT9* pViewport)
	{
		return m_device->GetViewport(pViewport);
	}

	STDMETHOD(SetMaterial)
	(THIS_ CONST D3DMATERIAL9* pMaterial)
	{
		return m_device->SetMaterial(pMaterial);
	}

	STDMETHOD(GetMaterial)
	(THIS_ D3DMATERIAL9* pMaterial)
	{
		return m_device->GetMaterial(pMaterial);
	}

	STDMETHOD(SetLight)
	(THIS_ DWORD Index, CONST D3DLIGHT9* pLight)
	{
		return m_device->SetLight(Index, pLight);
	}

	STDMETHOD(GetLight)
	(THIS_ DWORD Index, D3DLIGHT9* pLight)
	{
		return m_device->GetLight(Index, pLight);
	}

	STDMETHOD(LightEnable)
	(THIS_ DWORD Index, BOOL Enable)
	{
		return m_device->LightEnable(Index, Enable);
	}

	STDMETHOD(GetLightEnable)
	(THIS_ DWORD Index, BOOL* pEnable)
	{
		return m_device->GetLightEnable(Index, pEnable);
	}

	STDMETHOD(SetClipPlane)
	(THIS_ DWORD Index, CONST float* pPlane)
	{
		return m_device->SetClipPlane(Index, pPlane);
	}

	STDMETHOD(GetClipPlane)
	(THIS_ DWORD Index, float* pPlane)
	{
		return m_device->GetClipPlane(Index, pPlane);
	}

	STDMETHOD(SetRenderState)
	(THIS_ D3DRENDERSTATETYPE State, DWORD Value)
	{
		if (m_RenderStates[State] != Value)
		{
			GVM_VERIFY_D3D(ForceSetRenderState(State, Value));
		}
		return S_OK;
	}

	STDMETHOD(GetRenderState)
	(THIS_ D3DRENDERSTATETYPE State, DWORD* pValue)
	{
		return m_device->GetRenderState(State, pValue);
	}

	STDMETHOD(CreateStateBlock)
	(THIS_ D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
	{
		return m_device->CreateStateBlock(Type, ppSB);
	}

	STDMETHOD(BeginStateBlock)
	(THIS)
	{
		return m_device->BeginStateBlock();
	}

	STDMETHOD(EndStateBlock)
	(THIS_ IDirect3DStateBlock9** ppSB)
	{
		return m_device->EndStateBlock(ppSB);
	}

	STDMETHOD(SetClipStatus)
	(THIS_ CONST D3DCLIPSTATUS9* pClipStatus)
	{
		return m_device->SetClipStatus(pClipStatus);
	}

	STDMETHOD(GetClipStatus)
	(THIS_ D3DCLIPSTATUS9* pClipStatus)
	{
		return m_device->GetClipStatus(pClipStatus);
	}

	STDMETHOD(GetTexture)
	(THIS_ DWORD Stage, IDirect3DBaseTexture9** ppTexture)
	{
		return m_device->GetTexture(Stage, ppTexture);
	}

	STDMETHOD(SetTexture)
	(THIS_ DWORD Stage, IDirect3DBaseTexture9* pTexture)
	{
		return m_device->SetTexture(Stage, pTexture);
	}

	STDMETHOD(GetTextureStageState)
	(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
	{
		return m_device->GetTextureStageState(Stage, Type, pValue);
	}

	STDMETHOD(SetTextureStageState)
	(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
	{
		return m_device->SetTextureStageState(Stage, Type, Value);
	}

	STDMETHOD(GetSamplerState)
	(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
	{
		return m_device->GetSamplerState(Sampler, Type, pValue);
	}

	STDMETHOD(SetSamplerState)
	(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
	{
		return m_device->SetSamplerState(Sampler, Type, Value);
	}

	STDMETHOD(ValidateDevice)
	(THIS_ DWORD* pNumPasses)
	{
		return m_device->ValidateDevice(pNumPasses);
	}

	STDMETHOD(SetPaletteEntries)
	(THIS_ UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
	{
		return m_device->SetPaletteEntries(PaletteNumber, pEntries);
	}

	STDMETHOD(GetPaletteEntries)
	(THIS_ UINT PaletteNumber, PALETTEENTRY* pEntries)
	{
		return m_device->GetPaletteEntries(PaletteNumber, pEntries);
	}

	STDMETHOD(SetCurrentTexturePalette)
	(THIS_ UINT PaletteNumber)
	{
		return m_device->SetCurrentTexturePalette(PaletteNumber);
	}

	STDMETHOD(GetCurrentTexturePalette)
	(THIS_ UINT* PaletteNumber)
	{
		return m_device->GetCurrentTexturePalette(PaletteNumber);
	}

	STDMETHOD(SetScissorRect)
	(THIS_ CONST RECT* pRect)
	{
		return m_device->SetScissorRect(pRect);
	}

	STDMETHOD(GetScissorRect)
	(THIS_ RECT* pRect)
	{
		return m_device->GetScissorRect(pRect);
	}

	STDMETHOD(SetSoftwareVertexProcessing)
	(THIS_ BOOL bSoftware)
	{
		return m_device->SetSoftwareVertexProcessing(bSoftware);
	}

	STDMETHOD_(BOOL, GetSoftwareVertexProcessing)
	(THIS)
	{
		return m_device->GetSoftwareVertexProcessing();
	}

	STDMETHOD(SetNPatchMode)
	(THIS_ float nSegments)
	{
		return m_device->SetNPatchMode(nSegments);
	}

	STDMETHOD_(float, GetNPatchMode)
	(THIS)
	{
		return m_device->GetNPatchMode();
	}

	STDMETHOD(DrawPrimitive)
	(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex,
	 UINT PrimitiveCount)
	{
		return m_device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
	}

	STDMETHOD(DrawIndexedPrimitive)
	(THIS_ D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex,
	 UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
	{
		return m_device->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex,
											  MinVertexIndex, NumVertices,
											  startIndex, primCount);
	}

	STDMETHOD(DrawPrimitiveUP)
	(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount,
	 CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
	{
		return m_device->DrawPrimitiveUP(PrimitiveType, PrimitiveCount,
										 pVertexStreamZeroData,
										 VertexStreamZeroStride);
	}

	STDMETHOD(DrawIndexedPrimitiveUP)
	(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices,
	 UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat,
	 CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
	{
		return m_device->DrawIndexedPrimitiveUP(
			PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData,
			IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
	}

	STDMETHOD(ProcessVertices)
	(THIS_ UINT SrcStartIndex, UINT DestIndex, UINT VertexCount,
	 IDirect3DVertexBuffer9* pDestBuffer,
	 IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
	{
		return m_device->ProcessVertices(SrcStartIndex, DestIndex, VertexCount,
										 pDestBuffer, pVertexDecl, Flags);
	}

	STDMETHOD(CreateVertexDeclaration)
	(THIS_ CONST D3DVERTEXELEMENT9* pVertexElements,
	 IDirect3DVertexDeclaration9** ppDecl)
	{
		return m_device->CreateVertexDeclaration(pVertexElements, ppDecl);
	}

	STDMETHOD(SetVertexDeclaration)
	(THIS_ IDirect3DVertexDeclaration9* pDecl)
	{
		return m_device->SetVertexDeclaration(pDecl);
	}

	STDMETHOD(GetVertexDeclaration)
	(THIS_ IDirect3DVertexDeclaration9** ppDecl)
	{
		return m_device->GetVertexDeclaration(ppDecl);
	}

	STDMETHOD(SetFVF)
	(THIS_ DWORD FVF)
	{
		return m_device->SetFVF(FVF);
	}

	STDMETHOD(GetFVF)
	(THIS_ DWORD* pFVF)
	{
		return m_device->GetFVF(pFVF);
	}

	STDMETHOD(CreateVertexShader)
	(THIS_ CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
	{
		return m_device->CreateVertexShader(pFunction, ppShader);
	}

	STDMETHOD(SetVertexShader)
	(THIS_ IDirect3DVertexShader9* pShader)
	{
		if (m_VertexShader == pShader)
			return S_OK;
		m_VertexShader = pShader;
		return m_device->SetVertexShader(pShader);
	}

	STDMETHOD(GetVertexShader)
	(THIS_ IDirect3DVertexShader9** ppShader)
	{
		return m_device->GetVertexShader(ppShader);
	}

	STDMETHOD(SetVertexShaderConstantF)
	(THIS_ UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
	{
		return m_device->SetVertexShaderConstantF(StartRegister, pConstantData,
												  Vector4fCount);
	}

	STDMETHOD(GetVertexShaderConstantF)
	(THIS_ UINT StartRegister, float* pConstantData, UINT Vector4fCount)
	{
		return m_device->GetVertexShaderConstantF(StartRegister, pConstantData,
												  Vector4fCount);
	}

	STDMETHOD(SetVertexShaderConstantI)
	(THIS_ UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
	{
		return m_device->SetVertexShaderConstantI(StartRegister, pConstantData,
												  Vector4iCount);
	}

	STDMETHOD(GetVertexShaderConstantI)
	(THIS_ UINT StartRegister, int* pConstantData, UINT Vector4iCount)
	{
		return m_device->GetVertexShaderConstantI(StartRegister, pConstantData,
												  Vector4iCount);
	}

	STDMETHOD(SetVertexShaderConstantB)
	(THIS_ UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
	{
		return m_device->SetVertexShaderConstantB(StartRegister, pConstantData,
												  BoolCount);
	}

	STDMETHOD(GetVertexShaderConstantB)
	(THIS_ UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
	{
		return m_device->GetVertexShaderConstantB(StartRegister, pConstantData,
												  BoolCount);
	}

	STDMETHOD(SetStreamSource)
	(THIS_ UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData,
	 UINT OffsetInBytes, UINT Stride)
	{
		return m_device->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes,
										 Stride);
	}

	STDMETHOD(GetStreamSource)
	(THIS_ UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData,
	 UINT* pOffsetInBytes, UINT* pStride)
	{
		return m_device->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes,
										 pStride);
	}

	STDMETHOD(SetStreamSourceFreq)
	(THIS_ UINT StreamNumber, UINT Setting)
	{
		return m_device->SetStreamSourceFreq(StreamNumber, Setting);
	}

	STDMETHOD(GetStreamSourceFreq)
	(THIS_ UINT StreamNumber, UINT* pSetting)
	{
		return m_device->GetStreamSourceFreq(StreamNumber, pSetting);
	}

	STDMETHOD(SetIndices)
	(THIS_ IDirect3DIndexBuffer9* pIndexData)
	{
		return m_device->SetIndices(pIndexData);
	}

	STDMETHOD(GetIndices)
	(THIS_ IDirect3DIndexBuffer9** ppIndexData)
	{
		return m_device->GetIndices(ppIndexData);
	}

	STDMETHOD(CreatePixelShader)
	(THIS_ CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
	{
		return m_device->CreatePixelShader(pFunction, ppShader);
	}

	STDMETHOD(SetPixelShader)
	(THIS_ IDirect3DPixelShader9* pShader)
	{
		if (m_PixelShader == pShader)
			return S_OK;
		m_PixelShader = pShader;
		return m_device->SetPixelShader(pShader);
	}

	STDMETHOD(GetPixelShader)
	(THIS_ IDirect3DPixelShader9** ppShader)
	{
		return m_device->GetPixelShader(ppShader);
	}

	STDMETHOD(SetPixelShaderConstantF)
	(THIS_ UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount)
	{
		return m_device->SetPixelShaderConstantF(StartRegister, pConstantData,
												 Vector4fCount);
	}

	STDMETHOD(GetPixelShaderConstantF)
	(THIS_ UINT StartRegister, float* pConstantData, UINT Vector4fCount)
	{
		return m_device->GetPixelShaderConstantF(StartRegister, pConstantData,
												 Vector4fCount);
	}

	STDMETHOD(SetPixelShaderConstantI)
	(THIS_ UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount)
	{
		return m_device->SetPixelShaderConstantI(StartRegister, pConstantData,
												 Vector4iCount);
	}

	STDMETHOD(GetPixelShaderConstantI)
	(THIS_ UINT StartRegister, int* pConstantData, UINT Vector4iCount)
	{
		return m_device->GetPixelShaderConstantI(StartRegister, pConstantData,
												 Vector4iCount);
	}

	STDMETHOD(SetPixelShaderConstantB)
	(THIS_ UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount)
	{
		return m_device->SetPixelShaderConstantB(StartRegister, pConstantData,
												 BoolCount);
	}

	STDMETHOD(GetPixelShaderConstantB)
	(THIS_ UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
	{
		return m_device->GetPixelShaderConstantB(StartRegister, pConstantData,
												 BoolCount);
	}

	STDMETHOD(DrawRectPatch)
	(THIS_ UINT Handle, CONST float* pNumSegs,
	 CONST D3DRECTPATCH_INFO* pRectPatchInfo)
	{
		return m_device->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
	}

	STDMETHOD(DrawTriPatch)
	(THIS_ UINT Handle, CONST float* pNumSegs,
	 CONST D3DTRIPATCH_INFO* pTriPatchInfo)
	{
		return m_device->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
	}

	STDMETHOD(DeletePatch)
	(THIS_ UINT Handle)
	{
		return m_device->DeletePatch(Handle);
	}

	STDMETHOD(CreateQuery)
	(THIS_ D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
	{
		return m_device->CreateQuery(Type, ppQuery);
	}
	//------------------------------------------------------------------------>
	inline HRESULT ForceSetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
	{
		m_RenderStates[State] = Value;
		return m_device->SetRenderState(State, Value);
	}
	void SetDefaultRenderStates()
	{
		GV_PROFILE_EVENT_PIX(SetDefaultRenderStates, 0);
		ForceSetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		ForceSetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		ForceSetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		ForceSetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		ForceSetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		ForceSetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
		// SORRY WE ARE RIGHT HANDED SYSTEM
		ForceSetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		ForceSetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		ForceSetRenderState(D3DRS_ALPHAREF, 0);
		ForceSetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		ForceSetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		ForceSetRenderState(D3DRS_STENCILENABLE, FALSE);
		ForceSetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		ForceSetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		ForceSetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		ForceSetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		ForceSetRenderState(D3DRS_STENCILREF, 0);
		ForceSetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
		ForceSetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
		ForceSetRenderState(D3DRS_WRAP0, 0);
		ForceSetRenderState(D3DRS_WRAP1, 0);
		ForceSetRenderState(D3DRS_WRAP2, 0);
		ForceSetRenderState(D3DRS_WRAP3, 0);
		ForceSetRenderState(D3DRS_WRAP4, 0);
		ForceSetRenderState(D3DRS_WRAP5, 0);
		ForceSetRenderState(D3DRS_WRAP6, 0);
		ForceSetRenderState(D3DRS_WRAP7, 0);
		ForceSetRenderState(D3DRS_CLIPPLANEENABLE, 0);
		ForceSetRenderState(D3DRS_POINTSIZE, DX9FloatToDWORD(0.0f));
		ForceSetRenderState(D3DRS_POINTSIZE_MIN, DX9FloatToDWORD(0.0f));
		ForceSetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
		ForceSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		ForceSetRenderState(D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF);
		ForceSetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALL);
		ForceSetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		ForceSetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		ForceSetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
		ForceSetRenderState(D3DRS_MINTESSELLATIONLEVEL, DX9FloatToDWORD(1.0f));
		ForceSetRenderState(D3DRS_MAXTESSELLATIONLEVEL, DX9FloatToDWORD(1.0f));
		ForceSetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
		ForceSetRenderState(D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP);
		ForceSetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP);
		ForceSetRenderState(D3DRS_CCW_STENCILPASS, D3DSTENCILOP_KEEP);
		ForceSetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
		ForceSetRenderState(D3DRS_COLORWRITEENABLE1, 0xf);
		ForceSetRenderState(D3DRS_COLORWRITEENABLE2, 0xf);
		ForceSetRenderState(D3DRS_COLORWRITEENABLE3, 0xf);
		ForceSetRenderState(D3DRS_BLENDFACTOR, 0xffffffff);
		ForceSetRenderState(D3DRS_DEPTHBIAS, 0);
		ForceSetRenderState(D3DRS_WRAP8, 0);
		ForceSetRenderState(D3DRS_WRAP9, 0);
		ForceSetRenderState(D3DRS_WRAP10, 0);
		ForceSetRenderState(D3DRS_WRAP11, 0);
		ForceSetRenderState(D3DRS_WRAP12, 0);
		ForceSetRenderState(D3DRS_WRAP13, 0);
		ForceSetRenderState(D3DRS_WRAP14, 0);
		ForceSetRenderState(D3DRS_WRAP15, 0);
		ForceSetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
		ForceSetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
		ForceSetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
		ForceSetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);

		ForceSetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
		ForceSetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
		ForceSetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
		ForceSetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		ForceSetRenderState(D3DRS_SPECULARENABLE, FALSE);
		ForceSetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
		ForceSetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255));
	}
	inline const gv_recti& get_viewport()
	{
		return m_viewport;
	};
	inline gv_int get_viewport_w()
	{
		return m_viewport.width();
	}
	inline gv_int get_viewport_h()
	{
		return m_viewport.height();
	}
	inline void backup_viewport()
	{
		m_viewport_d3d_backup.push(m_viewport_d3d);
	}
	inline void restore_viewport()
	{
		m_viewport_d3d_backup.pop();
		m_viewport_d3d = m_viewport_d3d_backup.top();
		SetViewport(&m_viewport_d3d);
	}

private:
	IDirect3DDevice9* m_device;
	IDirect3D9* m_d3d;
	DWORD m_RenderStates[D3D9_RS_MAX];

	DWORD m_SamplerStates[DX9_MAX_SAMPLER][D3DSAMP_MAX];
	IDirect3DBaseTexture9* m_Textures[DX9_MAX_TEXTURE];
	IDirect3DVertexDeclaration9* m_VertexDeclaration;
	IDirect3DVertexShader9* m_VertexShader;
	IDirect3DPixelShader9* m_PixelShader;
	IDirect3DVertexBuffer9* m_VertexBuffers[DX9_MAX_STREAM];
	DWORD m_StreamOffsets[DX9_MAX_STREAM];
	DWORD m_StreamStrides[DX9_MAX_STREAM];
	IDirect3DIndexBuffer9* m_IndexBuffer;
	gv_recti m_viewport;
	D3DVIEWPORT9 m_viewport_d3d;
	gvt_stack_static< D3DVIEWPORT9, 16 > m_viewport_d3d_backup;
};

namespace gv_global
{
extern gvt_global< gv_Direct3DDevice9 > device_d3d9;
}

// more VA friendly ???
inline gv_Direct3DDevice9* get_device_d3d9()
{
	return gv_global::device_d3d9.get();
}

class gv_draw_policy_tri_list_d3d
{
public:
	void draw(gv_uint start_vertex, gv_uint end_vertex)
	{
		GV_ASSERT(end_vertex > start_vertex);
		get_device_d3d9()->DrawPrimitive(D3DPT_TRIANGLELIST, start_vertex,
										 (end_vertex - start_vertex + 1) / 3);
	}
};
}
