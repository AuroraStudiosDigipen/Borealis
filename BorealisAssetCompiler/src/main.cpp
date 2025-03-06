/******************************************************************************
/*!
\file       main.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 27, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <fstream>
#include <iostream>

#include "Importer/AssetImporter.hpp"
#include "MetaSerializer.hpp"

#include "Importer/TextureImporter.hpp"

int main(int argc, char** argv)
{
    if (argc != 2) 
    {
        std::cout << "Usage: " << argv[0] << " <filepath>" << std::endl;
        return -1;
    }

    std::filesystem::path filePath = argv[1];
    //std::filesystem::path filePath = "C:\\Users\\Owner\\Documents\\Digipen\\GAM300\\DTest\\Assets\\Dragon_Nor.png";

    filePath = filePath.string() + ".meta";

    BorealisAssetCompiler::AssetMetaData data = BorealisAssetCompiler::MetaSerializer::GetAssetMetaDataFile(filePath);

    BorealisAssetCompiler::AssetMetaData finalData = BorealisAssetCompiler::AssetImporter::ImportAsset(data);

    BorealisAssetCompiler::MetaSerializer::SaveMetaFile(finalData, filePath);

	return 1;
}

