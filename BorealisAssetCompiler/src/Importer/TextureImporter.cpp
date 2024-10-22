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
#include "stb_image.h"
//#include "ispc_texcomp.h"
#include <cmp_compressonatorlib/compressonator.h>

#define FOURCC_DXT5 0x35545844  // 'DXT5' in ASCII
#define FOURCC_DX10 0x30315844  // 'DX10' in ASCII
#define DXGI_FORMAT_BC7_UNORM 98  // The BC7 format value for DX10 header

namespace BorealisAssetCompiler
{
    void compressonatorTest(std::string const& filePath)
    {
        CMP_InitFramework();

        CMP_MipSet MipSetIn;
        memset(&MipSetIn, 0, sizeof(CMP_MipSet));
        CMP_ERROR cmp_status = CMP_LoadTexture(filePath.c_str(), &MipSetIn);
        if (cmp_status != CMP_OK) {
            std::printf("Error %d: Loading source file!\n", cmp_status);
            return;
        }

        if (MipSetIn.m_nMipLevels <= 1)
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

        kernel_options.format = CMP_FORMAT_BC3;   // Set the format to process
        kernel_options.fquality = 0.5;     // Set the quality of the result (range of 0 - 1)
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
        cmp_status = CMP_SaveTexture("CMP_test.dds", &MipSetCmp);

        CMP_FreeMipSet(&MipSetIn);
        CMP_FreeMipSet(&MipSetCmp);
    }

    void TextureImporter::SaveDDSFile(const std::string& filePath, int width, int height, const std::vector<uint8_t>& compressedData)
    {
        //DDSHeader header = {};
        //header.dwMagic = 0x20534444;  // 'DDS '
        //header.dwSize = 124;
        //header.dwFlags = 0x1007;  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
        //header.dwHeight = height;
        //header.dwWidth = width;
        //header.dwPitchOrLinearSize = (width / 4) * (height / 4) * 16;  // Compressed size for BC7 blocks
        //header.dwMipMapCount = 1;

        //header.ddpf.dwSize = 32;
        //header.ddpf.dwFlags = 4;  // DDPF_FOURCC
        //header.ddpf.dwFourCC = FOURCC_DX10;  // 'DX10' FourCC for BC7

        //header.ddsCaps.dwCaps1 = 0x1000;  // DDSCAPS_TEXTURE

        //// DX10 extended header
        //DDSHeaderDX10 dx10Header = {};
        //dx10Header.dxgiFormat = DXGI_FORMAT_BC7_UNORM;
        //dx10Header.resourceDimension = 3;  // DDS_DIMENSION_TEXTURE2D
        //dx10Header.arraySize = 1;

        //// Write the header and compressed data to the DDS file
        //std::ofstream outFile(filePath, std::ios::binary);
        //if (outFile.is_open()) {
        //    outFile.write(reinterpret_cast<char*>(&header), sizeof(header));
        //    outFile.write(reinterpret_cast<char*>(&dx10Header), sizeof(dx10Header));
        //    outFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
        //    outFile.close();
        //}

        DDSHeader header = {};
        header.dwMagic = 0x20534444;  // 'DDS '
        header.dwSize = 124;
        header.dwFlags = 0x1007;  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
        header.dwHeight = height;
        header.dwWidth = width;
        header.dwPitchOrLinearSize = (width / 4) * (height / 4) * 16;  // Compressed size for BC3 blocks
        header.dwMipMapCount = 1;

        header.ddpf.dwSize = 32;
        header.ddpf.dwFlags = 4;  // DDPF_FOURCC
        header.ddpf.dwFourCC = FOURCC_DXT5;  // 'DXT5' FourCC for BC3 (DXT5)

        header.ddsCaps.dwCaps1 = 0x1000;  // DDSCAPS_TEXTURE

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

    void TextureImporter::SaveFile(std::filesystem::path const& sourcePath, std::filesystem::path & cachePath)
    {
        int width, height, channels;
        unsigned char* imageData = stbi_load(sourcePath.string().c_str(), &width, &height, &channels, 4); // Force 4 channels (RGBA)
        if (!imageData) {
            std::cerr << "Failed to load image\n";
            return;
        }

        bc7_enc_settings settings;
        GetProfile_alpha_basic(&settings);

        FlipBitmapVertically(imageData, width, height, 4);

        rgba_surface srcSurface;
        srcSurface.ptr = imageData;
        srcSurface.width = width;
        srcSurface.height = height;
        srcSurface.stride = width * 4;

        int compressedSize = (width / 4) * (height / 4) * 16;
        std::vector<uint8_t> compressedData(compressedSize);

        //CompressBlocksBC7(&srcSurface, compressedData.data(), &settings);
        CompressBlocksBC3(&srcSurface, compressedData.data());

        std::string cacheString = cachePath.replace_extension(".dds").string();

        SaveDDSFile(cacheString.c_str(), width, height, compressedData);

        stbi_image_free(imageData);

        compressonatorTest(sourcePath.string());
    }
}

