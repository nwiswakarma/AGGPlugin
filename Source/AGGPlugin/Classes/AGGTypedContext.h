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

#include "AGGTypes.h"
#include "AGGContext.h"
#include "AGGRendererObject.h"
#include "AGGPathController.h"
#include "AGGTypedContext.generated.h"

typedef class TAGGRenderBuffer< FAGGPFG8     > FAGGBufferG8;
typedef class TAGGRenderBuffer< FAGGPFBGRA32 > FAGGBufferBGRA32;

UCLASS(BlueprintType, Blueprintable)
class AGGPLUGIN_API UAGGContextG8 : public UAGGContext
{
    GENERATED_BODY()

protected:

    typedef FAGGBufferG8 FBuffer;

    TSharedPtr<FBuffer> ContextBuffer;

public:

    virtual void InitContext() override
    {
        UAGGContext::InitContext();

        ContextBuffer = MakeShareable(new FBuffer());
    }

    virtual void ClearContext() override
    {
        if (ContextBuffer.IsValid())
        {
            ContextBuffer->Reset();
            ContextBuffer.Reset();
        }

        UAGGContext::ClearContext();
    }

    virtual void InitBuffer(int32 w, int32 h, int32 c, bool bSq) override
    {
        check(ContextBuffer.IsValid());

        ContextBuffer->Init(w, h, c, bSq);
    }

    virtual void ClearBuffer(uint8 ClearVal) override
    {
        if (ContextBuffer.IsValid())
        {
            ContextBuffer->Clear(0);
        }
    }

    FORCEINLINE virtual IAGGRenderBuffer* GetBuffer() override
    {
        return ContextBuffer.Get();
    }

    FORCEINLINE virtual const IAGGRenderBuffer* GetBuffer() const override
    {
        return ContextBuffer.Get();
    }

    virtual EPixelFormat GetPixelFormat() const override
    {
        return EPixelFormat::PF_G8;
    }
};

UCLASS(BlueprintType, Blueprintable)
class AGGPLUGIN_API UAGGContextBGRA : public UAGGContext
{
    GENERATED_BODY()

protected:

    typedef FAGGBufferBGRA32 FBuffer;

    TSharedPtr<FBuffer> ContextBuffer;

public:

    virtual void InitContext() override
    {
        UAGGContext::InitContext();

        ContextBuffer = MakeShareable(new FBuffer());
    }

    virtual void ClearContext() override
    {
        if (ContextBuffer.IsValid())
        {
            ContextBuffer->Reset();
            ContextBuffer.Reset();
        }

        UAGGContext::ClearContext();
    }

    virtual void InitBuffer(int32 w, int32 h, int32 c, bool bSq) override
    {
        check(ContextBuffer.IsValid());

        ContextBuffer->Init(w, h, c, bSq);
    }

    virtual void ClearBuffer(uint8 ClearVal) override
    {
        if (ContextBuffer.IsValid())
        {
            ContextBuffer->Clear(0);
        }
    }

    FORCEINLINE virtual IAGGRenderBuffer* GetBuffer() override
    {
        return ContextBuffer.Get();
    }

    FORCEINLINE virtual const IAGGRenderBuffer* GetBuffer() const override
    {
        return ContextBuffer.Get();
    }

    virtual EPixelFormat GetPixelFormat() const override
    {
        return EPixelFormat::PF_B8G8R8A8;
    }
};
