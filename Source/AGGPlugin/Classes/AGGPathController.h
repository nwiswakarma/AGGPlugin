// 

#pragma once

#include "agg_curves.h"
#include "agg_ellipse.h"
#include "agg_rounded_rect.h"
#include "agg_path_storage.h"
#include "agg_trans_affine.h"
#include "agg_conv_curve.h"
#include "agg_conv_dash.h"
#include "agg_conv_stroke.h"
#include "agg_conv_transform.h"

#include "CoreUObject.h"
#include "Queue.h"
#include "AGGPathController.generated.h"

// ---------------------------- Path Conversion Types

UENUM(BlueprintType)
enum class EAGGPathConv : uint8
{
	STROKE,
	DASH,
	CURVE,
	CONTOUR
};

// ---------------------------- Stroke Settings

UENUM(BlueprintType)
enum class EAGGLineCap : uint8
{
    BUTT_CAP,
    SQUARE_CAP,
    ROUND_CAP
};

UENUM(BlueprintType)
enum class EAGGLineJoin : uint8
{
    MITER_JOIN         = 0,
    MITER_JOIN_REVERT  = 1,
    ROUND_JOIN         = 2,
    BEVEL_JOIN         = 3,
    MITER_JOIN_ROUND   = 4
};

UENUM(BlueprintType)
enum class EAGGInnerJoin : uint8
{
    INNER_BEVEL,
    INNER_MITER,
    INNER_JAG,
    INNER_ROUND
};

USTRUCT(BlueprintType)
struct AGGPLUGIN_API FAGGStrokeSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float Width = .5f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float MiterLimit = 4.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float InnerMiterLimit = 1.01f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    EAGGLineCap LineCap = EAGGLineCap::BUTT_CAP;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    EAGGLineJoin LineJoin = EAGGLineJoin::MITER_JOIN;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    EAGGInnerJoin InnerJoin = EAGGInnerJoin::INNER_MITER;
};

// ---------------------------- Dash Settings

USTRUCT(BlueprintType)
struct AGGPLUGIN_API FAGGDashComp
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float DashLength = 25.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float GapLength = 15.f;
};

USTRUCT(BlueprintType)
struct AGGPLUGIN_API FAGGDashSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    FAGGStrokeSettings StrokeSettings;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    TArray<FAGGDashComp> DashCompositions;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float DashStart = 0.f;
};

// ---------------------------- Curve Settings

UENUM(BlueprintType)
enum class EAGGCurveApproxMethod : uint8
{
    CURVE_INC,
    CURVE_DIV
};

USTRUCT(BlueprintType)
struct AGGPLUGIN_API FAGGCurveSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    EAGGCurveApproxMethod ApproximationMethod = EAGGCurveApproxMethod::CURVE_DIV;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float ApproximationScale = 1.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    float AngleTolerance = 0.f;
};

// ---------------------------- Path Controller

UCLASS(BlueprintType)
class AGGPLUGIN_API UAGGPathController : public UObject
{
	GENERATED_BODY()

    struct FConversionEntry
    {
        EAGGPathConv ConversionType;
        int32 SettingIndex;

        FConversionEntry() = default;

        FConversionEntry(EAGGPathConv t, int32 i)
            : ConversionType(t)
            , SettingIndex(i)
        {
        }
    };

    TQueue<FConversionEntry> ConversionQueue;
    TArray<FAGGStrokeSettings> StrokeSettings;
    TArray<FAGGDashSettings> DashSettings;
    TArray<FAGGCurveSettings> CurveSettings;

    agg::path_storage Path;
    agg::trans_affine Transform;

public:

