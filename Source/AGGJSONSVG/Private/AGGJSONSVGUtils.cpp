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

#include "AGGJSONSVGUtils.h"
#include "GenericPlatformMath.h"

DEFINE_LOG_CATEGORY(LogAGGJSVG);
DEFINE_LOG_CATEGORY(UntAGGJSVG);

void UAGGJSONSVGUtils::DrawJSONSVG(const FJSONSVGDoc& SVGDoc, UAGGContext* Context, UAGGRendererBase* Renderer, const FIntPoint& DocSize)
{
    if (! SVGDoc.IsValid() || ! IsValid(Context) || ! IsValid(Renderer))
    {
        UE_LOG(LogAGGJSVG,Warning, TEXT("DrawJSONSVG() ABORTED, INVALID PARAMETERS"));
        return;
    }

    typedef FGenericPlatformMath FPM;

    const FJSONSVGAttr& attr( SVGDoc.Attrs );
    const float texDim = FPM::RoundUpToPowerOfTwo( DocSize.GetMax() );
    const bool bIdentity = (texDim == attr.Width) && (texDim == attr.Height);
    ShapeTransform transform;

    if (attr.Width <= 0 || attr.Height <= 0)
    {
        UE_LOG(LogAGGJSVG,Warning, TEXT("DrawJSONSVG() ABORTED, SVG DOCUMENT SIZE NOT SPECIFIED"));
        return;
    }

    const float docMax = (attr.Width > attr.Height) ? attr.Width : attr.Height;
    const float scale = bIdentity ? 1.f : (texDim/docMax);

    transform.bIdentity = bIdentity;
    transform.Extent = docMax/2.f;
    transform.Scale = scale;

    UE_LOG(UntAGGJSVG,Warning, TEXT("DrawJSONSVG() Document Properties [bIdentity: %d, extents: %f, scale: %f]"), bIdentity?1:0, transform.Extent, transform.Scale);

    Context->ConstructBuffer(texDim, texDim, 0, true);
    Renderer->AttachBuffer(Context);

    if (! Context->HasValidBuffer())
    {
        UE_LOG(LogAGGJSVG,Warning, TEXT("DrawJSONSVG() ABORTED, INVALID BUFFER"));
        return;
    }

    DrawElement(SVGDoc, *Context, *Renderer, transform);
}

TArray<FVector2D> UAGGJSONSVGUtils::ExtractPath(const FJSONSVGDoc& SVGDoc, UAGGPathController* Path)
{
    if (! SVGDoc.IsValid() || ! Path)
    {
        UE_LOG(LogAGGJSVG,Warning, TEXT("ExtractPath() ABORTED, INVALID PARAMETERS"));
        return TArray<FVector2D>();
    }

    TArray<FVector2D> Vertices;

    ExtractPath(SVGDoc, *Path, Vertices);

    return MoveTemp( Vertices );
}

void UAGGJSONSVGUtils::DrawElement(const FJSONSVGElement& Element, UAGGContext& Context, UAGGRendererBase& Renderer, const ShapeTransform& T)
{
    check(Element.IsValid());

    UE_LOG(UntAGGJSVG,Warning, TEXT("DrawElement() Element [%s]"), *Element.Name.ToString());

    UAGGPathController& Path( *Context.GetPathController() );
    FJSONSVGShapeAttr shapeAttr;
    bool bShapeConstructed = false;

    if (Element.Name == "path")
    {
        FJSONSVGPath SVGPath;
        if (Element.AsType<FJSONSVGPath>(SVGPath))
        {
            ConstructPath(SVGPath, Path);
            shapeAttr = SVGPath.Attrs;
            bShapeConstructed = true;
        }
    }
    else if (Element.Name == "circle")
    {
        FJSONSVGCircle SVGCircle;
        if (Element.AsType<FJSONSVGCircle>(SVGCircle))
        {
            ConstructCircle(SVGCircle, Path);
            shapeAttr = SVGCircle.Attrs;
            bShapeConstructed = true;
        }
    }
    else if (Element.Name == "ellipse")
    {
        FJSONSVGEllipse SVGEllipse;
        if (Element.AsType<FJSONSVGEllipse>(SVGEllipse))
        {
            ConstructEllipse(SVGEllipse, Path);
            shapeAttr = SVGEllipse.Attrs;
            bShapeConstructed = true;
        }
    }
    else if (Element.Name == "rect")
    {
        FJSONSVGRect SVGRect;
        if (Element.AsType<FJSONSVGRect>(SVGRect))
        {
            ConstructRect(SVGRect, Path);
            shapeAttr = SVGRect.Attrs;
            bShapeConstructed = true;
        }
    }

    if (bShapeConstructed)
    {
        DrawShape(shapeAttr, Path, Renderer, T);
    }

    for (const FJSONSVGElement& child : Element.Children)
    {
        DrawElement(child, Context, Renderer, T);
    }
}

