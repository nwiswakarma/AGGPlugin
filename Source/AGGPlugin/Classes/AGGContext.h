// 

#pragma once

#include "CoreUObject.h"
#include "SharedPointer.h"
#include "Engine/Texture2D.h"

#include "AGGTypes.h"
#include "AGGPathController.h"
#include "AGGContext.generated.h"

UCLASS(Abstract)
class AGGPLUGIN_API UAGGContext : public UObject
{
	GENERATED_BODY()

public:

    // BEGIN UObject Interface

    virtual void PostInitProperties() override
    {
        Super::PostInitProperties();
        InitContext();
    }

    virtual void BeginDestroy() override
    {
        ClearContext();
        Super::BeginDestroy();
    }

    // END UObject Interface

    // BEGIN UAGGContext Interface

    // Called by CDO, must not be pure virtual
    virtual void InitContext() { }

    // Called by CDO, must not be pure virtual
    virtual void ClearContext() { }

    virtual void InitBuffer(int32 w, int32 h, int32 c, bool bSq)
        PURE_VIRTUAL(UAGGContext::InitBuffer, );

    virtual void ClearBuffer(uint8 ClearVal)
        PURE_VIRTUAL(UAGGContext::ClearBuffer, );

    UFUNCTION(BlueprintCallable, Category = "AGG")
    virtual void RenderContext()
        PURE_VIRTUAL(UAGGContext::RenderContext, );

    virtual IAGGRenderBuffer* GetBuffer()
        PURE_VIRTUAL(UAGGContext::GetBuffer, return nullptr;);

    virtual const IAGGRenderBuffer* GetBuffer() const
        PURE_VIRTUAL(UAGGContext::GetBuffer, return nullptr;);

    virtual EPixelFormat GetPixelFormat()
        PURE_VIRTUAL(
            UAGGContext::GetPixelFormat,
            return EPixelFormat::PF_Unknown;
        );

    virtual void SetColor(const FColor& Color) { }

    virtual void SetColorByte(uint8 Color) { }

    FORCEINLINE bool HasValidBuffer() const
    {
        if (const IAGGRenderBuffer* b = GetBuffer())
        {
            return b->IsValid();
        }
        return false;
    }

    uint8 GetByteAt(int32 X, int32 Y) const
    {
        if (HasValidBuffer())
        {
            return GetBuffer()->GetByteAt(X, Y);
        }
        return 0;
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    uint8 GetByteAtUnsafe(int32 X, int32 Y) const
    {
        return GetBuffer()->GetByteAt(X, Y);
    }

    // END UAGGContext Interface

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AGG")
    UAGGPathController* GetPathController()
    {
        return PathController;
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void ClearPath()
    {
        if (PathController)
            PathController->Clear();
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    bool Blueprint_HasValidBuffer() const
    {
        return HasValidBuffer();
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    uint8 Blueprint_GetByteAt(int32 X, int32 Y) const
    {
        return GetByteAt(X, Y);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    uint8 Blueprint_GetByteAtUnsafe(int32 X, int32 Y) const
    {
        return GetByteAtUnsafe(X, Y);
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AGG")
    const TArray<uint8>& GetByteBuffer() const
    {
        return GetBuffer()->GetByteBuffer();
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void ConstructBuffer(int32 InBufferW, int32 InBufferH, uint8 InClearVal = 0, bool bSquareSize = false)
    {
        InitBuffer(InBufferW, InBufferH, InClearVal, bSquareSize);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void ClearBuffer()
    {
        ClearBuffer(0);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    UTexture2D* CreateTexture(bool bSRGB = false)
    {
        if (! HasValidBuffer())
        {
            return nullptr;
        }

        IAGGRenderBuffer& Buffer( *GetBuffer() );

        // Generates transient texture
        UTexture2D*	texture = Buffer.CreateTransientTexture( GetPixelFormat() );
        texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        texture->SRGB = bSRGB ? 1 : 0;

        // Flush buffer data
        Buffer.CopyTo(texture);

        // Update texture resource
        texture->UpdateResource();

        return texture;
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void CopyFromTexture(UTexture2D* Texture)
    {
        if (Texture && HasValidBuffer())
        {
            GetBuffer()->CopyFrom(Texture);
        }
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void CopyToTexture(UTexture2D* Texture)
    {
        if (Texture && HasValidBuffer())
        {
            GetBuffer()->CopyTo(Texture);
            Texture->UpdateResource();
        }
    }

    //FORCEINLINE IAGGRenderBuffer::TRawBuffer GetRawBuffer()
    //{
    //    return GetBuffer()->GetRawBuffer();
    //}

    FORCEINLINE int32 GetBufferSize()
    {
        return GetBuffer()->GetBufferSize();
    }

    FORCEINLINE int32 GetBufferTypeSize()
    {
        return GetBuffer()->GetTypeSize();
    }

    //FORCEINLINE bool FlushBuffer(IAGGRenderBuffer::TRawBuffer OutBuffer)
    //{
    //    return GetBuffer()->Flush( OutBuffer );
    //}

    //FORCEINLINE bool CopyBuffer(IAGGRenderBuffer::TRawBuffer OutBuffer)
    //{
    //    return GetBuffer()->CopyTo( OutBuffer );
    //}

protected:

	UPROPERTY(Transient)
    UAGGPathController* PathController;

};