    UFUNCTION(BlueprintCallable, Category = "AGG")
    int32 Num() const
    {
        return Path.total_vertices();
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void ResetTransform()
    {
        Transform.reset();
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void Translate(float x, float y)
    {
        Transform *= agg::trans_affine_translation(x, y);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void Rotation(float angleRad)
    {
        Transform *= agg::trans_affine_rotation(angleRad);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void Scale(float scale)
    {
        Transform *= agg::trans_affine_scaling(scale);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void ApplyTransform()
    {
        Path.transform_all_paths(Transform);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    bool HasPathConversion() const
    {
        return ! ConversionQueue.IsEmpty();
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void MoveTo(float x, float y)
    {
        Path.move_to(x, y);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void LineTo(float x, float y)
    {
        Path.line_to(x, y);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void ArcTo(float rx, float ry, float angle,
               bool bLargeArc, bool bSweep,
               float x, float y)
    {
        Path.arc_to(rx, ry, angle, bLargeArc, bSweep, x, y);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void CurveTo(float x1, float y1, float x2, float y2)
    {
        agg::curve3 curve;
        double x0, y0;
        Path.last_vertex(&x0, &y0);
        curve.init(x0, y0,
                   x1, y1,
                   x2, y2);
        Path.concat_path(curve);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void Curve3(float x1, float y1, float x2, float y2)
    {
        Path.curve3(x1, y1, x2, y2);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void SmoothCurve3(float x, float y)
    {
        Path.curve3(x, y);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void Curve4(float cx0, float cy0, float cx1, float cy1, float x1, float y1)
    {
        Path.curve4(cx0, cy0, cx1, cy1, x1, y1);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void SmoothCurve4(float cx1, float cy1, float x1, float y1)
    {
        Path.curve4(cx1, cy1, x1, y1);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void AddCircle(float x, float y, float r, int32 step = 0)
    {
        AddEllipse(x, y, r, r, step);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void AddEllipse(float x, float y, float rx, float ry, int32 step = 0)
    {
        if (step < 0)
            step = 0;
        agg::ellipse ellipse(x, y, rx, ry, step);
        Path.concat_path(ellipse);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void AddRoundRect(float x, float y, float rx, float ry, float w, float h)
    {
        agg::rounded_rect rect;
        rect.rect(x, y, x+w, y+h);
        rect.radius(rx, ry);
        Path.concat_path(rect);
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void ClosePolygon()
    {
        Path.close_polygon();
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void PathAsStroke(FAGGStrokeSettings Settings)
    {
        EAGGPathConv e = EAGGPathConv::STROKE;
        int32 s = StrokeSettings.Add(Settings);
        ConversionQueue.Enqueue(FConversionEntry(e, s));
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void PathAsDash(FAGGDashSettings Settings)
    {
        if (Settings.DashCompositions.Num() <= 0)
        {
            return;
        }

        EAGGPathConv e = EAGGPathConv::DASH;
        int32 s = DashSettings.Add(Settings);
        ConversionQueue.Enqueue(FConversionEntry(e, s));
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void PathAsCurve(FAGGCurveSettings Settings)
    {
        EAGGPathConv e = EAGGPathConv::CURVE;
        int32 s = CurveSettings.Add(Settings);
        ConversionQueue.Enqueue(FConversionEntry(e, s));
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void ApplyConversion()
    {
        while (! ConversionQueue.IsEmpty())
        {
            FConversionEntry entry;
            ConversionQueue.Dequeue(entry);

            agg::path_storage p;
            EAGGPathConv e( entry.ConversionType );
            int32 s( entry.SettingIndex );

            switch (e)
            {
                case EAGGPathConv::STROKE:
                    if (StrokeSettings.IsValidIndex(s))
                    {
                        PathToStroke(Path, p, StrokeSettings[s]);
                        Path = p;
                    }
                    break;

                case EAGGPathConv::DASH:
                    if (DashSettings.IsValidIndex(s))
                    {
                        PathToDash(Path, p, DashSettings[s]);
                        Path = p;
                    }
                    break;

                case EAGGPathConv::CURVE:
                    if (CurveSettings.IsValidIndex(s))
                    {
                        PathToCurve(Path, p, CurveSettings[s]);
                        Path = p;
                    }
                    break;
            }
        }

        ResetConversion();
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    TArray<FVector2D> ToArray(bool bApplyConversion = true)
    {
        TArray<FVector2D> points;
        double x, y;

        if (bApplyConversion && HasPathConversion())
        {
            ApplyConversion();
        }

        Path.rewind(0);
        points.Reserve(Path.total_vertices());

        while (! agg::is_stop(Path.vertex(&x, &y)))
        {
            points.Emplace(x, y);
        }

        points.Shrink();

        return MoveTemp(points);
    }

    void ToArray(TArray<FVector2D>& OutArray, bool bApplyConversion = true)
    {
        TArray<FVector2D>& points( OutArray );
        double x, y;

        if (bApplyConversion && HasPathConversion())
        {
            ApplyConversion();
        }

        points.Reserve(points.Num() + Path.total_vertices());
        Path.rewind(0);

        while (! agg::is_stop(Path.vertex(&x, &y)))
        {
            points.Emplace(x, y);
        }

        points.Shrink();
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void ClearConversion()
    {
        ConversionQueue.Empty();
    }

    UFUNCTION(BlueprintCallable, Category = "AGG")
    void Clear()
    {
        Path.remove_all();
    }

    FORCEINLINE agg::path_storage& GetStorage(bool bApplyConversion = true)
    {
        if (bApplyConversion && HasPathConversion())
            ApplyConversion();
        return Path;
    }

    FORCEINLINE agg::path_storage& operator*()
    {
        return GetStorage();
    }

private:

    template<class TVertexSource>
    void PathToStroke(TVertexSource& in_vs, TVertexSource& out_vs, const FAGGStrokeSettings& settings)
    {
        agg::conv_stroke<TVertexSource> stroke(in_vs);
        // Apply Settings
        stroke.width(settings.Width);
        stroke.line_join(agg::line_join_e(settings.LineJoin));
        stroke.line_cap(agg::line_cap_e(settings.LineCap));
        stroke.inner_join(agg::inner_join_e(settings.InnerJoin));
        stroke.miter_limit(settings.MiterLimit);
        stroke.inner_miter_limit(settings.InnerMiterLimit);
        // Concat Path
        out_vs.concat_path(stroke);
    }

    template<class TVertexSource>
    void PathToDash(TVertexSource& in_vs, TVertexSource& out_vs, const FAGGDashSettings& ds)
    {
        const FAGGStrokeSettings& ss( ds.StrokeSettings );
        agg::conv_dash<TVertexSource> dash(in_vs);
        agg::conv_stroke< agg::conv_dash<TVertexSource> > stroke(dash);
        // Apply Dash Settings
        for (const FAGGDashComp& comp : ds.DashCompositions)
            dash.add_dash(comp.DashLength, comp.GapLength);
        dash.dash_start(ds.DashStart);
        // Apply Stroke Settings
        stroke.width(ss.Width);
        stroke.line_join(agg::line_join_e(ss.LineJoin));
        stroke.line_cap(agg::line_cap_e(ss.LineCap));
        stroke.inner_join(agg::inner_join_e(ss.InnerJoin));
        stroke.miter_limit(ss.MiterLimit);
        stroke.inner_miter_limit(ss.InnerMiterLimit);
        // Concat Path
        out_vs.concat_path(stroke);
    }

    template<class TVertexSource>
    void PathToCurve(TVertexSource& in_vs, TVertexSource& out_vs, const FAGGCurveSettings& settings)
    {
        agg::conv_curve<TVertexSource> curve(in_vs);
        // Apply Settings
        curve.approximation_method(
            agg::curve_approximation_method_e(settings.ApproximationMethod));
        curve.approximation_scale(settings.ApproximationScale);
        curve.angle_tolerance(agg::deg2rad(settings.AngleTolerance));
        // Concat Path
        out_vs.concat_path(curve);
    }

    void ResetConversion()
    {
        StrokeSettings.Reset();
        DashSettings.Reset();
        CurveSettings.Reset();
    }

};
