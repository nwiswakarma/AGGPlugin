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

#include "AGGUtilityLibrary.h"
#include "AGGContext.h"
#include "AGGLogs.h"

void UAGGUtilityLibrary::FindValidNeighbours(int32 i, int32 x, int32 y, int32 NX, int32 NY, const int32* Offsets, int32* Neighbours)
{
    if (y>0) // S
    {
        if (x>0)    // SW
        {
            Neighbours[DSW] = i+Offsets[DSW];
        }
        if (x<NX-1) // SE
        {
            Neighbours[DSE] = i+Offsets[DSE];
        }

        Neighbours[DS] = i+Offsets[DS];
    }

    if (y<NY-1) // N
    {
        if (x>0)    // NW
        {
            Neighbours[DNW] = i+Offsets[DNW];
        }
        if (x<NX-1) // NE
        {
            Neighbours[DNE] = i+Offsets[DNE];
        }

        Neighbours[DN] = i+Offsets[DN];
    }

    if (x>0)    // W
    {
        Neighbours[DW] = i+Offsets[DW];
    }

    if (x<NX-1) // E
    {
        Neighbours[DE] = i+Offsets[DE];
    }
}

void UAGGUtilityLibrary::GenerateDepthMap(TArray<float>& DepthMap, UAGGContext* Context, UCurveFloat* ValueCurve, float Scale, uint8 SolidThreshold)
{
    if (! IsValid(Context))
    {
        UE_LOG(LogAGG,Warning, TEXT("UAGGUtilityLibrary::GenerateDepthMap() ABORTED, INVALID CONTEXT OBJECT"));
        return;
    }

    if (! Context->HasValidBuffer())
    {
        UE_LOG(LogAGG,Warning, TEXT("UAGGUtilityLibrary::GenerateDepthMap() ABORTED, INVALID RENDER BUFFER"));
        return;
    }

    IAGGRenderBuffer* Buffer = Context->GetBuffer();
    const int32 DimX = Buffer->GetWidth();
    const int32 DimY = Buffer->GetHeight();
    const int32 MapSize = DimX * DimY;

    if (DimX < 3 || DimY < 3)
    {
        UE_LOG(LogAGG,Warning, TEXT("UAGGUtilityLibrary::GenerateDepthMap() ABORTED, INVALID RENDER BUFFER DIMENSION"));
        return;
    }

    TArray<uint8>& BufferData(Buffer->GetByteBuffer());

    // Iteration queue
    TQueue<int32> tvQ;
    // Solid index set
    TSet<int32> tvS;
    // Sorting height values (not actual height map values)
    TArray<int32> tvV;

    TArray<FIntVector> BorderCoords;

    tvS.Reserve(MapSize);
    tvV.SetNumZeroed(MapSize);
    BorderCoords.Reserve(MapSize);

    int32 Offsets[NCOUNT];
    Offsets[DE ] =  1;
    Offsets[DNE] =  DimX+1;
    Offsets[DN ] =  DimX;
    Offsets[DNW] =  DimX-1;
    Offsets[DW ] = -1;
    Offsets[DSW] = -DimX-1;
    Offsets[DS ] = -DimX;
    Offsets[DSE] = -DimX+1;

    // Iterate over solid values to find solid borders

    for (int32 y=0, i=0; y<DimY; ++y)
    for (int32 x=0     ; x<DimX; ++x, ++i)
    {
        // Skip non-solid values
        if (BufferData[i] < SolidThreshold)
        {
            continue;
        }

        // Find solid neighbours

        int32 n[NCOUNT] = { -1, -1, -1, -1, -1, -1, -1, -1 };

        FindValidNeighbours(i, x, y, DimX, DimY, Offsets, n);
        bool bIsBorder = false;

        for (int32 no=0; no<NCOUNT; ++no)
        {
            int32 ni = n[no];

            if (ni < 0 || BufferData[ni] < SolidThreshold)
            {
                bIsBorder = true;
                break;
            }
        }

        if (bIsBorder)
        {
            tvS.Emplace(i);
            BorderCoords.Emplace(x, y, i);
        }
    }

    // Find first inner solid values

    for (const FIntVector& BorderId : BorderCoords)
    {
        const int32 x = BorderId.X;
        const int32 y = BorderId.Y;
        const int32 i = BorderId.Z;

        int32 n[NCOUNT] = { -1, -1, -1, -1, -1, -1, -1, -1 };

        FindValidNeighbours(i, x, y, DimX, DimY, Offsets, n);

        for (int32 no=0; no<NCOUNT; ++no)
        {
            int32 ni = n[no];

            if (ni < 0 || BufferData[ni] < SolidThreshold || tvS.Contains(ni))
            {
                continue;
            }

            tvV[ni] = 1;
            tvS.Emplace(ni);
            tvQ.Enqueue(ni);
        }
    }

    // Find remaining solid values

    int32 MaxValue = 1;

    while (! tvQ.IsEmpty())
    {
        int32 i;
        tvQ.Dequeue(i);

        const int32 v0 = tvV[i];
        const int32 v1 = v0 + 1;

        MaxValue = FMath::Max(v1, MaxValue);

        const int32 o[NCOUNT] = {
            i + Offsets[DE ],
            i + Offsets[DNE],
            i + Offsets[DN ],
            i + Offsets[DNW],
            i + Offsets[DW ],
            i + Offsets[DSW],
            i + Offsets[DS ],
            i + Offsets[DSE]
            };

        for (int32 no=0; no<NCOUNT; ++no)
        {
            int32 ni = o[no];

            if (ni < 0 || BufferData[ni] < SolidThreshold || tvS.Contains(ni))
            {
                continue;
            }

            tvV[ni] = v1;
            tvS.Emplace(ni);
            tvQ.Enqueue(ni);
        }
    }

    TArray<int32> SolidIndices(tvS.Array());

    SolidIndices.Sort( [&tvV](const int32& i0, const int32& i1) {
        return tvV[i0] < tvV[i1];
    } );

    TMap<int32, float> ElevationMap;
    int32 SolidCount = SolidIndices.Num();
    float MaxElevation = -1.0f;
    float MaxElevationInv = -1.0f;

    ElevationMap.Reserve(SolidCount);

    // Find maximum elevation value
    {
        float y = (SolidCount-1) / (SolidCount-1.0f);
        float x = FMath::Sqrt(1.1f) - FMath::Sqrt(1.1f * (1.0f-y));
        MaxElevation = x;
        MaxElevationInv = 1 / MaxElevation;
    }

    // Calculate elevation value using height redistribution formula
    for (int32 i=0; i<SolidCount; i++)
    {
        // Let y(x) be the total area that we want at elevation <= x.
        // We want the higher elevations to occur less than lower
        // ones, and set the area to be y(x) = 1 - (1-x)^2.
        float y = i / (SolidCount-1.0f);

        // Since the data is sorted by elevation, this will linearly
        // increase the elevation as the loop goes on.
        float x = FMath::Sqrt(1.1f) - FMath::Sqrt(1.1f * (1.0f-y));

        ElevationMap.Emplace(SolidIndices[i], x*MaxElevationInv);
    }

    // Max elevation under minimum threshold, abort
    if (MaxElevation <= KINDA_SMALL_NUMBER)
    {
        return;
    }

    DepthMap.Reset(MapSize);
    DepthMap.SetNumZeroed(MapSize);

    const bool bUseCurve = IsValid(ValueCurve);
    const float AvgInv = 1.f/(NCOUNT+1);

    for (int32 i : SolidIndices)
    {
        float e = ElevationMap.FindChecked(i);

        int32 x = i % DimX;
        int32 y = i / DimX;
        int32 n[NCOUNT] = { -1, -1, -1, -1, -1, -1, -1, -1 };
        FindValidNeighbours(i, x, y, DimX, DimY, Offsets, n);

        for (int32 no=0; no<NCOUNT; ++no)
        {
            int32 ni = n[no];

            if (ni < 0)
            {
                continue;
            }

            if (ElevationMap.Contains(ni))
            {
                e += ElevationMap.FindChecked(ni);
            }
        }

        e *= AvgInv;

        if (bUseCurve)
        {
            e = ValueCurve->GetFloatValue(e);
        }

        DepthMap[i] = e;
    }
}

