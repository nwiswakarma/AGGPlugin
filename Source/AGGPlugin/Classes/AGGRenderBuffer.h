// 

#pragma once

#include "agg_rendering_buffer.h"

#include "UnrealMathUtility.h"
#include "GenericPlatformMath.h"
#include "SharedPointer.h"
#include "Engine/Texture2D.h"

class IAGGRenderBuffer
{

public:

    typedef TArray<uint8> FByteBuffer;
    typedef uint8* TRawBuffer;

    // Apply default virtual destructor
	virtual ~IAGGRenderBuffer() = default;

    virtual void InitPixFmt() = 0;

	virtual void Init(int32 InBufferW, int32 InBufferH, uint8 InClearVal=0, bool bSquareSize = false)
    {
        check(InBufferW > 1 || InBufferH > 1);

        if (IsValid())
        {
            Reset();
        }

        InitPixFmt();

        if (bSquareSize)
        {
            int32 d = FMath::Max(InBufferW, InBufferH);
            d = FGenericPlatformMath::RoundUpToPowerOfTwo(d);

            BufferWidth = d;
            BufferHeight = d;
        }

        BufferWidth = InBufferW;
        BufferHeight = InBufferH;

        Buffer.SetNumZeroed(CalcBufferSize());
        AGGBuffer.attach(Buffer.GetData(), BufferWidth, BufferHeight, GetStride());
        Clear(InClearVal);
    }

	virtual void Reset()
    {
        Buffer.Empty();
        BufferWidth = -1;
        BufferHeight = -1;
        BufferBPP = -1;
        AGGBuffer = agg::rendering_buffer();
    }

    // Query Operations

	FORCEINLINE bool IsValid() const
    {
        return Buffer.Num() == CalcBufferSize();
    }

    FORCEINLINE int32 GetWidth() const
    {
        return BufferWidth;
    }

    FORCEINLINE int32 GetHeight() const
    {
        return BufferHeight;
    }

    FORCEINLINE int32 GetStride() const
    {
        return BufferWidth*BufferBPP;
    }

    FORCEINLINE int32 GetBufferSize() const
    {
        return Buffer.Num();
    }

    FORCEINLINE int32 CalcBufferSize() const
    {
        return BufferHeight*GetStride();
    }

    FORCEINLINE agg::rendering_buffer& GetAGGBuffer()
    {
        return AGGBuffer;
    }

    // Buffer Operations

	FORCEINLINE const FByteBuffer& GetByteBuffer() const
    {
        return Buffer;
    }

	FORCEINLINE uint32 GetTypeSize() const
    {
        return sizeof(uint8);
    }

	FORCEINLINE uint8 GetByteAt(int32 X, int32 Y) const
    {
        return (X >= 0 && X < BufferWidth && Y >= 0 && Y < BufferHeight)
            ? Buffer[Y*GetStride()+X*BufferBPP]
            : 0;
    }

	FORCEINLINE void Clear(uint8 ClrVal)
    {
        if (IsValid())
            FMemory::Memset(Buffer.GetData(), ClrVal, GetBufferSize());
    }

	FORCEINLINE bool CopyFrom(TRawBuffer InBuffer)
    {
        if (IsValid())
        {
            FMemory::Memcpy(Buffer.GetData(), InBuffer, GetBufferSize());
            return true;
        }
        return false;
    }

	FORCEINLINE void CopyFromUnsafe(TRawBuffer InBuffer)
    {
        FMemory::Memcpy(Buffer.GetData(), InBuffer, GetBufferSize());
    }

	FORCEINLINE bool CopyTo(TRawBuffer OutBuffer) const
    {
        if (IsValid())
        {
            FMemory::Memcpy(OutBuffer, Buffer.GetData(), GetBufferSize());
            return true;
        }
        return false;
    }

	FORCEINLINE void CopyToUnsafe(TRawBuffer OutBuffer) const
    {
        FMemory::Memcpy(OutBuffer, Buffer.GetData(), GetBufferSize());
    }

	FORCEINLINE bool CopyFrom(UTexture2D* Tex, int32 MipLevel=0)
    {
        if (IsValid() && Tex)
        {
            FTexture2DMipMap& Mip( Tex->PlatformData->Mips[MipLevel] );
            void* InData( Mip.BulkData.Lock(LOCK_READ_ONLY) );
            CopyFrom( static_cast<TRawBuffer>(InData) );
            Mip.BulkData.Unlock();
            return true;
        }
        return false;
    }

	FORCEINLINE bool CopyTo(UTexture2D* Tex, int32 MipLevel=0) const
    {
        if (IsValid() && Tex)
        {
            FTexture2DMipMap& Mip( Tex->PlatformData->Mips[MipLevel] );
            void* OutData( Mip.BulkData.Lock(LOCK_READ_WRITE) );
            CopyTo( static_cast<TRawBuffer>(OutData) );
            Mip.BulkData.Unlock();
            return true;
        }
        return false;
    }

	FORCEINLINE UTexture2D* CreateTransientTexture(EPixelFormat fmt) const
    {
        return IsValid()
            ? UTexture2D::CreateTransient(GetWidth(), GetHeight(), fmt)
            : nullptr;
    }

protected:

	FByteBuffer Buffer;
	int32 BufferWidth;
	int32 BufferHeight;
    int32 BufferBPP;
    agg::rendering_buffer AGGBuffer;

    // Hidden Constructor
	IAGGRenderBuffer() = default;

private:

    // Non-Copyable
    IAGGRenderBuffer(const IAGGRenderBuffer&);
    const IAGGRenderBuffer& operator=(const IAGGRenderBuffer&);

};

template<class TPixelFormat> class TAGGRenderBuffer : public IAGGRenderBuffer
{

public:

    typedef TPixelFormat TPixFmt;
    typedef TSharedPtr<TPixFmt> TPSPixFmt;

	TAGGRenderBuffer()
	{
	}

	TAGGRenderBuffer(int32 InBufferD, uint8 InClearVal=0, bool bSquareSize=false)
	{
        Init(InBufferD, InBufferD, InClearVal, bSquareSize);
	}

	TAGGRenderBuffer(int32 InBufferW, int32 InBufferH, uint8 InClearVal=0, bool bSquareSize=false)
	{
        Init(InBufferW, InBufferH, InClearVal, bSquareSize);
	}

	virtual void InitPixFmt()
    {
        BufferBPP = TPixFmt::pix_width;
        PixFmt = MakeShareable( new TPixFmt(AGGBuffer) );
    }

	virtual void Reset()
    {
        PixFmt.Reset();
        IAGGRenderBuffer::Reset();
    }

    FORCEINLINE TPixFmt* GetPixFmt()
    {
        return PixFmt.IsValid() ? PixFmt.Get() : nullptr;
    }

private:

    TPSPixFmt PixFmt;

    // Non-Copyable
    TAGGRenderBuffer(const TAGGRenderBuffer<TPixFmt>&);
    const TAGGRenderBuffer<TPixFmt>& operator = (
        const TAGGRenderBuffer<TPixFmt>&
    );

};