void UAGGJSONSVGUtils::ExtractPath(const FJSONSVGElement& Element, UAGGPathController& Path, TArray<FVector2D>& OutVertices)
{
    check(Element.IsValid());

    UE_LOG(UntAGGJSVG,Warning, TEXT("ExtractPath() Element [%s]"), *Element.Name.ToString());

    bool bShapeConstructed = false;

    if (Element.Name == "path")
    {
        FJSONSVGPath SVGPath;
        if (Element.AsType<FJSONSVGPath>(SVGPath))
        {
            ConstructPath(SVGPath, Path);
            bShapeConstructed = true;
        }
    }
    else if (Element.Name == "circle")
    {
        FJSONSVGCircle SVGCircle;
        if (Element.AsType<FJSONSVGCircle>(SVGCircle))
        {
            ConstructCircle(SVGCircle, Path);
            bShapeConstructed = true;
        }
    }
    else if (Element.Name == "ellipse")
    {
        FJSONSVGEllipse SVGEllipse;
        if (Element.AsType<FJSONSVGEllipse>(SVGEllipse))
        {
            ConstructEllipse(SVGEllipse, Path);
            bShapeConstructed = true;
        }
    }
    else if (Element.Name == "rect")
    {
        FJSONSVGRect SVGRect;
        if (Element.AsType<FJSONSVGRect>(SVGRect))
        {
            ConstructRect(SVGRect, Path);
            bShapeConstructed = true;
        }
    }

    if (bShapeConstructed)
    {
        Path.ToArray(OutVertices);
    }

    for (const FJSONSVGElement& child : Element.Children)
    {
        ExtractPath(child, Path, OutVertices);
    }
}

void UAGGJSONSVGUtils::DrawShape(const FJSONSVGShapeAttr& ShapeAttr, UAGGPathController& Path, UAGGRendererBase& Renderer, const ShapeTransform& T)
{
    FColor fillColor( FColor::FromHex(ShapeAttr.Fill) );

    UE_LOG(UntAGGJSVG,Warning, TEXT("=== path"));
    UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.Attrs.Fill: %s"), *fillColor.ToString());

    // Path does not contain any vertices, abort
    if (Path.Num() <= 0)
    {
        return;
    }

    if (! T.bIdentity)
    {
        Path.ResetTransform();

        Path.Translate(-T.Extent, -T.Extent);
        Path.Scale(T.Scale);
        Path.Translate(T.Extent*T.Scale, T.Extent*T.Scale);

        Path.ApplyTransform();
    }

    Renderer.SetColor(fillColor);
    Renderer.Render(&Path);

    float strokeWidth = ShapeAttr.StrokeWidth;

    if (strokeWidth > .001f)
    {
        FAGGStrokeSettings strokeSettings;
        FColor strokeColor( FColor::FromHex(ShapeAttr.Stroke) );

        strokeSettings.Width = strokeWidth;

        Path.PathAsStroke(strokeSettings);

        UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.Attrs.Stroke: %s"), *strokeColor.ToString());
        UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.Attrs.StrokeWidth: %f"), strokeWidth);

        Renderer.SetColor(fillColor);
        Renderer.Render(&Path);
    }
}