UTexture2D* UAGGUtilityLibrary::CreateDepthMapTexture(UAGGContext* Context, UCurveFloat* ValueCurve, float Scale, uint8 SolidThreshold)
{
    UTexture2D* Texture = nullptr;

    if (! IsValid(Context))
    {
        UE_LOG(LogAGG,Warning, TEXT("UAGGUtilityLibrary::CreateDepthMapTexture() ABORTED, INVALID CONTEXT OBJECT"));
        return Texture;
    }

    if (! Context->HasValidBuffer())
    {
        UE_LOG(LogAGG,Warning, TEXT("UAGGUtilityLibrary::CreateDepthMapTexture() ABORTED, INVALID RENDER BUFFER"));
        return Texture;
    }

    IAGGRenderBuffer* Buffer = Context->GetBuffer();
    const int32 DimX = Buffer->GetWidth();
    const int32 DimY = Buffer->GetHeight();
    const int32 MapSize = DimX * DimY;

    if (DimX < 3 || DimY < 3)
    {
        UE_LOG(LogAGG,Warning, TEXT("UAGGUtilityLibrary::CreateDepthMapTexture() ABORTED, INVALID RENDER BUFFER DIMENSION"));
        return Texture;
    }

    TArray<float> DepthMap;
    GenerateDepthMap(DepthMap, Context, ValueCurve, Scale, SolidThreshold);

    Texture = UTexture2D::CreateTransient(DimX, DimY, EPixelFormat::PF_R32_FLOAT);
    Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    Texture->SRGB = 0;

    FTexture2DMipMap& Mip(Texture->PlatformData->Mips[0]);
    void* OutData = Mip.BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(OutData, DepthMap.GetData(), MapSize*DepthMap.GetTypeSize());
    Mip.BulkData.Unlock();

    Texture->UpdateResource();

    return Texture;
}
