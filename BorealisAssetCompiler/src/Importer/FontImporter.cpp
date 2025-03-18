/******************************************************************************
/*!
\file       FontImporter.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 30, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "Importer/FontImporter.hpp"
#include "Importer/TextureImporter.hpp"

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<BorealisAssetCompiler::FontQuadBound>
	{
		static Node encode(const BorealisAssetCompiler::FontQuadBound& quadBound)
		{
			Node node;
			node["top"] =		quadBound.top;
			node["bottom"] =	quadBound.bottom;
			node["left"] =		quadBound.left;
			node["right"] =		quadBound.right;

			return node;
		}

		static bool decode(const Node& node, BorealisAssetCompiler::FontQuadBound& quadBound)
		{
			if (!node.IsMap())
			{
				return false;
			}

			quadBound.top =		node["top"].as<double>();
			quadBound.bottom =	node["bottom"].as<double>();
			quadBound.left =	node["left"].as<double>();
			quadBound.right =	node["right"].as<double>();

			return true;
		}
	};

	template<>
	struct convert<BorealisAssetCompiler::FontGlyph>
	{
		static Node encode(const BorealisAssetCompiler::FontGlyph& glyph)
		{
			Node node;
			node["advance"] = glyph.advance;
			node["atlasBound"] = glyph.altasBound;
			node["planeBound"] = glyph.planeBound;

			return node;
		}

		static bool decode(const Node& node, BorealisAssetCompiler::FontGlyph& glyph)
		{
			if (!node.IsMap())
			{
				return false;
			}

			glyph.advance = node["advance"].as<double>();
			glyph.altasBound = node["atlasBound"].as<BorealisAssetCompiler::FontQuadBound>();
			glyph.altasBound = node["planeBound"].as<BorealisAssetCompiler::FontQuadBound>();

			return true;
		}
	};

	template<>
	struct convert<BorealisAssetCompiler::FontInfo>
	{
		static Node encode(const BorealisAssetCompiler::FontInfo& fontInfo)
		{
			Node node;
			node["ascenderY"] = fontInfo.ascenderY;
			node["descenderY"] = fontInfo.descenderY;

			Node glyphsNode;
			for (const auto& glyphPair : fontInfo.glyphs) {
				glyphsNode[std::string(1, glyphPair.first)] = glyphPair.second;
			}
			node["glyphs"] = glyphsNode;

			Node kerningsNode;
			for (const auto& kerningPair : fontInfo.kernings) {
				Node keyNode;
				keyNode.push_back(kerningPair.first.first);
				keyNode.push_back(kerningPair.first.second);
				kerningsNode[keyNode] = kerningPair.second;
			}
			node["kernings"] = kerningsNode;

			return node;
		}

		static bool decode(const Node& node, BorealisAssetCompiler::FontInfo& fontInfo)
		{
			if (!node.IsMap())
			{
				return false;
			}

			fontInfo.ascenderY = node["ascenderY"].as<double>();
			fontInfo.descenderY = node["descenderY"].as<double>();

			if (node["glyphs"]) {
				for (const auto& glyphNode : node["glyphs"]) {
					char key = glyphNode.first.as<std::string>()[0];
					fontInfo.glyphs[key] = glyphNode.second.as<BorealisAssetCompiler::FontGlyph>();
				}
			}

			if (node["kernings"]) {
				for (const auto& kerningNode : node["kernings"]) {
					std::pair<uint32_t, uint32_t> key;
					key.first = kerningNode.first[0].as<uint32_t>();
					key.second = kerningNode.first[1].as<uint32_t>();
					fontInfo.kernings[key] = kerningNode.second.as<double>();
				}
			}

			return true;
		}
	};
}

namespace BorealisAssetCompiler
{
	template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
	static void CreateAndCacheAtlas(const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
		const msdf_atlas::FontGeometry& fontGeometry, uint32_t width, uint32_t height, std::filesystem::path cachePath)
	{
		msdf_atlas::GeneratorAttributes attributes;
		attributes.config.overlapSupport = true;
		attributes.scanlinePass = true;

		msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
		generator.setAttributes(attributes);
		generator.setThreadCount(8);
		generator.generate(glyphs.data(), (int)glyphs.size());

		msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

		msdfgen::savePng(bitmap, cachePath.replace_extension(".png").string().c_str());

		TextureImporter::SaveFile(bitmap.pixels, bitmap.width, bitmap.height, cachePath.replace_extension(".dds"));
	}

	void FontImporter::SaveFile(std::filesystem::path const& fontPath, std::filesystem::path& cachePath)
	{
		cachePath.replace_extension(".dds");
		FontInfo fontInfo = generateAtlas(fontPath, cachePath);
		cachePath.replace_extension(".bfi");
		SerializeFontInfo(fontInfo, cachePath);
	}

	FontInfo FontImporter::generateAtlas(std::filesystem::path fontPath, std::filesystem::path cachePath)
	{
		FontInfo fontInfo;
		if (msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype())
		{
			if (msdfgen::FontHandle* font = loadFont(ft, fontPath.string().c_str()))
			{
				std::vector<msdf_atlas::GlyphGeometry> glyphs;

				static const uint32_t ranges[] =
				{
					0x0020, 0x00FF,
					0
				};

				msdf_atlas::Charset charSet;
				for (int range = 0; range < 2; range += 2)
				{
					for (uint32_t c = ranges[range]; c <= ranges[range + 1]; c++)
					{
						charSet.add(c);
					}
				}
				double fontScale = 8.0; // let meta decide in the future
				msdf_atlas::FontGeometry fontGeometry = msdf_atlas::FontGeometry(&glyphs);
				int loadedGlyphs = fontGeometry.loadCharset(font, fontScale, charSet);


				msdf_atlas::TightAtlasPacker packer;
				packer.setPixelRange(10.0);
				packer.setUnitRange(0.5);
				packer.setMiterLimit(1.0);
				packer.setScale(40.0);
				int remain = packer.pack(glyphs.data(), (int)glyphs.size());

				int width, height;
				packer.getDimensions(width, height);


#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define ANGLE_THRESHOULD 3.0
				int threadCount = 8;
				uint64_t coloringSeed = 0;
				bool expensiveColoring = true;
				if (expensiveColoring) {
					msdf_atlas::Workload([&glyphs, &coloringSeed](int i, int threadNo) -> bool {
						unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
						glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, ANGLE_THRESHOULD, glyphSeed);
						return true;
						}, (int)glyphs.size()).finish(threadCount);
				}
				else {
					unsigned long long glyphSeed = coloringSeed;
					for (msdf_atlas::GlyphGeometry& glyph : glyphs) {
						glyphSeed *= LCG_MULTIPLIER;
						glyph.edgeColoring(msdfgen::edgeColoringInkTrap, ANGLE_THRESHOULD, glyphSeed);
					}
				}

				fontInfo.ascenderY = fontGeometry.getMetrics().ascenderY;
				fontInfo.descenderY = fontGeometry.getMetrics().descenderY;

				//fontInfo.fontAtlas = CreateAndCacheAtlas<uint8_t, float, 4, msdf_atlas::mtsdfGenerator>("Test", (float)packer.getScale(), glyphs, fontGeometry, width, height);
				CreateAndCacheAtlas<uint8_t, float, 4, msdf_atlas::mtsdfGenerator>(glyphs, fontGeometry, width, height, cachePath);

				for (auto glyph1 = charSet.begin(); glyph1 != charSet.end(); glyph1++)
				{
					FontGlyph glyph;
					const msdf_atlas::GlyphGeometry* glyphGeometry = fontGeometry.getGlyph(*glyph1);
					if (glyphGeometry)
					{
						glyph.advance = fontGeometry.getGlyph(*glyph1)->getAdvance();
						fontGeometry.getGlyph(*glyph1)->getQuadAtlasBounds(glyph.altasBound.left, glyph.altasBound.bottom, glyph.altasBound.right, glyph.altasBound.top);
						fontGeometry.getGlyph(*glyph1)->getQuadPlaneBounds(glyph.planeBound.left, glyph.planeBound.bottom, glyph.planeBound.right, glyph.planeBound.top);
						fontInfo.glyphs.insert({ *glyph1, glyph });
					}

					for (auto glyph2 = charSet.begin(); glyph2 != charSet.end(); glyph2++)
					{
						double glyphAdvance;
						if (fontGeometry.getAdvance(glyphAdvance, *glyph1, *glyph2))
						{
							fontInfo.kernings.insert({ {*glyph1, *glyph2}, glyphAdvance });
						}
					}
				}

				msdfgen::destroyFont(font);
			}
			msdfgen::deinitializeFreetype(ft);
		}

		return fontInfo;
	}

	void FontImporter::SerializeFontInfo(FontInfo const& fontInfo, std::filesystem::path const& cachePath)
	{
		//YAML::Node node = YAML::convert<FontInfo>::encode(fontInfo);
		//std::ofstream fout(cachePath);
		//fout << node;
		//fout.close();

		//std::ofstream out(cachePath, std::ios::binary);
		//out.write(reinterpret_cast<const char*>(&fontInfo), sizeof(FontInfo));
		//out.close();

		std::ofstream out(cachePath, std::ios::binary);

		// Serialize doubles
		out.write(reinterpret_cast<const char*>(&fontInfo.ascenderY), sizeof(double));
		out.write(reinterpret_cast<const char*>(&fontInfo.descenderY), sizeof(double));

		// Serialize the unordered_map<char, FontGlyph>
		size_t glyphCount = fontInfo.glyphs.size();
		out.write(reinterpret_cast<const char*>(&glyphCount), sizeof(size_t));
		for (const auto& [key, value] : fontInfo.glyphs) {
			out.write(reinterpret_cast<const char*>(&key), sizeof(char));
			out.write(reinterpret_cast<const char*>(&value), sizeof(FontGlyph));
		}

		// Serialize the map<std::pair<uint32_t, uint32_t>, double>
		size_t kerningCount = fontInfo.kernings.size();
		out.write(reinterpret_cast<const char*>(&kerningCount), sizeof(size_t));
		for (const auto& [key, value] : fontInfo.kernings) {
			out.write(reinterpret_cast<const char*>(&key.first), sizeof(uint32_t));
			out.write(reinterpret_cast<const char*>(&key.second), sizeof(uint32_t));
			out.write(reinterpret_cast<const char*>(&value), sizeof(double));
		}

		out.close();
	}
}

