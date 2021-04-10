#include "RenderFormats.h"

namespace Horizon
{
    const FormatDescription gFormatTable[] =
    {
        // format                     name                type                      bytes  channels  { depth, stencil }    numChannelBits
        { Format::Unknown,           "Unknown",           FormatType::Unknown,      0,     0,        { false, false },    { 0,  0,  0,  0  } },
                                                                                                              
        { Format::R8Unorm,           "R8Unorm",           FormatType::Unorm,        1,     1,        { false, false },    { 8,  0,  0,  0  } },
        { Format::R8Snorm,           "R8Snorm",           FormatType::Snorm,        1,     1,        { false, false },    { 8,  0,  0,  0  } },
        { Format::R16Unorm,          "R16Unorm",          FormatType::Unorm,        2,     1,        { false, false },    { 16, 0,  0,  0  } },
        { Format::R16Snorm,          "R16Snorm",          FormatType::Snorm,        2,     1,        { false, false },    { 16, 0,  0,  0  } },
        { Format::RG8Unorm,          "RG8Unorm",          FormatType::Unorm,        2,     2,        { false, false },    { 8,  8,  0,  0  } },
        { Format::RG8Snorm,          "RG8Snorm",          FormatType::Snorm,        2,     2,        { false, false },    { 8,  8,  0,  0  } },
        { Format::RG16Unorm,         "RG16Unorm",         FormatType::Unorm,        4,     2,        { false, false },    { 16, 16, 0,  0  } },
        { Format::RG16Snorm,         "RG16Snorm",         FormatType::Snorm,        4,     2,        { false, false },    { 16, 16, 0,  0  } },
        { Format::RGB16Unorm,        "RGB16Unorm",        FormatType::Unorm,        6,     3,        { false, false },    { 16, 16, 16, 0  } },
        { Format::RGB16Snorm,        "RGB16Snorm",        FormatType::Snorm,        6,     3,        { false, false },    { 16, 16, 16, 0  } },
        { Format::RGBA8Unorm,        "RGBA8Unorm",        FormatType::Unorm,        4,     4,        { false, false },    { 8,  8,  8,  8  } },
        { Format::RGBA8Snorm,        "RGBA8Snorm",        FormatType::Snorm,        4,     4,        { false, false },    { 8,  8,  8,  8  } },
        { Format::RGBA16Unorm,       "RGBA16Unorm",       FormatType::Unorm,        8,     4,        { false, false },    { 16, 16, 16, 16 } },
                                                                                                              
        { Format::RGBA8UnormSrgb,    "RGBA8UnormSrgb",    FormatType::UnormSrgb,    4,     4,        { false, false },    { 8,  8,  8,  8  } },
                                                                                                              
        { Format::R16Float,          "R16Float",          FormatType::Float,        2,     1,        { false, false },    { 16, 0,  0,  0  } },
        { Format::RG16Float,         "RG16Float",         FormatType::Float,        4,     2,        { false, false },    { 16, 16, 0,  0  } },
        { Format::RGB16Float,        "RGB16Float",        FormatType::Float,        6,     3,        { false, false },    { 16, 16, 16, 0  } },
        { Format::RGBA16Float,       "RGBA16Float",       FormatType::Float,        8,     4,        { false, false },    { 16, 16, 16, 16 } },
        { Format::R32Float,          "R32Float",          FormatType::Float,        4,     1,        { false, false },    { 32, 0,  0,  0  } },
        { Format::RG32Float,         "RG32Float",         FormatType::Float,        8,     2,        { false, false },    { 32, 32, 0,  0  } },
        { Format::RGB32Float,        "RGB32Float",        FormatType::Float,        12,    3,        { false, false },    { 32, 32, 32, 0  } },
        { Format::RGBA32Float,       "RGBA32Float",       FormatType::Float,        16,    4,        { false, false },    { 32, 32, 32, 32 } },
                                                                                                              
        { Format::R8Int,             "R8Int",             FormatType::Sint,         1,     1,        { false, false },    { 8,  0,  0,  0  } },
        { Format::R8Uint,            "R8Uint",            FormatType::Uint,         1,     1,        { false, false },    { 8,  0,  0,  0  } },
        { Format::R16Int,            "R16Int",            FormatType::Sint,         2,     1,        { false, false },    { 16, 0,  0,  0  } },
        { Format::R16Uint,           "R16Uint",           FormatType::Uint,         2,     1,        { false, false },    { 16, 0,  0,  0  } },
        { Format::R32Int,            "R32Int",            FormatType::Sint,         4,     1,        { false, false },    { 32, 0,  0,  0  } },
        { Format::R32Uint,           "R32Uint",           FormatType::Uint,         4,     1,        { false, false },    { 32, 0,  0,  0  } },
        { Format::RG8Int,            "RG8Int",            FormatType::Sint,         2,     2,        { false, false },    { 8,  8,  0,  0  } },
        { Format::RG8Uint,           "RG8Uint",           FormatType::Uint,         2,     2,        { false, false },    { 8,  8,  0,  0  } },
        { Format::RG16Int,           "RG16Int",           FormatType::Sint,         4,     2,        { false, false },    { 16, 16, 0,  0  } },
        { Format::RG16Uint,          "RG16Uint",          FormatType::Uint,         4,     2,        { false, false },    { 16, 16, 0,  0  } },
        { Format::RG32Int,           "RG32Int",           FormatType::Sint,         8,     2,        { false, false },    { 32, 32, 0,  0  } },
        { Format::RG32Uint,          "RG32Uint",          FormatType::Uint,         8,     2,        { false, false },    { 32, 32, 0,  0  } },
        { Format::RGB16Int,          "RGB16Int",          FormatType::Sint,         6,     3,        { false, false },    { 16, 16, 16, 0  } },
        { Format::RGB16Uint,         "RGB16Uint",         FormatType::Uint,         6,     3,        { false, false },    { 16, 16, 16, 0  } },
        { Format::RGB32Int,          "RGB32Int",          FormatType::Sint,         12,    3,        { false, false },    { 32, 32, 32, 0  } },
        { Format::RGB32Uint,         "RGB32Uint",         FormatType::Uint,         12,    3,        { false, false },    { 32, 32, 32, 0  } },
        { Format::RGBA8Int,          "RGBA8Int",          FormatType::Sint,         4,     4,        { false, false },    { 8,  8,  8,  8  } },
        { Format::RGBA8Uint,         "RGBA8Uint",         FormatType::Uint,         4,     4,        { false, false },    { 8,  8,  8,  8  } },
        { Format::RGBA16Int,         "RGBA16Int",         FormatType::Sint,         8,     4,        { false, false },    { 16, 16, 16, 16 } },
        { Format::RGBA16Uint,        "RGBA16Uint",        FormatType::Uint,         8,     4,        { false, false },    { 16, 16, 16, 16 } },
        { Format::RGBA32Int,         "RGBA32Int",         FormatType::Sint,         16,    4,        { false, false },    { 32, 32, 32, 32 } },
        { Format::RGBA32Uint,        "RGBA32Uint",        FormatType::Uint,         16,    4,        { false, false },    { 32, 32, 32, 32 } },
                                                                                                              
        { Format::BGRA8Unorm,        "BGRA8Unorm",        FormatType::Unorm,        4,     4,        { false, false },    { 8,  8,  8,  8  } },
        { Format::BGRA8UnormSrgb,    "BGRA8UnormSrgb",    FormatType::UnormSrgb,    4,     4,        { false, false },    { 8,  8,  8,  8  } },
                                                                                                              
        { Format::D32Float,          "D32Float",          FormatType::Float,        4,     1,        { true,  false },    { 32, 0,  0,  0  } },
        { Format::D16Unorm,          "D16Unorm",          FormatType::Unorm,        2,     1,        { true,  false },    { 16, 0,  0,  0  } },
        { Format::D32FloatS8X24,     "D32FloatS8X24",     FormatType::Float,        8,     2,        { true,  true, },    { 32, 8,  24, 0  } },
        { Format::D24UnormS8,        "D24UnormS8",        FormatType::Unorm,        4,     2,        { true,  true, },    { 24, 8,  0,  0  } },
    };
    static_assert(HORIZON_ARRAYSIZE(gFormatTable) == (uint32)Format::Count);
}