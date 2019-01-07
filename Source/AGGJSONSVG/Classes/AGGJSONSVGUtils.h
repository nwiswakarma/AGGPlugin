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

#include "Kismet/BlueprintFunctionLibrary.h"
#include "JSONSVGTypes.h"
#include "AGGContext.h"
#include "AGGRendererObject.h"
#include "AGGJSONSVGUtils.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAGGJSVG, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(UntAGGJSVG, Log, All);

UCLASS(BlueprintType, Blueprintable)
class AGGJSONSVG_API UAGGJSONSVGUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

    struct ShapeTransform
    {
        ShapeTransform() = default;
        float Extent;
        float Scale;
        bool bIdentity;
    };

public:

    UFUNCTION(BlueprintCallable, Category = "AGG")
    static void DrawJSONSVG(const FJSONSVGDoc& SVGDoc, UAGGContext* Context, UAGGRendererBase* Renderer, const FIntPoint& DocSize);

    UFUNCTION(BlueprintCallable, Category = "AGG")
    static TArray<FVector2D> ExtractPath(const FJSONSVGDoc& SVGDoc, UAGGPathController* Path);

private:

    static void DrawElement(const FJSONSVGElement& Element, UAGGContext& Context, UAGGRendererBase& Renderer, const ShapeTransform& T);
    static void DrawShape(const FJSONSVGShapeAttr& ShapeAttr, UAGGPathController& Path, UAGGRendererBase& Renderer, const ShapeTransform& T);

    static void ExtractPath(const FJSONSVGElement& Element, UAGGPathController& Path, TArray<FVector2D>& OutVertices);

    static void ConstructPath(const FJSONSVGPath& SVGPath, UAGGPathController& Path, FAGGCurveSettings CurveSettings = FAGGCurveSettings());
    static void ConstructCircle(const FJSONSVGCircle& SVGCircle, UAGGPathController& Path);
    static void ConstructEllipse(const FJSONSVGEllipse& SVGEllipse, UAGGPathController& Path);
    static void ConstructRect(const FJSONSVGRect& SVGRect, UAGGPathController& Path);

};
