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

#include "agg_rendering_buffer.h"

#include "UnrealMathUtility.h"
#include "GenericPlatformMath.h"
#include "SharedPointer.h"
#include "Engine/Texture2D.h"

#include "AGGTypes.h"

class AGGPLUGIN_API IAGGRenderBuffer
{

public:

    typedef TArray<uint8> FByteBuffer;
    typedef uint8* FRawBuffer;

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

    FORCEINLINE int32 GetBPP() const
    {
        return BufferBPP;
    }

    FORCEINLINE SIZE_T GetBufferSize() const
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

	FORCEINLINE FByteBuffer& GetByteBuffer()
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

	FORCEINLINE bool CopyFrom(FRawBuffer InBuffer)
    {
        if (IsValid())
        {
            FMemory::Memcpy(Buffer.GetData(), InBuffer, GetBufferSize());
            return true;
        }
        return false;
    }

	FORCEINLINE bool CopyFrom(const FByteBuffer& InBuffer)
    {
        if (IsValid())
        {
            FMemory::Memcpy(Buffer.GetData(), InBuffer.GetData(), GetBufferSize());
            return true;
        }
        return false;
    }

	FORCEINLINE void CopyFromUnsafe(FRawBuffer InBuffer)
    {
        FMemory::Memcpy(Buffer.GetData(), InBuffer, GetBufferSize());
    }

	FORCEINLINE bool CopyTo(FRawBuffer OutBuffer) const
    {
        if (IsValid())
        {
            FMemory::Memcpy(OutBuffer, Buffer.GetData(), GetBufferSize());
            return true;
        }
        return false;
    }

	FORCEINLINE void CopyToUnsafe(FRawBuffer OutBuffer) const
    {
        FMemory::Memcpy(OutBuffer, Buffer.GetData(), GetBufferSize());
    }

	FORCEINLINE bool CopyFrom(UTexture2D* Tex, int32 MipLevel=0)
    {
        if (IsValid() && Tex)
        {
            FTexture2DMipMap& Mip( Tex->PlatformData->Mips[MipLevel] );
            void* InData( Mip.BulkData.Lock(LOCK_READ_ONLY) );
            CopyFrom( static_cast<FRawBuffer>(InData) );
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
            CopyTo( static_cast<FRawBuffer>(OutData) );
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

template<class FPixFmtType>
class AGGPLUGIN_API TAGGRenderBuffer : public IAGGRenderBuffer
{

public:

    typedef FPixFmtType FPixFmt;
    typedef TSharedPtr<FPixFmt> FPSPixFmt;

	TAGGRenderBuffer() = default;

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
        BufferBPP = FPixFmt::pix_width;
    }

	virtual void Reset()
    {
        IAGGRenderBuffer::Reset();
    }

private:

    // Non-Copyable
    TAGGRenderBuffer(const TAGGRenderBuffer<FPixFmt>&) = delete;
    const TAGGRenderBuffer<FPixFmt>& operator=(const TAGGRenderBuffer<FPixFmt>&) = delete;

};
