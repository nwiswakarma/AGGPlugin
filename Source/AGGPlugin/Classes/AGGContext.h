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
#include "Engine/Texture2D.h"

#include "AGGTypes.h"
#include "AGGPathController.h"
#include "AGGRenderBuffer.h"
#include "AGGContext.generated.h"

USTRUCT(BlueprintType)
struct AGGPLUGIN_API FAGGByteBufferRef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
    TArray<uint8> ByteBuffer;
};

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

    virtual void InitContext()
    {
        if (! PathController)
        {
            PathController = NewObject<UAGGPathController>(this);
        }
    }

    virtual void ClearContext()
    {
        if (PathController)
        {
            PathController->Clear();
            PathController = nullptr;
        }
    }

    virtual void InitBuffer(int32 w, int32 h, int32 c, bool bSq)
        PURE_VIRTUAL(UAGGContext::InitBuffer, );

    virtual void ClearBuffer(uint8 ClearVal)
        PURE_VIRTUAL(UAGGContext::ClearBuffer, );

    UFUNCTION(BlueprintCallable, Category="AGG")
    virtual void RenderContext()
        PURE_VIRTUAL(UAGGContext::RenderContext, );

    virtual IAGGRenderBuffer* GetBuffer()
        PURE_VIRTUAL(UAGGContext::GetBuffer, return nullptr;);

    virtual const IAGGRenderBuffer* GetBuffer() const
        PURE_VIRTUAL(UAGGContext::GetBuffer, return nullptr;);

    UFUNCTION(BlueprintCallable, Category="AGG")
    virtual EPixelFormat GetPixelFormat() const
        PURE_VIRTUAL(
            UAGGContext::GetPixelFormat,
            return EPixelFormat::PF_Unknown;
        );

    UFUNCTION(BlueprintCallable, Category="AGG")
    EAGGPixFmt GetAGGPixFmt() const
    {
        return FAGGTypeUtility::GetAGGPixFmt(GetPixelFormat());
    }

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

    FORCEINLINE uint8 GetByteAt(int32 X, int32 Y) const
    {
        if (HasValidBuffer())
        {
            return GetBuffer()->GetByteAt(X, Y);
        }
        return 0;
    }

    FORCEINLINE uint8 GetByteAtUnsafe(int32 X, int32 Y) const
    {
        return GetBuffer()->GetByteAt(X, Y);
    }

    // END UAGGContext Interface

    FORCEINLINE int32 GetBufferSize()
    {
        return GetBuffer()->GetBufferSize();
    }

    FORCEINLINE int32 GetBufferTypeSize()
    {
        return GetBuffer()->GetTypeSize();
    }

    // ~ BLUEPRINT FUNCTIONS

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="AGG")
    UAGGPathController* GetPathController()
    {
        return PathController;
    }

    UFUNCTION(BlueprintCallable, Category="AGG")
    void ClearPath()
    {
        if (PathController)
            PathController->Clear();
    }

    UFUNCTION(BlueprintCallable, Category="AGG", meta=(DisplayName="HasValidBuffer"))
    bool K2_HasValidBuffer() const
    {
        return HasValidBuffer();
    }

    UFUNCTION(BlueprintCallable, Category="AGG")
    void K2_ClearContext()
    {
        ClearContext();
    }

    UFUNCTION(BlueprintCallable, Category="AGG")
    void K2_ClearBuffer()
    {
        ClearBuffer(0);
    }

    UFUNCTION(BlueprintCallable, Category="AGG", meta=(DisplayName="GetByteAt"))
    uint8 K2_GetByteAt(int32 X, int32 Y) const
    {
        return GetByteAt(X, Y);
    }

    UFUNCTION(BlueprintCallable, Category="AGG", meta=(DisplayName="GetByteAtUnsafe"))
    uint8 K2_GetByteAtUnsafe(int32 X, int32 Y) const
    {
        return GetByteAtUnsafe(X, Y);
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="AGG")
    const TArray<uint8>& GetByteBuffer() const
    {
        return GetBuffer()->GetByteBuffer();
    }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="AGG")
    void CopyByteBuffer(FAGGByteBufferRef& ByteBufferRef) const
    {
        ByteBufferRef.ByteBuffer = GetBuffer()->GetByteBuffer();
    }

    UFUNCTION(BlueprintCallable, Category="AGG")
    void ConstructBuffer(int32 InBufferW, int32 InBufferH, uint8 InClearVal = 0, bool bSquareSize = false)
    {
        InitBuffer(InBufferW, InBufferH, InClearVal, bSquareSize);
    }

    UFUNCTION(BlueprintCallable, Category="AGG")
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

    UFUNCTION(BlueprintCallable, Category="AGG")
    UTexture2D* CreateTextureWithFilterType(TextureFilter FilterType, bool bSRGB = false)
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
        texture->Filter = FilterType;

        // Flush buffer data
        Buffer.CopyTo(texture);

        // Update texture resource
        texture->UpdateResource();

        return texture;
    }

    UFUNCTION(BlueprintCallable, Category="AGG")
    void CopyFromByteBuffer(const TArray<uint8>& ByteBuffer)
    {
        if (HasValidBuffer())
        {
            IAGGRenderBuffer& Buffer( *GetBuffer() );

            // Copy buffer if size matches
            if (Buffer.GetBufferSize() == ByteBuffer.Num())
            {
                Buffer.CopyFrom(ByteBuffer);
            }
        }
    }

    UFUNCTION(BlueprintCallable, Category="AGG")
    void CopyFromTexture(UTexture2D* Texture)
    {
        if (IsValid(Texture) && HasValidBuffer())
        {
            GetBuffer()->CopyFrom(Texture);
        }
    }

    UFUNCTION(BlueprintCallable, Category="AGG")
    void CopyToTexture(UTexture2D* Texture)
    {
        if (IsValid(Texture) && HasValidBuffer())
        {
            GetBuffer()->CopyTo(Texture);
            Texture->UpdateResource();
        }
    }

    UFUNCTION(BlueprintCallable, Category="AGG")
    virtual void CopyAlphaFromBuffer(const TArray<uint8>& AlphaBuffer)
    {
    }

protected:

	UPROPERTY(Transient)
    UAGGPathController* PathController;

};
