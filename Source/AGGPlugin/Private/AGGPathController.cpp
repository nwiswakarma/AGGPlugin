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

#include "AGGPathController.h"

void UAGGPathController::ConvertCurvesToPoints(TArray<FVector2D>& OutPoints, const TArray<FVector2D>& InPoints, FAGGCurveSettings CurveSettings, bool bCircular)
{
    // Not enough input points to form a curve, abort
    if (InPoints.Num() < 3)
    {
        return;
    }

    const int32 PointCount = InPoints.Num();
    OutPoints.Reset(PointCount * 4);

    typedef FAGGPathController FAGGPath;

    FAGGPath Path;

    if (bCircular)
    {
        const FVector2D& PN(InPoints.Last());
        const FVector2D& P0(InPoints[0]);
        const FVector2D& P1(InPoints[1]);
        const FVector2D PN0(FAGGPath::GetMidPoint(PN, P0));
        const FVector2D P01(FAGGPath::GetMidPoint(P0, P1));
        Path.MoveTo(PN0);
        Path.Curve3(P0, P01);
    }
    else
    {
        const FVector2D& P0(InPoints[0]);
        const FVector2D& P1(InPoints[1]);
        const FVector2D P01(FAGGPath::GetMidPoint(P0, P1));
        Path.MoveTo(P0);
        Path.LineTo(P01);
    }

    for (int32 i=1; i<PointCount; ++i)
    {
        const FVector2D& P0(InPoints[i]);
        const FVector2D& P1(InPoints[(i+1)%PointCount]);
        const FVector2D P01(FAGGPath::GetMidPoint(P0, P1));
        Path.Curve3(P0, P01);
    }

    Path.PathAsCurve(CurveSettings);
    Path.ApplyConversion();
    Path.ToArray(OutPoints);
}
