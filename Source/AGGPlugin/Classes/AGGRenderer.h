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

#include "agg_path_storage.h"
#include "agg_rendering_buffer.h"
#include "agg_scanline_p.h"
#include "agg_scanline_u.h"
#include "agg_scanline_bin.h"
#include "agg_renderer_scanline.h"
#include "agg_renderer_outline_aa.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_rasterizer_outline_aa.h"

#include "AGGTypes.h"
#include "AGGRenderBuffer.h"
#include "AGGPathController.h"

template<class FPixFmtType>
class AGGPLUGIN_API TAGGRendererBase
{
public:

    typedef FPixFmtType FPixFmt;

protected:

    typedef agg::renderer_base<FPixFmt> FBaseRenderer;
    FPixFmt* PixFmt = nullptr;

public:

    FBaseRenderer BaseRenderer;

	TAGGRendererBase() = default;

	virtual ~TAGGRendererBase()
    {
        ClearPixFmt();
        check(PixFmt == nullptr);
    }

    void ClearPixFmt()
    {
        if (PixFmt)
        {
            delete PixFmt;
            PixFmt = nullptr;
        }
    }

    virtual void Attach(agg::rendering_buffer& buf)
    {
        ClearPixFmt();
        PixFmt = new FPixFmt(buf);
        BaseRenderer.attach(*PixFmt);
    }
};

// Renderer Scanline

template<class FPixFmtType>
class AGGPLUGIN_API TAGGRendererScanline : public TAGGRendererBase<FPixFmtType>
{
protected:

    typedef agg::rasterizer_scanline_aa<> FRasterizer;

public:

    FRasterizer Rasterizer;
    agg::rgba8 Color;

    FORCEINLINE void SetColor(uint8 v)
    {
        Color = agg::rgba8(v, v, v, v);
    }

    FORCEINLINE void SetColor(FColor c)
    {
        Color = agg::rgba8(c.R, c.G, c.B, c.A);
    }

    FORCEINLINE void ResetPath()
    {
        Rasterizer.reset_clipping();
    }

    FORCEINLINE void AddPath(agg::path_storage& Path)
    {
        Rasterizer.add_path(Path);
    }

    FORCEINLINE void SetPath(agg::path_storage& Path)
    {
        ResetPath();
        AddPath(Path);
    }

    FORCEINLINE void Render(agg::path_storage& Path, FColor InColor, EAGGScanline ScanlineType)
    {
        SetPath(Path);
        SetColor(InColor);

        switch (ScanlineType)
        {
            case EAGGScanline::SL_P8:  RenderP8();  break;
            case EAGGScanline::SL_U8:  RenderU8();  break;
            case EAGGScanline::SL_Bin: RenderBin(); break;
        }
    }

    FORCEINLINE void RenderP8()
    {
        agg::scanline_p8 Scanline;
        agg::render_scanlines_aa_solid(Rasterizer, Scanline, BaseRenderer, Color);
    }

    FORCEINLINE void RenderU8()
    {
        agg::scanline_u8 Scanline;
        agg::render_scanlines_aa_solid(Rasterizer, Scanline, BaseRenderer, Color);
    }

    FORCEINLINE void RenderBin()
    {
        agg::scanline_bin Scanline;
        agg::render_scanlines_bin_solid(Rasterizer, Scanline, BaseRenderer, Color);
    }
};

// Renderer Outline

template<class FPixFmtType>
class AGGPLUGIN_API TAGGRendererOutline : public TAGGRendererBase<FPixFmtType>
{
private:

    TAGGRendererOutline(const TAGGRendererOutline&);
    const TAGGRendererOutline& operator=(const TAGGRendererOutline&);

protected:

    typedef agg::renderer_outline_aa<FBaseRenderer> FRenderer;
    typedef agg::rasterizer_outline_aa<FRenderer>   FRasterizer;

public:

    FRenderer*   Renderer;
    FRasterizer* Rasterizer;

    bool        bClosePolygon;
    agg::rgba8  Color;
    agg::line_profile_aa Profile;

