// 

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "JSONSVGTypes.h"
#include "AGGContext.h"
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
    static void DrawJSONSVG(const FJSONSVGDoc& SVGDoc, UAGGContext* Context, const FIntPoint& DocSize);

    UFUNCTION(BlueprintCallable, Category = "AGG")
    static TArray<FVector2D> ExtractPath(const FJSONSVGDoc& SVGDoc, UAGGPathController* Path);

private:

    static void DrawElement(const FJSONSVGElement& Element, UAGGContext& Context, const ShapeTransform& T);
    static void DrawShape(const FJSONSVGShapeAttr& ShapeAttr, UAGGContext& Context, const ShapeTransform& T);

    static void ExtractPath(const FJSONSVGElement& Element, UAGGPathController& Path, TArray<FVector2D>& OutVertices);

    static void ConstructPath(const FJSONSVGPath& SVGPath, UAGGPathController& Path, FAGGCurveSettings CurveSettings = FAGGCurveSettings());
    static void ConstructCircle(const FJSONSVGCircle& SVGCircle, UAGGPathController& Path);
    static void ConstructEllipse(const FJSONSVGEllipse& SVGEllipse, UAGGPathController& Path);
    static void ConstructRect(const FJSONSVGRect& SVGRect, UAGGPathController& Path);

};
