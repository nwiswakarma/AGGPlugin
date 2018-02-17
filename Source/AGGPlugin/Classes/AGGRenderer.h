// 

#pragma once

#include "agg_path_storage.h"

#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"

#include "AGGTypes.h"

template<class TPixFmt>
class TAGGRendererBase
{
protected:
    typedef agg::renderer_base<TPixFmt> TBaseRenderer;

public:

    TBaseRenderer BaseRenderer;

    // Apply default virtual destructor
	virtual ~TAGGRendererBase() = default;

    virtual void ConstructRenderer(TPixFmt& pixfmt)
    {
        BaseRenderer.attach(pixfmt);
    }
};

template<class TPixFmt>
class TAGGScanlineRenderer : public TAGGRendererBase<TPixFmt>
{
    typedef TAGGRendererBase<TPixFmt>::TBaseRenderer TBaseRenderer;

protected:

    typedef agg::renderer_scanline_aa_solid<TBaseRenderer> TRenderer;
    typedef agg::rasterizer_scanline_aa<> TRasterizer;

public:

    TRenderer Renderer;
    TRasterizer Rasterizer;
    agg::scanline_p8 Scanline;

    virtual void ConstructRenderer(TPixFmt& pixfmt) override
    {
        TAGGRendererBase<TPixFmt>::ConstructRenderer(pixfmt);
        Renderer.attach(BaseRenderer);
    }

    virtual void AddPath(agg::path_storage& Path)
    {
        Rasterizer.add_path(Path);
    }

    virtual void Render()
    {
        agg::render_scanlines(Rasterizer, Scanline, Renderer);
    }
};
