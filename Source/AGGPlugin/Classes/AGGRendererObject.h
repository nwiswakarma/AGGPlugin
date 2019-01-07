////////////////////////////////////////////////////////////////////////////////
//
// MIT License
// 
// Copyright (c) 2018-2019 Nuraga Wiswakarma
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////
// 

#pragma once

#include "CoreUObject.h"
#include "SharedPointer.h"

#include "AGGTypes.h"
#include "AGGContext.h"
#include "AGGRenderer.h"
#include "AGGRendererObject.generated.h"

UCLASS(BlueprintType)
class AGGPLUGIN_API UAGGScanlineRenderer : public UObject
{
	GENERATED_BODY()

public:

    struct IRenderProxy
    {
        virtual void SetColor(const FColor& Color) = 0;
    };

    template<class FAGGRenderer>
    struct TRenderProxy : public IRenderProxy
    {
        FAGGRenderer* Renderer;

        TRenderProxy(FAGGRenderer* c)
        {
            Renderer = c;
        }

        virtual void SetColor(const FColor& c) override
        {
            if (Renderer)
                Renderer->color(agg::rgba8(c.R, c.G, c.B, c.A));
        }
    };

    template<class FAGGRenderer>
    void SetRenderer(FAGGRenderer* Renderer)
    {
        if (RenderProxy.IsValid())
            RenderProxy.Reset();
        RenderProxy = MakeShareable( new TRenderProxy<FAGGRenderer>(Renderer) );
    }


    UFUNCTION(BlueprintCallable, Category = "AGG")
    void SetColor(const FColor& Color)
    {
        if (RenderProxy.IsValid())
            RenderProxy->SetColor(Color);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void SetColorByte(uint8 v)
    {
        if (RenderProxy.IsValid())
            RenderProxy->SetColor(FColor(v,v,v,v));
    }

    TSharedPtr<IRenderProxy> RenderProxy;
};

#define AGG_TYPED_RENDERER_SWITCH(TypeName, PixelFormat, Method, Statement) \
    switch (PixelFormat)\
    {\
        case EAGGPixFmt::PF_G8:          Method<TypeName, FAGGPFG8>()          Statement; break;\
        case EAGGPixFmt::PF_BGRA32:      Method<TypeName, FAGGPFBGRA32>()      Statement; break;\
        case EAGGPixFmt::PF_AlphaBlendR: Method<TypeName, FAGGPFAlphaBlendR>() Statement; break;\
        case EAGGPixFmt::PF_AlphaBlendG: Method<TypeName, FAGGPFAlphaBlendG>() Statement; break;\
        case EAGGPixFmt::PF_AlphaBlendB: Method<TypeName, FAGGPFAlphaBlendB>() Statement; break;\
    }

#define AGG_TYPED_RENDERER_CALL(TypeName, PixelFormat, Method) \
    AGG_TYPED_RENDERER_SWITCH(TypeName, PixelFormat, GetRenderer, -> Method())

#define AGG_TYPED_RENDERER_CALL_ONE_PARAM(TypeName, PixelFormat, Method, Param1) \
    AGG_TYPED_RENDERER_SWITCH(TypeName, PixelFormat, GetRenderer, -> Method(Param1))

#define AGG_TYPED_RENDERER_CALL_TWO_PARAM(TypeName, PixelFormat, Method, Param1, Param2) \
    AGG_TYPED_RENDERER_SWITCH(TypeName, PixelFormat, GetRenderer, -> Method(Param1, Param2))

#define AGG_TYPED_RENDERER_CALL_THREE_PARAM(TypeName, PixelFormat, Method, Param1, Param2, Param3) \
    AGG_TYPED_RENDERER_SWITCH(TypeName, PixelFormat, GetRenderer, -> Method(Param1, Param2, Param3))

UCLASS(Abstract, BlueprintType)
class AGGPLUGIN_API UAGGRendererBase : public UObject
{
	GENERATED_BODY()

protected:

    void* UntypedRenderer = nullptr;
    EAGGPixFmt PixFmt = EAGGPixFmt::PF_Unknown;

public:

    // BEGIN UObject Interface

    virtual void PostInitProperties() override
    {
        Super::PostInitProperties();
        InitRenderer();
    }

    virtual void BeginDestroy() override
    {
        ResetRenderer();
        Super::BeginDestroy();
    }

    // END UObject Interface

    virtual void InitRenderer()
    {
        // Blank Implementation
    }

    UFUNCTION(BlueprintCallable)
    virtual void SetColor(FColor Color)
        PURE_VIRTUAL(UAGGContext::SetColor, );

    UFUNCTION(BlueprintCallable)
    virtual void SetColorByte(uint8 Value)
        PURE_VIRTUAL(UAGGContext::SetColorByte, );

