// 

#pragma once

#include "AGGTypes.h"
#include "AGGContext.h"
#include "AGGRendererObject.h"
#include "AGGTypedContext.generated.h"

UCLASS(BlueprintType, Blueprintable)
class AGGPLUGIN_API UAGGContextScanlineBGRA : public UAGGContext
{
	GENERATED_BODY()

    typedef TAGGBufferBGRA32 TBuffer;
    typedef TAGGScanlineRenderer<TBuffer::TPixFmt> TRenderer;

    typedef TSharedPtr<TBuffer> TPSBuffer;
    typedef TSharedPtr<TRenderer> TPSRenderer;

	UPROPERTY(Transient)
    UAGGScanlineRenderer* Renderer;

protected:

    TPSBuffer ContextBuffer;
    TPSRenderer ContextRenderer;

public:

    UFUNCTION(BlueprintCallable, Category = "AGG")
    virtual UAGGScanlineRenderer* GetRendererController()
    {
        return Renderer;
    }

    // BEGIN UAGGContext Interface

    virtual void InitContext()
    {
        ContextBuffer = MakeShareable( new TBuffer() );
        ContextRenderer = MakeShareable( new TRenderer() );

        if (! Renderer)
        {
            Renderer = NewObject<UAGGScanlineRenderer>(this);
        }

        if (! PathController)
        {
            PathController = NewObject<UAGGPathController>(this);
        }
    }

    virtual void ClearContext()
    {
        ContextBuffer.Reset();
        ContextRenderer.Reset();
        Renderer = nullptr;
        PathController = nullptr;
    }

    virtual void InitBuffer(int32 w, int32 h, int32 c, bool bSq)
    {
        check(ContextBuffer.IsValid());
        check(ContextRenderer.IsValid());
        check(Renderer);

        ContextBuffer->Init(w, h, c, bSq);
        ContextRenderer->ConstructRenderer( *ContextBuffer->GetPixFmt() );
        Renderer->SetRenderer( &ContextRenderer->Renderer );
    }

    virtual void ClearBuffer(uint8 ClearVal)
    {
        if (ContextBuffer.IsValid())
            ContextBuffer->Clear(0);
    }

    virtual void RenderContext()
    {
        check(ContextBuffer.IsValid());
        check(ContextBuffer->IsValid());
        check(ContextRenderer.IsValid());
        check(Renderer);

        ContextRenderer->AddPath(**GetPathController());
        ContextRenderer->Render();
    }

    virtual IAGGRenderBuffer* GetBuffer()
    {
        return ContextBuffer.Get();
    }

    virtual const IAGGRenderBuffer* GetBuffer() const
    {
        return ContextBuffer.Get();
    }

    virtual EPixelFormat GetPixelFormat()
    {
        return EPixelFormat::PF_B8G8R8A8;
    }

    virtual void SetColor(const FColor& Color)
    {
        check(Renderer);
        return Renderer->SetColor(Color);
    }

    virtual void SetColorByte(uint8 Color)
    {
        check(Renderer);
        return Renderer->SetColorByte(Color);
    }

    // END UAGGContext Interface

};

UCLASS(BlueprintType, Blueprintable)
class AGGPLUGIN_API UAGGContextScanlineG8 : public UAGGContext
{
	GENERATED_BODY()

    typedef TAGGBufferG8 TBuffer;
    typedef TAGGScanlineRenderer<TBuffer::TPixFmt> TRenderer;

    typedef TSharedPtr<TBuffer> TPSBuffer;
    typedef TSharedPtr<TRenderer> TPSRenderer;

	UPROPERTY(Transient)
    UAGGScanlineRenderer* Renderer;

protected:

    TPSBuffer ContextBuffer;
    TPSRenderer ContextRenderer;

public:

    UFUNCTION(BlueprintCallable, Category = "AGG")
    virtual UAGGScanlineRenderer* GetRendererController()
    {
        return Renderer;
    }

    // BEGIN UAGGContext Interface

    virtual void InitContext()
    {
        ContextBuffer = MakeShareable( new TBuffer() );
        ContextRenderer = MakeShareable( new TRenderer() );

        if (! Renderer)
        {
            Renderer = NewObject<UAGGScanlineRenderer>(this);
        }

        if (! PathController)
        {
            PathController = NewObject<UAGGPathController>(this);
        }
    }

    virtual void ClearContext()
    {
        ContextBuffer.Reset();
        ContextRenderer.Reset();
        Renderer = nullptr;
        PathController = nullptr;
    }

    virtual void InitBuffer(int32 w, int32 h, int32 c, bool bSq)
    {
        check(ContextBuffer.IsValid());
        check(ContextRenderer.IsValid());
        check(Renderer);

        ContextBuffer->Init(w, h, c, bSq);
        ContextRenderer->ConstructRenderer( *ContextBuffer->GetPixFmt() );
        Renderer->SetRenderer( &ContextRenderer->Renderer );
    }

    virtual void ClearBuffer(uint8 ClearVal)
    {
        if (ContextBuffer.IsValid())
            ContextBuffer->Clear(0);
    }

    virtual void RenderContext()
    {
        check(ContextBuffer.IsValid());
        check(ContextBuffer->IsValid());
        check(ContextRenderer.IsValid());
        check(Renderer);

        ContextRenderer->AddPath(**GetPathController());
        ContextRenderer->Render();
    }

    virtual IAGGRenderBuffer* GetBuffer()
    {
        return ContextBuffer.Get();
    }

    virtual const IAGGRenderBuffer* GetBuffer() const
    {
        return ContextBuffer.Get();
    }

    virtual EPixelFormat GetPixelFormat()
    {
        return EPixelFormat::PF_G8;
    }

    virtual void SetColor(const FColor& Color)
    {
        check(Renderer);
        return Renderer->SetColor(Color);
    }

    virtual void SetColorByte(uint8 Color)
    {
        check(Renderer);
        return Renderer->SetColorByte(Color);
    }

    // END UAGGContext Interface

};
