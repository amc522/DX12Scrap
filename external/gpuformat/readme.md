# gpuformat

- [About](#about)
  - [Features](#features)
  - [Supported Platforms](#supported-platforms)
- [Examples](#examples)
  - [Format Traits](#format-traits)
  - [Translating Formats](#translating-formats)
  - [Iterating over Formats](#iterating-over-formats)
  - [Loading Texture Blocks](#loading-texture-blocks)
  - [Storing Texture Blocks](#storing-texture-blocks)
  - [Sampling](#sampling)
  - [Decompressing](#decompressing)
- [Building](#building)
- [Format Name Standard](#format-name-standard)
- [TODO](#todo)
- [Thirdparty libraries](#thidparty-libraries)

## About

There are several different graphics api's out there helping to abstract the gpu, but all of them represent the gpu in slightly different ways. Formats to represent textures and buffers can be one of the hardest to keep straight. **gpuformat** aims to provide a clear, unified abstraction of gpu formats while also giving accurate and reliable translations to each graphics api.

### Features
- **Abstraction of most dxgi (DirectX), OpenGL, and Vulkan formats** - Most formats used for vertex buffers, textures, and general purpose buffers are accounted for. Video formats are not currently supported.
- **`constexpr` traits for each format** - Each format has a set of traits detailing the properties that define it. Those include the number of channels, channels used, a best fit sample type, block extents, block byte size, in which api they are supported, whether they are normalized, an integer, a float, signed, or srgb. Other properties try to help define the position and bit length of each component in the format.
- **Loading format data streams into common sample data types** - Almost every format (all except pvrtc) has a function to load a block of that formats data into a common sample type. Those sample types are four components (RGBA), 32 or 64 bits per component, with the data type being uint32_t, int32_t, float, uint64_t, int64_t, or double. Reading is available for all uncompressed and compressed formats.
- **Storing data as the specified format** - Most formats have a function to take a common sample type and store the data as the desired format. *ASTC, BC, ETC, EAC, and PVRTC compressed formats are not supported for writing.*
- **Support for compressed formats** - These formats include ASTC, BC, ETC, EAC and PVRTC compression schemes.

### Supported Platforms
- DirectX 11 (dxgi)
- DirectX 12 (dxgi)
- OpenGL
- OpenGL ES
- Vulkan

### Examples
#### Format Traits
Format traits are type safe, static constexpr traits of a specific format.
```
#include <gpufmt/traits.h>

// all values are for the example format R8G8B8A8_UNORM

// size of a single block of data in the texture
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::BlockByteSize; //4 bytes

// how many texels in each dimension for a single data block of the texture
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::BlockExtent; // 1, 1, 1

// number of texels in a single data block (BlockExtent.x * BlockExtent.y * BlockExtent.z)
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::BlockTexelCount; // 1

// number of components the format represents
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::ComponentCount; // 4

// data type for a single component when sampled
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::ValueType; // float

// data type for a single sample representing only the number of components available in the format
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::NarrowSampleType; // glm::vec4

// size of a single narrow sample
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::NarrowSampleByteSize; // 16 bytes

// size of all the narrow samples in a block
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::NarrowSampleBlockByteSize; // 16 bytes

// data type for a single sample representing the RGBA channels, even if the format does not have some of those channels
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::WideSampleType; // glm::vec4

// size of a single wide sample
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::WideSampleByteSize; // 16 bytes

// size of all the wide samples in a block
gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::WideSampleBlockByteSize; // 16 bytes
```

#### Format Info
Format info duplicates some of the type safe traits, but also adds a bunch of properties that are available at runtime.

There are two ways to access a format's gpufmt::FormatInfo object
1. Access the FormatInfo object directly from the FormatTraits template overload. This can be used for compile time constants.
```
    const gpufmt::FormatInfo &formatInfo = gpufmt::FormatTraits<gpufmt::Format::R8G8B8A8_UNORM>::info;
```
2. From the formatInfo function. This can be used for dynamic runtime information.
```
    gpufmt::Format someFormat;
    ...
    const gpufmt::FormatInfo &formatInfo = gpufmt::formatInfo(someFormat);
```

Here's all the values in FormatInfo:
```
formatInfo.blockByteSize;
formatInfo.blockExtent;
formatInfo.componentCount;
formatInfo.narrowSampleByteSize;
formatInfo.narrowSampleBlockByteSize;
formatInfo.wideSampleByteSize;
formatInfo.wideSampleBlockByteSize;
formatInfo.narrowIsWide;
formatInfo.isSigned;
formatInfo.normalized;
formatInfo.scaled
formatInfo.integer;
formatInfo.floatingPoint;
formatInfo.srgb;
formatInfo.packed;
formatInfo.depth;
formatInfo.stencil;
formatInfo.compression;
formatInfo.readable;
formatInfo.writable;
formatInfo.decompressible;
formatInfo.suppoertedApis;
formatInfo.channels;
formatInfo.redBitMask;
formatInfo.greenBitMask;
formatInfo.blueBitMask;
formatInfo.alphaBitMask;
formatInfo.redIndex;
formatInfo.greenIndex;
formatInfo.blueIndex;
formatInfo.alphaIndex;
```

#### Translating Formats
DirectX11 & DirectX12
```
#include <gpufmt/dxgi.h>

gpufmt::dxgi::FormatConversion dxgiConversion = gpufmt::dxgi::translateFormat(someGpuFormat_Format);

std::optional<gpufmt::Format> format = gpufmt::dxgi::translateFormat(someDXGIFormat);
```

OpenGL
```
#include <gpufmt/opengl.h>

gpufmt::gl::FormatConversion glConversion = gpufmt::gl::translateFormat(someGpuFormat_Format);

std::optional<gpufmt::Format> format = gpufmt::gl::translateFormat(openglInternalFormat, openglFormat, openglType);
```

Vulkan
```
#include <gpufmt/vulkan.h>

gpufmt::vulkan::FormatConversion vulkanConversion = gpufmt::vulkan::translateFormat(someGpuFormat_Format);

std::optional<gpufmt::Format> format = gpufmt::vulkan::translateFormat(someVkFormat);
```

#### Iterating over Formats
```
#include <gpufmt/utility.h>

// enumerate all formats
for(gpufmt::Format format : gpufmt::FormatEnumerator())
{
  ...
}

// emumerate a range of formats
for(gpufmt::Format format : gpufmt::FormatEnumerator<gpufmt::R16_UNORM, gpufmt::R16_SFLOAT>())
{
  ...
}
```

#### Loading Texture Blocks
**gpuformat** provides the base functions for loading (reading) texture blocks. Load functions will take a single texture block and output an array of texels equal to the number texels in a single block. For most uncompressed textures, this will be 1 texel. Each format has a `FormatStorage` class to load texels. Not all formats can be read one block at a time. Some can only be fully decompressed. Every `FormatStorage` class has static boolean members `Writeable`, `Readable`, `Decompressible` and can be used to check if the corresponding functions are available.
```
#include <gpufmt/storage.h>

// optional if constexpr
if constexpr (gpufmt::FormatStorage<R8_UNORM>::Readable)
{
  uint8 r8Block = 127;
  glm::float4 texel;
  gpufmt::FormatStorage<gpufmt::R8_UNORM>::loadBlock(r8Block, texel);

  //texel == 0.4980392158031463623046875
}

```

#### Storing Texture Blocks
**gpuformat** provides the base functions for storing (writing) texture blocks. Store functions will take an array of texels (equal to the number in a block) and output exactly one texture block. For most uncompressed textures, this will be 1 texel. Each format has a `FormatStorage` class to store texels. Not all formats can be written. This includes any compressed format. Every `FormatStorage` class has static boolean members `Writeable`, `Readable`, `Decompressible` and can be used to check if the corresponding functions are available.
```
#include <gpufmt/storage.h>

// optional if constexpr
if constexpr (gpufmt::FormatStorage<R8_UNORM>::Writeable)
{
  uint8 r8Block;
  glm::float4 texel = 0.4980392158031463623046875;
  gpufmt::FormatStorage<gpufmt::R8_UNORM>::loadBlock(gpufmt::span<glm::float4, 1>(&texel, 1), r8Block);

  //r8Block == 127
}

```

#### Sampling
The sampling class provided is a low level block sampler the reads an entire block. This class can be used as the building blocks for a more complicated sampler. The block can output as an array of bytes representing the decompressed texels of a block or as an array of `gpufmt::SampleVariant`. `SampleVariant` is a variant type wrapping the following types:
- `glm::u32vec4`
- `glm::i32vec4`
- `glm::u64vec4`
- `glm::i64vec4`
- `glm::vec4`
- `glm::dvec4`

The format sample is expanded to the closest matching type, like how a texture would be sampled in a shader.
```
#include <gpufmt/sampler.h>

gpufmt::Format someFormat;
...
gpufmt::BlockSampler sampler{someFormat};

gpufmt::Extent bloxelToSample{244, 12, 0};
gpufmt::BlockSampleError error;
std::vector<gpufmt::SampleVariant> samples = sampler.variantSample(surface, bloxelToSample, error);
```

#### Decompressing
The decompressor class will decompressed an entire texture surface to its decompressed format. If the format cannot be decompressed or is not a compressed format, the decompress functions will return an error.
```
#include <gpufmt/decompress.h>

gpufmt::Format someFormat;
...
gpufmt::Decompressor decompressor{someFormat};
gpufmt::DecompressError error;
std::vector<gpufmt::byte> decompressor.decompress(compressedSurface, error);
```

### Supported Compilers
- Microsoft Visual C++ 2017
- Microsoft Visual C++ 2019

No other compilers have been tested, but there's no reason why they shouldn't work. I am more than happy integrate changes that provide compatibility for other compilers.

### Building
- If you **do not need** to read compressed formats (ASTC, BC, ETC, EAC, or PVRTC)
  **gpuformat** can be configured as a header only library. To do so, you need to do the following steps:
  1. Include all the headers except for `astc.h`, `bc.h`, `etc.h`, `pvrtc.h`.
  2. In `config.h`, uncomment `#define GF_EXCLUDE_COMPRESSED_FORMATS`
- If you **want** to read ASTC, BC, ETC, EAC, or PVRTC formats.
  1. Run [premake 5.0](https://premake.github.io/) in the gpuformat directory using the `premake5.lua` file targeting your desired toolset. ex. `premake5.exe vs2019`.
  2. Then build the static libraries with your desired environment.
  3. Include all of the files in `gpufmt/include` in your project.

### TODO
- Metal and Metal2 support
- Video format read support
- Support for legacy formats like luminance
- Utility functions for Vulkan
- Complete test coverage

### Thirdparty Libraries
- [astc_dec](https://github.com/richgel999/astc_dec) - ASTC format decompression.
- [DirectXTex](https://github.com/microsoft/DirectXTex) - BC format decompression and converting to and from half types.
- [glm](https://github.com/g-truc/glm) - Basic vector types and some packing and unpacking functions.
- [tcbrindle-span](https://github.com/tcbrindle/span) - std::span implementation for C++11 and later.
- [SwiftShader](https://github.com/google/swiftshader) - ETC and EAC format decompression.
- [PowerVR SDK](https://www.imgtec.com/developers/powervr-sdk-tools/powervr-sdk/) - PVRTC format decompression.