	TAGGRendererOutline()
    {
        Renderer   = new FRenderer(BaseRenderer, Profile);
        Rasterizer = new FRasterizer(*Renderer);
    }

	virtual ~TAGGRendererOutline()
    {
        check(Renderer   != nullptr);
        check(Rasterizer != nullptr);

        delete Renderer;
        delete Rasterizer;
    }

    FORCEINLINE void SetColor(uint8 v)
    {
        Color = agg::rgba8(v, v, v, v);
    }

    FORCEINLINE void SetColor(FColor c)
    {
        Color = agg::rgba8(c.R, c.G, c.B, c.A);
    }

    FORCEINLINE void SetClosePolygon(bool bInClosePolygon)
    {
        bClosePolygon = bInClosePolygon;
    }

    FORCEINLINE void SetLineProfile(FAGGOutlineAALineProfile LineProfile)
    {
        Profile.smoother_width(LineProfile.SmootherWidth);
        Profile.width(LineProfile.Width);

        switch (LineProfile.LineJoin)
        {
            case EAGGOutlineAALineJoin::OUTLINE_NO_JOIN:
                Rasterizer->line_join(agg::outline_no_join);
                break;

            case EAGGOutlineAALineJoin::OUTLINE_MITER_JOIN:
                Rasterizer->line_join(agg::outline_miter_join);
                break;

            case EAGGOutlineAALineJoin::OUTLINE_ROUND_JOIN:
                Rasterizer->line_join(agg::outline_round_join);
                break;

            case EAGGOutlineAALineJoin::OUTLINE_MITER_ACCURATE_JOIN:
                Rasterizer->line_join(agg::outline_miter_accurate_join);
                break;
        }

        Rasterizer->round_cap(LineProfile.bRoundCap);
    }

    FORCEINLINE void AddPath(agg::path_storage& Path)
    {
        Rasterizer->add_path(Path);
    }

    FORCEINLINE void AddPath(agg::path_storage& Path, bool bInClosePolygon)
    {
        Rasterizer->add_path(Path);
        SetClosePolygon(bInClosePolygon);
    }

    FORCEINLINE void Render(agg::path_storage& Path, FColor InColor, bool bInClosePolygon)
    {
        AddPath(Path);
        SetColor(InColor);
        SetClosePolygon(bInClosePolygon);
        Render();
    }

    FORCEINLINE void Render(agg::path_storage& Path)
    {
        AddPath(Path);
        Render();
    }

    FORCEINLINE void Render()
    {
        Rasterizer->render(bClosePolygon);
    }
};

// Typed Renderers - Renderer Scanline

class AGGPLUGIN_API FAGGRendererScanlineG8          : public TAGGRendererScanline<FAGGPFG8>     { };
class AGGPLUGIN_API FAGGRendererScanlineBGRA        : public TAGGRendererScanline<FAGGPFBGRA32> { };

class AGGPLUGIN_API FAGGRendererScanlineAlphaBlendR : public TAGGRendererScanline<FAGGPFAlphaBlendR> { };
class AGGPLUGIN_API FAGGRendererScanlineAlphaBlendG : public TAGGRendererScanline<FAGGPFAlphaBlendG> { };
class AGGPLUGIN_API FAGGRendererScanlineAlphaBlendB : public TAGGRendererScanline<FAGGPFAlphaBlendB> { };

// Typed Renderers - Renderer Outline

class AGGPLUGIN_API FAGGRendererOutlineG8           : public TAGGRendererOutline<FAGGPFG8>     { };
class AGGPLUGIN_API FAGGRendererOutlineBGRA         : public TAGGRendererOutline<FAGGPFBGRA32> { };

class AGGPLUGIN_API FAGGRendererOutlineAlphaBlendR  : public TAGGRendererOutline<FAGGPFAlphaBlendR> { };
class AGGPLUGIN_API FAGGRendererOutlineAlphaBlendG  : public TAGGRendererOutline<FAGGPFAlphaBlendG> { };
class AGGPLUGIN_API FAGGRendererOutlineAlphaBlendB  : public TAGGRendererOutline<FAGGPFAlphaBlendB> { };