    UFUNCTION(BlueprintCallable)
    virtual void ResetRenderer()
    {
        check(UntypedRenderer == nullptr);
        check(PixFmt == EAGGPixFmt::PF_Unknown);
    }

    UFUNCTION(BlueprintCallable)
    virtual void AttachBuffer(UAGGContext* Context)
        PURE_VIRTUAL(UAGGContext::AttachBuffer, );

    UFUNCTION(BlueprintCallable)
    virtual void Render(UAGGPathController* Path)
        PURE_VIRTUAL(UAGGContext::Render, );

protected:

    template<template<typename> class FRenderer>
    void ResetRendererTyped()
    {
        if (UntypedRenderer)
        {
            check(PixFmt != EAGGPixFmt::PF_Unknown);
            AGG_TYPED_RENDERER_SWITCH(FRenderer, PixFmt, ResetRendererTyped, )
        }

        PixFmt = EAGGPixFmt::PF_Unknown;
    }

    template<template<typename> class FRenderer, class FPixFmt>
    void ResetRendererTyped()
    {
        check(UntypedRenderer);

        delete GetRenderer<FRenderer, FPixFmt>();
        UntypedRenderer = nullptr;
    }

    template<template<typename> class FRenderer, class FPixFmt>
    FORCEINLINE FRenderer<FPixFmt>* GetRenderer()
    {
        return reinterpret_cast<FRenderer<FPixFmt>*>(UntypedRenderer);
    }

    template<template<typename> class FRenderer, class FPixFmt>
    void CreateRendererTyped()
    {
        UntypedRenderer = reinterpret_cast<void*>(new FRenderer<FPixFmt>());
    }

    template<template<typename> class FRenderer>
    void CreateRendererTyped(EAGGPixFmt InPixFmt)
    {
        ResetRendererTyped<FRenderer>();

        AGG_TYPED_RENDERER_SWITCH(FRenderer, InPixFmt, CreateRendererTyped, );

        if (UntypedRenderer)
        {
            PixFmt = InPixFmt;
        }
    }

    template<template<typename> class FRenderer>
    void AttachBufferTyped(agg::rendering_buffer& Buffer)
    {
        if (UntypedRenderer)
        {
            AGG_TYPED_RENDERER_CALL_ONE_PARAM(FRenderer, PixFmt, Attach, Buffer)
        }
    }

    template<template<typename> class FRenderer>
    void SetColorTyped(const FColor& Color)
    {
        if (UntypedRenderer)
        {
            AGG_TYPED_RENDERER_CALL_ONE_PARAM(FRenderer, PixFmt, SetColor, Color)
        }
    }

    template<template<typename> class FRenderer>
    void SetColorTyped(uint8 Value)
    {
        if (UntypedRenderer)
        {
            AGG_TYPED_RENDERER_CALL_ONE_PARAM(FRenderer, PixFmt, SetColor, Value)
        }
    }

    template<template<typename> class FRenderer>
    void ResetPathTyped()
    {
        if (UntypedRenderer)
        {
            AGG_TYPED_RENDERER_CALL(FRenderer, PixFmt, ResetPath)
        }
    }

    template<template<typename> class FRenderer>
    void AddPathTyped(agg::path_storage& Path)
    {
        if (UntypedRenderer)
        {
            AGG_TYPED_RENDERER_CALL_ONE_PARAM(FRenderer, PixFmt, AddPath, Path)
        }
    }
};

UCLASS(BlueprintType)
class AGGPLUGIN_API UAGGRendererScanline : public UAGGRendererBase
{
	GENERATED_BODY()

    template <class FPixFmt>
    using FRenderer = TAGGRendererScanline<FPixFmt>;

public:

    UPROPERTY(BlueprintReadWrite)
    EAGGScanline ScanlineType = EAGGScanline::SL_P8;

    UPROPERTY(BlueprintReadWrite)
    FColor Color;

    virtual void ResetRenderer() override
    {
        ResetRendererTyped<FRenderer>();
        UAGGRendererBase::ResetRenderer();
    }

    UFUNCTION(BlueprintCallable)
    void CreateRenderer(EAGGPixFmt InPixFmt, UAGGContext* Context = nullptr)
    {
        CreateRendererTyped<FRenderer>(InPixFmt);

        if (IsValid(Context))
        {
            AttachBuffer(Context);
        }
    }

    virtual void AttachBuffer(UAGGContext* Context) override
    {
        if (IsValid(Context))
        {
            if (IAGGRenderBuffer* Buffer = Context->GetBuffer())
            {
                AttachBufferTyped<FRenderer>(Buffer->GetAGGBuffer());
            }
        }
    }

