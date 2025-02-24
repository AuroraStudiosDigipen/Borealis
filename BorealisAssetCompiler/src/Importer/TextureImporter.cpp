/******************************************************************************
/*!
\file       TextureImporter.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 29, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "Importer/TextureImporter.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
//#include "ispc_texcomp.h"
#include <cmp_compressonatorlib/compressonator.h>
#include <gli.hpp>

#define FOURCC_DXT1 0x31545844  // 'DXT1'
#define FOURCC_DXT5 0x35545844  // 'DXT5' in ASCII
#define FOURCC_DX10 0x30315844  // 'DX10' in ASCII
#define FOURCC_ATI2 0x32495441
#define DXGI_FORMAT_BC7_UNORM 98  // The BC7 format value for DX10 header

namespace BorealisAssetCompiler
{
    void SaveAsDDSCompressonator(std::filesystem::path const& filePath, std::filesystem::path const& output, TextureConfig & config)
    {
        CMP_InitFramework();

        CMP_MipSet MipSetIn;
        memset(&MipSetIn, 0, sizeof(CMP_MipSet));
        CMP_ERROR cmp_status = CMP_LoadTexture(filePath.string().c_str(), &MipSetIn);
        if (cmp_status != CMP_OK) {
            std::printf("Error %d: Loading source file!\n", cmp_status);
            return;
        }

        if (config.generateMipMaps && MipSetIn.m_nMipLevels <= 1)
        {
            CMP_INT requestLevel = 10; // Request 10 miplevels for the source image

            //------------------------------------------------------------------------
            // Checks what the minimum image size will be for the requested mip levels
            // if the request is too large, a adjusted minimum size will be returned
            //------------------------------------------------------------------------
            CMP_INT nMinSize = CMP_CalcMinMipSize(MipSetIn.m_nHeight, MipSetIn.m_nWidth, 10);

            //--------------------------------------------------------------
            // now that the minimum size is known, generate the miplevels
            // users can set any requested minumum size to use. The correct
            // miplevels will be set acordingly.
            //--------------------------------------------------------------
            CMP_GenerateMIPLevels(&MipSetIn, nMinSize);
        }

        //==========================
        // Set Compression Options
        //==========================
        KernelOptions   kernel_options;
        memset(&kernel_options, 0, sizeof(KernelOptions));

        if(config.type == TextureType::_NORMAL_MAP)
            kernel_options.format = CMP_FORMAT_BC5;   // Set the format to process
        else
            kernel_options.format = CMP_FORMAT_BC3;   // Set the format to process
        kernel_options.fquality = 1;     // Set the quality of the result (range of 0 - 1)
        kernel_options.threads = 0;            // Auto setting

        if (kernel_options.format == CMP_FORMAT_BC3)
        {
            // Enable punch through alpha setting
            kernel_options.bc15.useAlphaThreshold = true;
            kernel_options.bc15.alphaThreshold = 128;

            // Enable setting channel weights
            kernel_options.bc15.useChannelWeights = true;
            kernel_options.bc15.channelWeights[0] = 0.3086f;
            kernel_options.bc15.channelWeights[1] = 0.6094f;
            kernel_options.bc15.channelWeights[2] = 0.0820f;
        }

        //--------------------------------------------------------------
        // Setup a results buffer for the processed file,
        // the content will be set after the source texture is processed
        // in the call to CMP_ProcessTexture()
        //--------------------------------------------------------------
        CMP_MipSet MipSetCmp;
        memset(&MipSetCmp, 0, sizeof(CMP_MipSet));

        //===============================================
        // Compress the texture using Framework Lib
        //===============================================
        cmp_status = CMP_ProcessTexture(&MipSetIn, &MipSetCmp, kernel_options, nullptr);
        if (cmp_status != CMP_OK) {
            
        }

        //----------------------------------------------------------------
        // Save the result into a DDS file
        //----------------------------------------------------------------
        std::filesystem::path cachePath = output;
        cachePath.replace_extension(".dds").string();
        cmp_status = CMP_SaveTexture(cachePath.string().c_str(), &MipSetCmp);
        if (cmp_status == CMP_OK)
        {
            std::filesystem::rename(cachePath, output);
        }
        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);
    }

    gli::texture2d load_texture(const std::string& file_path) {
        gli::texture texture = gli::load(file_path);
        if (texture.empty()) {
            throw std::runtime_error("Failed to load texture: " + file_path);
        }
        return gli::texture2d(texture);
    }

    void SplitTShape(const char* input_file, const char* output_dir, std::array<std::filesystem::path, 6> & facesArray) {
        int width, height, channels;
        unsigned char* data = stbi_load(input_file, &width, &height, &channels, 0);
        if (!data) {
            throw std::runtime_error("Failed to load texture");
        }

        if (width % 4 != 0 || height % 3 != 0) {
            stbi_image_free(data);
            throw std::runtime_error("Input texture dimensions must be divisible by 4 and 3");
        }

        int face_size = width / 4; // Size of each cubemap face

        // Face order: +X, -X, +Y, -Y, +Z, -Z
        int offsets[6][2] = {
            {2 * face_size, face_size},     // +X
            {0 * face_size, face_size},      // -X
            {1 * face_size, 0},             // +Y
            {1 * face_size, 2 * face_size}, // -Y
            {1 * face_size, face_size},     // +Z
            {3 * face_size, face_size}     // -Z
        };

        // Extract and save each face
        for (int i = 0; i < 6; ++i) {
            int offset_x = offsets[i][0];
            int offset_y = offsets[i][1];

            // Allocate memory for one face
            unsigned char* face_data = new unsigned char[face_size * face_size * channels];
            for (int y = 0; y < face_size; ++y) {
                std::memcpy(
                    face_data + y * face_size * channels,
                    data + ((offset_y + y) * width + offset_x) * channels,
                    face_size * channels
                );
            }

            //Save as PNG
            char output_file[256];
            std::string str = "face_" + std::to_string(i) + ".png";
            std::string str2 = "face_" + std::to_string(i) + ".dds";
            snprintf(output_file, sizeof(output_file), str.c_str());
            stbi_write_png(output_file, face_size, face_size, channels, face_data, face_size * channels);
            delete[] face_data;

            std::filesystem::path path1 = str;
            std::filesystem::path path2 = str2;

            TextureConfig config{};
            SaveAsDDSCompressonator(path1, path2, config);

            facesArray[i] = path2;

            std::filesystem::remove(path1);
        }

        stbi_image_free(data);
    }

    void TextureImporter::CreateCubeMap(std::filesystem::path const& sourcePath, std::filesystem::path& cachePath)
    {
        std::array<std::filesystem::path, 6> facesArray;
        SplitTShape(sourcePath.string().c_str(), "", facesArray);

        // Load the 6 textures
        gli::texture2d face_textures[6];
        for (int i = 0; i < 6; ++i) {
            face_textures[i] = load_texture(facesArray[i].string());
        }

        gli::extent2d dimensions = face_textures[0].extent();
        gli::format format = face_textures[0].format();
        for (int i = 1; i < 6; ++i) {
            if (face_textures[i].extent() != dimensions || face_textures[i].format() != format) {
                throw std::runtime_error("All textures must have the same dimensions and format");
            }
        }

        // Create a cubemap texture
        gli::texture_cube cubemap(format, dimensions);

        // Copy each face into the cubemap
        for (int i = 0; i < 6; ++i) {
            std::memcpy(
                cubemap[i].data(),
                face_textures[i].data(),
                face_textures[i].size()
            );
        }

        for (int i = 0; i < 6; ++i) {
            std::filesystem::remove(facesArray[i]);
        }
        std::filesystem::path cache = cachePath;
        cache.replace_extension(".dds");
        // Save the cubemap to a DDS file
        if (!gli::save(cubemap, cache.string())) {
            throw std::runtime_error("Failed to save cubemap to file");
        }

        std::filesystem::path finalCachePath = cachePath;
        finalCachePath.replace_extension("");

        std::filesystem::rename(cache, finalCachePath);
    }

    void TextureImporter::SaveDDSFile(const std::string& filePath, int width, int height, const std::vector<uint8_t>& compressedData, DDSHeader header)
    {
        // Write the header and compressed data to the DDS file
        std::ofstream outFile(filePath, std::ios::binary);
        if (outFile.is_open()) {
            outFile.write(reinterpret_cast<char*>(&header), sizeof(header));
            outFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
            outFile.close();
        }
    }

    void FlipBitmapVertically(unsigned char* bitmap, int width, int height, int channels) {
        int stride = width * channels;
        std::vector<unsigned char> rowBuffer(stride);

        for (int y = 0; y < height / 2; ++y) {
            unsigned char* topRow = bitmap + y * stride;
            unsigned char* bottomRow = bitmap + (height - y - 1) * stride;

            // Swap the rows
            std::memcpy(rowBuffer.data(), topRow, stride);
            std::memcpy(topRow, bottomRow, stride);
            std::memcpy(bottomRow, rowBuffer.data(), stride);
        }
    }

    inline uint8_t sRGBToLinear(uint8_t sRGBValue) 
    {
        float normalized = sRGBValue / 255.0f;
        float linear = (normalized <= 0.04045f) ? normalized / 12.92f : std::pow((normalized + 0.055f) / 1.055f, 2.4f);
        return static_cast<uint8_t>(std::clamp(linear * 255.0f, 0.0f, 255.0f));
    }

    void TextureImporter::SaveFile(std::filesystem::path const& sourcePath, AssetConfig& assetConfig, std::filesystem::path & cachePath)
    {
        std::cout << sourcePath.string() << '\n';

        TextureConfig config = GetConfig<TextureConfig>(assetConfig);

        if (config.shape == TextureShape::_CUBE)
        {
            CreateCubeMap(sourcePath, cachePath);
            return;
        }

        //int width, height, channels;
        //unsigned char* imageData = stbi_load(sourcePath.string().c_str(), &width, &height, &channels, 4);
        //if (!imageData) {
        //    std::cerr << "Failed to load image\n";
        //    return;
        //}

        //if (!config.sRGB || config.type == TextureType::_NORMAL_MAP) 
        //{
        //    for (int i = 0; i < width * height * 4; i += 4) {
        //        imageData[i] = sRGBToLinear(imageData[i]);     // Red
        //        imageData[i + 1] = sRGBToLinear(imageData[i + 1]); // Green
        //        imageData[i + 2] = sRGBToLinear(imageData[i + 2]); // Blue
        //    }
        //}

        //FlipBitmapVertically(imageData, width, height, 4);

        //rgba_surface srcSurface;
        //srcSurface.ptr = imageData;
        //srcSurface.width = width;
        //srcSurface.height = height;
        //srcSurface.stride = width * 4;

        //int compressedSize = (width / 4) * (height / 4) * 16;
        //std::vector<uint8_t> compressedData(compressedSize);

        //DDSHeader header = {};
        //CompressBlocksBC3(&srcSurface, compressedData.data());

        //header.dwMagic = 0x20534444;  // 'DDS '
        //header.dwSize = 124;
        //header.dwFlags = 0x1007;  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
        //header.dwHeight = height;
        //header.dwWidth = width;
        //header.dwPitchOrLinearSize = (width / 4) * (height / 4) * 16;  // Compressed size for BC3 blocks
        //header.dwMipMapCount = 1;

        //header.ddpf.dwSize = 32;
        //header.ddpf.dwFlags = 4;  // DDPF_FOURCC
        //header.ddpf.dwFourCC = FOURCC_DXT5;  // 'DXT5' FourCC for BC3 (DXT5)

        //header.ddsCaps.dwCaps1 = 0x1000;  // DDSCAPS_TEXTURE
        ////else
        ////{
        ////    CompressBlocksBC5(&srcSurface, compressedData.data());

        ////    header.dwMagic = 0x20534444;  // 'DDS '
        ////    header.dwSize = 124;
        ////    header.dwFlags = 0x1007;  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
        ////    header.dwHeight = height;
        ////    header.dwWidth = width;
        ////    header.dwPitchOrLinearSize = compressedSize;  // Compressed size for BC5 blocks
        ////    header.dwMipMapCount = 1;

        ////    header.ddpf.dwSize = 32;
        ////    header.ddpf.dwFlags = 4;  // DDPF_FOURCC
        ////    header.ddpf.dwFourCC = FOURCC_ATI2;  // 'ATI2' FourCC for BC5

        ////    header.ddsCaps.dwCaps1 = 0x1000;

        ////    header.dwPitchOrLinearSize = compressedSize;  // Compressed size for BC5 blocks
        ////    header.dwMipMapCount = 1;  // No mipmaps

        ////    header.ddpf.dwSize = 32;
        ////    header.ddpf.dwFlags = 4;  // DDPF_FOURCC
        ////    header.ddpf.dwFourCC = 0x32495441;  // 'ATI2' for BC5

        ////    header.ddsCaps.dwCaps1 = 0x1000;  // DDSCAPS_TEXTURE
        ////    header.ddsCaps.dwCaps2 = 0;  // Not used
        ////    header.dwReserved1[0] = 0;
        ////}

        //std::string cacheString = cachePath.replace_extension(".dds").string();
        std::string cacheString = cachePath.string();


        //SaveDDSFile(cacheString.c_str(), width, height, compressedData, header);
        SaveAsDDSCompressonator(sourcePath, cacheString, config);

        //stbi_image_free(imageData);
    }
}

