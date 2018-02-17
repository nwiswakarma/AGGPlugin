// 

#pragma once

#include "agg_pixfmt_gray.h"
#include "agg_pixfmt_rgb.h"
#include "agg_pixfmt_rgba.h"

#include "AGGRenderBuffer.h"

typedef agg::pixfmt_gray8  TPFMTG8;
typedef agg::pixfmt_bgr24  TPFMTBGR24;
typedef agg::pixfmt_bgra32 TPFMTBGRA32;

typedef class TAGGRenderBuffer< TPFMTG8     > TAGGBufferG8;
typedef class TAGGRenderBuffer< TPFMTBGR24  > TAGGBufferBGR24;
typedef class TAGGRenderBuffer< TPFMTBGRA32 > TAGGBufferBGRA32;