    virtual void SetColor(FColor InColor) override
    {
        SetColorTyped<FRenderer>(InColor);
        Color = InColor;
    }

    virtual void SetColorByte(uint8 InValue) override
    {
        SetColorTyped<FRenderer>(InValue);
        Color = FColor(InValue, InValue, InValue, InValue);
    }

    UFUNCTION(BlueprintCallable)
    void ResetPath()
    {
        ResetPathTyped<FRenderer>();
    }

    UFUNCTION(BlueprintCallable)
    void AddPath(UAGGPathController* PathController)
    {
        if (IsValid(PathController))
        {
            AddPathTyped<FRenderer>(PathController->GetAGGPath());
        }
    }

    UFUNCTION(BlueprintCallable)
    void RenderPath(UAGGPathController* Path, FColor InColor, EAGGScanline Scanline = EAGGScanline::SL_Unknown)
    {
        if (UntypedRenderer && IsValid(Path))
        {
            if (Scanline == EAGGScanline::SL_Unknown)
            {
                Scanline = ScanlineType;
            }

            AGG_TYPED_RENDERER_CALL_THREE_PARAM(FRenderer, PixFmt, Render, Path->GetAGGPath(), InColor, Scanline);
        }
    }

    virtual void Render(UAGGPathController* Path) override
    {
        if (UntypedRenderer && IsValid(Path))
        {
            AGG_TYPED_RENDERER_CALL_THREE_PARAM(FRenderer, PixFmt, Render, Path->GetAGGPath(), Color, ScanlineType);
        }
    }
};

UCLASS(BlueprintType)
class AGGPLUGIN_API UAGGRendererOutlineAA : public UAGGRendererBase
{
	GENERATED_BODY()

    template <class FPixFmt>
    using FRenderer = TAGGRendererOutline<FPixFmt>;

public:

    UPROPERTY(BlueprintReadWrite)
    FColor Color;

    virtual void ResetRenderer() override
    {
        ResetRendererTyped<FRenderer>();
        UAGGRendererBase::ResetRenderer();
    }

    virtual void AttachBuffer(UAGGContext* Context) override
    {
        if (IsValid(Context))
        {
            if (IAGGRenderBuffer* Buffer = Context->GetBuffer())
            {
                AttachBufferTyped<FRenderer>(Buffer->GetAGGBuffer());
            }
        }
    }

    virtual void SetColor(FColor InColor) override
    {
        SetColorTyped<FRenderer>(InColor);
        Color = InColor;
    }

    virtual void SetColorByte(uint8 InValue) override
    {
        SetColorTyped<FRenderer>(InValue);
        Color = FColor(InValue, InValue, InValue, InValue);
    }

    UFUNCTION(BlueprintCallable)
    void CreateRenderer(EAGGPixFmt InPixFmt, UAGGContext* Context = nullptr)
    {
        CreateRendererTyped<FRenderer>(InPixFmt);

        if (IsValid(Context))
        {
            AttachBuffer(Context);
        }
    }

    UFUNCTION(BlueprintCallable)
    void SetLineProfile(FAGGOutlineAALineProfile LineProfile)
    {
        if (UntypedRenderer)
        {
            AGG_TYPED_RENDERER_CALL_ONE_PARAM(FRenderer, PixFmt, SetLineProfile, LineProfile);
        }
    }

    UFUNCTION(BlueprintCallable)
    void SetClosePolygon(bool bClosePolygon)
    {
        if (UntypedRenderer)
        {
            AGG_TYPED_RENDERER_CALL_ONE_PARAM(FRenderer, PixFmt, SetClosePolygon, bClosePolygon);
        }
    }

    UFUNCTION(BlueprintCallable)
    void AddPath(UAGGPathController* Path, bool bClosePolygon)
    {
        if (UntypedRenderer && IsValid(Path))
        {
            AGG_TYPED_RENDERER_CALL_TWO_PARAM(FRenderer, PixFmt, AddPath, Path->GetAGGPath(), bClosePolygon);
        }
    }

    UFUNCTION(BlueprintCallable)
    void RenderPath(UAGGPathController* Path, FColor InColor, bool bClosePolygon)
    {
        if (UntypedRenderer && IsValid(Path))
        {
            AGG_TYPED_RENDERER_CALL_THREE_PARAM(FRenderer, PixFmt, Render, Path->GetAGGPath(), InColor, bClosePolygon);
        }
    }

    virtual void Render(UAGGPathController* Path) override
    {
        if (UntypedRenderer && IsValid(Path))
        {
            AGG_TYPED_RENDERER_CALL_ONE_PARAM(FRenderer, PixFmt, Render, Path->GetAGGPath());
        }
    }
};
