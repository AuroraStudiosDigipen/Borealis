/******************************************************************************
/*!
\file       Font.cpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 19, 2024
\brief      Defines

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include <BorealisPCH.hpp>


#include <Graphics/Texture.hpp>
#include <Graphics/Font.hpp>

namespace Borealis
{
	static Ref<Font> font;

	Font::Font()
	{
		
	}

	Font::Font(FontInfo const& fontInfo)
	{
		mFontInfo = fontInfo;
	}

	Font::Font(std::filesystem::path const& filePath)
	{
		FontInfo fontInfo;
		//std::ifstream in(filePath, std::ios::binary);
		//in.read(reinterpret_cast<char*>(&fontInfo), sizeof(FontInfo));
		//in.close();
		//mFontInfo = MakeRef<FontInfo>(fontInfo);
		

		std::ifstream in(filePath, std::ios::binary);

		// Deserialize doubles
		in.read(reinterpret_cast<char*>(&fontInfo.ascenderY), sizeof(double));
		in.read(reinterpret_cast<char*>(&fontInfo.descenderY), sizeof(double));

		// Deserialize the unordered_map<char, FontGlyph>
		size_t glyphCount;
		in.read(reinterpret_cast<char*>(&glyphCount), sizeof(size_t));
		for (size_t i = 0; i < glyphCount; ++i) {
			char key;
			FontGlyph value;
			in.read(reinterpret_cast<char*>(&key), sizeof(char));
			in.read(reinterpret_cast<char*>(&value), sizeof(FontGlyph));
			fontInfo.glyphs[key] = value;
		}

		// Deserialize the map<std::pair<uint32_t, uint32_t>, double>
		size_t kerningCount;
		in.read(reinterpret_cast<char*>(&kerningCount), sizeof(size_t));
		for (size_t i = 0; i < kerningCount; ++i) {
			uint32_t first, second;
			double value;
			in.read(reinterpret_cast<char*>(&first), sizeof(uint32_t));
			in.read(reinterpret_cast<char*>(&second), sizeof(uint32_t));
			in.read(reinterpret_cast<char*>(&value), sizeof(double));
			fontInfo.kernings[{first, second}] = value;
		}

		in.close();

		mFontInfo = fontInfo;
	}

	Font::~Font()
	{

	}

	FontInfo const& Font::GetFontInfo() const
	{
		return mFontInfo;
	}

	Ref<Texture2D> Font::GetAtlasTexture() const
	{
		return mFontInfo.fontAtlas;
	}

	void Font::SetTexture(std::filesystem::path const& fontTexturePath)
	{
		mFontInfo.fontAtlas = Texture2D::Create(fontTexturePath.string());
	}

	void Font::SetDefaultFont(Ref<Font> defaultFont)
	{
		font = defaultFont;
	}

	Ref<Font> Font::GetDefaultFont()
	{
		if (!font)
			return nullptr;
		return font;
	}
	void Font::swap(Asset& other)
	{
		Font& o = static_cast<Font&>(other);
		std::swap(mFontInfo, o.mFontInfo);
	}
}

