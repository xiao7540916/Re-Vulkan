#pragma once

#include "Horizon/Core/HorizonCommon.h"

namespace Horizon
{
    /** Resource formats */
    enum class Format : uint32
    {
        Unknown,
        // Norm
        R8Unorm,
        R8Snorm,
        R16Unorm,
        R16Snorm,
        RG8Unorm,
        RG8Snorm,
        RG16Unorm,
        RG16Snorm,
        RGB16Unorm,
        RGB16Snorm,
        RGBA8Unorm,
        RGBA8Snorm,
        RGBA16Unorm,
        // UnormSrgb
        RGBA8UnormSrgb,
        // Float
        R16Float,
        RG16Float,
        RGB16Float,
        RGBA16Float,
        R32Float,
        RG32Float,
        RGB32Float,
        RGBA32Float,
        // Int
        R8Int,
        R8Uint,
        R16Int,
        R16Uint,
        R32Int,
        R32Uint,
        RG8Int,
        RG8Uint,
        RG16Int,
        RG16Uint,
        RG32Int,
        RG32Uint,
        RGB16Int,
        RGB16Uint,
        RGB32Int,
        RGB32Uint,
        RGBA8Int,
        RGBA8Uint,
        RGBA16Int,
        RGBA16Uint,
        RGBA32Int,
        RGBA32Uint,
        // BGRA
        BGRA8Unorm,
        BGRA8UnormSrgb,
        // Depth stencil
        D32Float,
        D16Unorm,
        D32FloatS8X24,
        D24UnormS8,
        // Count
        Count
    };

    /** Format type */
    enum class FormatType
    {
        Unknown,        ///< Unknown
        Float,          ///< Floating-point
        Sint,           ///< Signed integer
        Uint,           ///< Unsigned integer
        Snorm,          ///< Signed normalized
        Unorm,          ///< Unsigned normalized
        UnormSrgb,      ///< Unsigned normalized sRGB
    };

    /** Format Description */
    struct FormatDescription
    {
        Format format;
        String name;
        FormatType type;
        uint32 bytes;
        uint32 channels;
        struct
        {
            bool isDepth;
            bool isStencil;
        };
        int numChannelBits[4];
    };

    extern const FormatDescription gFormatTable[];

    inline bool IsDepthOnlyFormat(Format format)
    {
        return gFormatTable[(uint32)format].isDepth && !gFormatTable[(uint32)format].isStencil;
    }

    inline bool IsDepthStencilFormat(Format format)
    {
        return gFormatTable[(uint32)format].isDepth || gFormatTable[(uint32)format].isStencil;
    }

    inline FormatType GetFormatType(Format format)
    {
        return gFormatTable[(uint32)format].type;
    }

    inline uint32 GetFormatBytes(Format format)
    {
        return gFormatTable[(uint32_t)format].bytes;
    }
}