void UAGGJSONSVGUtils::ConstructPath(const FJSONSVGPath& SVGPath, UAGGPathController& Path, FAGGCurveSettings CurveSettings)
{
    const FJSONSVGPathAttr& attr( SVGPath.Attrs );
    bool bAsCurve = false;

    Path.Clear();

    for (const FJSONSVGPathData& d : attr.D)
    {
        UE_LOG(UntAGGJSVG,Warning, TEXT("\td.path: %d [x0:%6.3f y0:%6.3f] [x1:%6.3f y1:%6.3f] [x2:%6.3f y2:%6.3f] [x:%6.3f y:%6.3f]"),
            d.code, d.x0, d.y0, d.x1, d.y1, d.x2, d.y2, d.x, d.y);

        switch (d.code)
        {
            case 77:            // Move
                Path.MoveTo(d.x, d.y);
                UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.MoveTo(): [x:%6.3f y:%6.3f]"), d.x, d.y);
                break;

            case 90:            // Close path
                Path.ClosePolygon();
                UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.ClosePolygon()"));
                break;

            case 76:            // Line to
                Path.LineTo(d.x, d.y);
                UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.LineTo(): [x:%6.3f y:%6.3f]"), d.x, d.y);
                break;

            case 67:            // Bezier Curve To
                bAsCurve = true;
                Path.Curve4(d.x1, d.y1, d.x2, d.y2, d.x, d.y);
                UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.Curve4(): [x1:%6.3f y1:%6.3f] [x2:%6.3f y2:%6.3f] [x:%6.3f y:%6.3f]"),
                    d.x1, d.y1, d.x2, d.y2, d.x, d.y);
                break;

            case 83:            // Bezier Curve To (smooth)
                bAsCurve = true;
                Path.SmoothCurve4(d.x2, d.y2, d.x, d.y);
                UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.SmoothCurve4(): [x2:%6.3f y2:%6.3f] [x:%6.3f y:%6.3f]"), d.x2, d.y2, d.x, d.y);
                break;

            case 81:            // Quadratic Curve To
                bAsCurve = true;
                Path.Curve3(d.x1, d.y1, d.x, d.y);
                UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.Curve3(): [x1:%6.3f y1:%6.3f] [x:%6.3f y:%6.3f]"), d.x1, d.y1, d.x, d.y);
                break;

            case 84:            // Quadratic Curve To (smooth)
                bAsCurve = true;
                Path.SmoothCurve3(d.x, d.y);
                UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.SmoothCurve3(): [x:%6.3f y:%6.3f]"), d.x, d.y);
                break;

            case 65:            // Arc
                const float rx = d.x1;
                const float ry = d.y1;
                const float a = d.x2;
                const float x = d.x;
                const float y = d.y;
                const int32 arcFlags = static_cast<int32>(d.y2);
                const bool bLargeArc = (arcFlags&1) != 0;
                const bool bSweep = (arcFlags&2) != 0;
                bAsCurve = true;
                Path.ArcTo(rx, ry, a, bLargeArc, bSweep, x, y);
                UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.Curve3(): [rx:%6.3f ry:%6.3f a:%6.3f f:%d] [x:%6.3f y:%6.3f]"), d.x1, d.y1, d.x2, arcFlags, d.x, d.y);
                break;
        }
    }

    if (bAsCurve)
    {
        Path.PathAsCurve(CurveSettings);
    }
}

void UAGGJSONSVGUtils::ConstructCircle(const FJSONSVGCircle& SVGCircle, UAGGPathController& Path)
{
    const FJSONSVGCircleAttr& attr( SVGCircle.Attrs );
    Path.Clear();
    Path.AddCircle(attr.cx, attr.cy, attr.r);
    UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.AddCircle(): [cx:%6.3f cy:%6.3f r:%6.3f]"), attr.cx, attr.cy, attr.r);
}

void UAGGJSONSVGUtils::ConstructEllipse(const FJSONSVGEllipse& SVGEllipse, UAGGPathController& Path)
{
    const FJSONSVGEllipseAttr& attr( SVGEllipse.Attrs );
    Path.Clear();
    Path.AddEllipse(attr.cx, attr.cy, attr.rx, attr.ry);
    UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.AddEllipse(): [cx:%6.3f cy:%6.3f rx:%6.3f ry:%6.3f]"), attr.cx, attr.cy, attr.rx, attr.ry);
}

void UAGGJSONSVGUtils::ConstructRect(const FJSONSVGRect& SVGRect, UAGGPathController& Path)
{
    const FJSONSVGRectAttr& attr( SVGRect.Attrs );
    Path.Clear();
    Path.AddRoundRect(attr.x, attr.y, attr.rx, attr.ry, attr.Width, attr.Height);
    UE_LOG(UntAGGJSVG,Warning, TEXT("\tpath.AddRoundRect(): [x:%6.3f y:%6.3f rx:%6.3f ry:%6.3f w:%6.3f h:%6.3f]"), attr.x, attr.y, attr.rx, attr.ry, attr.Width, attr.Height);
}

