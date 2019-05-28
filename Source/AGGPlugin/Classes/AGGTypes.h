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

#include "agg_pixfmt_gray.h"
#include "agg_pixfmt_rgb.h"
#include "agg_pixfmt_rgba.h"
#include "AGGTypes.generated.h"

typedef agg::pixfmt_gray8  FAGGPFG8;
typedef agg::pixfmt_bgra32 FAGGPFBGRA32;
typedef agg::pixfmt_bgra32_plain FAGGPFBGRA32_Plain;

typedef agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::rendering_buffer, 4, 3> FAGGPFAlphaBlendA;
typedef agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::rendering_buffer, 4, 2> FAGGPFAlphaBlendR;
typedef agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::rendering_buffer, 4, 1> FAGGPFAlphaBlendG;
typedef agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::rendering_buffer, 4, 0> FAGGPFAlphaBlendB;

UENUM(BlueprintType)
enum class EAGGPixFmt : uint8
{
    PF_Unknown,
    PF_G8,
    PF_BGRA32,

    PF_AlphaBlendR,
    PF_AlphaBlendG,
    PF_AlphaBlendB
};

UENUM(BlueprintType)
enum class EAGGScanline : uint8
{
    SL_Unknown,
    SL_P8,
    SL_U8,
    SL_Bin
};

UENUM(BlueprintType)
enum class EAGGOutlineAALineJoin : uint8
{
    OUTLINE_NO_JOIN,
    OUTLINE_MITER_JOIN,
    OUTLINE_ROUND_JOIN,
    OUTLINE_MITER_ACCURATE_JOIN
};

USTRUCT(BlueprintType)
struct FAGGOutlineAALineProfile
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    float SmootherWidth = 1.f;

    UPROPERTY(BlueprintReadWrite)
    float Width = 1.f;

    UPROPERTY(BlueprintReadWrite)
    EAGGOutlineAALineJoin LineJoin = EAGGOutlineAALineJoin::OUTLINE_ROUND_JOIN;

    UPROPERTY(BlueprintReadWrite)
    bool bRoundCap = false;
};

class FAGGTypeUtility
{
public:

    FORCEINLINE static EAGGPixFmt GetAGGPixFmt(EPixelFormat PixelFormat)
    {
        switch (PixelFormat)
        {
            case EPixelFormat::PF_G8:       return EAGGPixFmt::PF_G8;
            case EPixelFormat::PF_B8G8R8A8: return EAGGPixFmt::PF_BGRA32;
        }

        return EAGGPixFmt::PF_Unknown;
    }
};
