// 

#pragma once

#include "CoreUObject.h"
#include "SharedPointer.h"

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

    template<class TAGGRenderer>
    struct FRenderProxy : public IRenderProxy
    {
        TAGGRenderer* Renderer;

        FRenderProxy(TAGGRenderer* c)
        {
            Renderer = c;
        }

        virtual void SetColor(const FColor& c) override
        {
            if (Renderer)
                Renderer->color(agg::rgba8(c.R, c.G, c.B, c.A));
        }
    };

    template<class TAGGRenderer>
    void SetRenderer(TAGGRenderer* Renderer)
    {
        if (RenderProxy.IsValid())
            RenderProxy.Reset();
        RenderProxy = MakeShareable( new FRenderProxy<TAGGRenderer>(Renderer) );
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
