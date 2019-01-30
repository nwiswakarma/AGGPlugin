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

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AGGUtilityLibrary.generated.h"

class UAGGContext;

UCLASS()
class AGGPLUGIN_API UAGGUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

    enum { DE  = 0 };
    enum { DNE = 1 };
    enum { DN  = 2 };
    enum { DNW = 3 };
    enum { DW  = 4 };
    enum { DSW = 5 };
    enum { DS  = 6 };
    enum { DSE = 7 };
    enum { NCOUNT = 8 };

    static void FindValidNeighbours(int32 i, int32 x, int32 y, int32 NX, int32 NY, const int32* Offsets, int32* Neighbours);

public:

    UFUNCTION(BlueprintCallable)
    static void GenerateDepthMap(TArray<float>& DepthMap, UAGGContext* Context, UCurveFloat* ValueCurve = nullptr, float Scale = 1.f, uint8 SolidThreshold = 127);

    UFUNCTION(BlueprintCallable)
    static UTexture2D* CreateDepthMapTexture(UAGGContext* Context, UCurveFloat* ValueCurve = nullptr, float Scale = 1.f, uint8 SolidThreshold = 127);
};
