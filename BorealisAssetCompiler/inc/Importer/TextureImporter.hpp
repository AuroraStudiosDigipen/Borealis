/******************************************************************************
/*!
\file       TextureImporter.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 29, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef TextureImporter_HPP
#define TextureImporter_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "ispc_texcomp.h"

#include "Importer/AssetConfigs.hpp"

namespace BorealisAssetCompiler
{
    struct DDSHeader {
        uint32_t dwMagic;
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwHeight;
        uint32_t dwWidth;
        uint32_t dwPitchOrLinearSize;
        uint32_t dwDepth;
        uint32_t dwMipMapCount;
        uint32_t dwReserved1[11];
        struct {
            uint32_t dwSize;
            uint32_t dwFlags;
            uint32_t dwFourCC;
            uint32_t dwRGBBitCount;
            uint32_t dwRBitMask;
            uint32_t dwGBitMask;
            uint32_t dwBBitMask;
            uint32_t dwABitMask;
        } ddpf;
        struct {
            uint32_t dwCaps1;
            uint32_t dwCaps2;
            uint32_t dwReserved[2];
        } ddsCaps;
        uint32_t dwReserved2;
    };

    struct DDSHeaderDX10 {
        uint32_t dxgiFormat;
        uint32_t resourceDimension;
        uint32_t miscFlag;
        uint32_t arraySize;
        uint32_t miscFlags2;
    };

    class TextureImporter
    {
    public:
        static void SaveFile(std::filesystem::path const& sourcePath, AssetConfig& assetConfig, std::filesystem::path& cachePath);

        template <typename T>
        static void SaveFile(const T * bitmap, int width, int height, std::filesystem::path& cachePath);

        static void CreateCubeMap();

        static void SaveDDSFile(const std::string& filePath, int width, int height, const std::vector<uint8_t>& compressedData);
    private:
    };

    //OPTIMIZE IT
    template <typename T>
    void FlipBitmapVertically(std::vector<T>& bitmap, int width, int height, int channels) {
        int stride = width * channels;
        std::vector<T> rowBuffer(stride);

        for (int y = 0; y < height / 2; ++y) {
            T* topRow = bitmap.data() + y * stride;
            T* bottomRow = bitmap.data() + (height - y - 1) * stride;

            // Swap the rows
            std::memcpy(rowBuffer.data(), topRow, stride);
            std::memcpy(topRow, bottomRow, stride);
            std::memcpy(bottomRow, rowBuffer.data(), stride);
        }
    }

    //move to tpp file
    template <typename T>
    void TextureImporter::SaveFile(const T * bitmap, int width, int height, std::filesystem::path& cachePath)
    {
        int totalSize = width * height * 4;  // Assuming 4 bytes per pixel (RGBA)
        std::vector<T> bitmapCopy(bitmap, bitmap + totalSize);

        //FlipBitmapVertically(bitmapCopy, width, height, 4);


        bc7_enc_settings settings;
        GetProfile_alpha_basic(&settings);

        rgba_surface srcSurface;
        srcSurface.ptr = bitmapCopy.data();
        srcSurface.width = width;
        srcSurface.height = height;
        srcSurface.stride = width * 4;

        int compressedSize = (width / 4) * (height / 4) * 16;
        std::vector<uint8_t> compressedData(compressedSize);

        //CompressBlocksBC7(&srcSurface, compressedData.data(), &settings);

        CompressBlocksBC3(&srcSurface, compressedData.data());

        SaveDDSFile(cachePath.string(), width, height, compressedData);
    }
}

#endif
