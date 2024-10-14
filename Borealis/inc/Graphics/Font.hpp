/******************************************************************************
/*!
\file       Font.hpp
\author     Chan Guo Geng Gordon
\par        email: g.chan/@digipen.edu
\date       September 19, 2024
\brief      Declares

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef Font_HPP
#define Font_HPP

#include <map>
#include <Assets/Asset.hpp>
namespace Borealis
{
	class Texture2D; //forward declare
	struct FontQuadBound
	{
		double top;
		double bottom;
		double left;
		double right;
	};
	struct FontGlyph
	{
		double advance;
		FontQuadBound altasBound;
		FontQuadBound planeBound;
	};

	struct FontInfo
	{
		double ascenderY;
		double descenderY;
		std::unordered_map<char, FontGlyph> glyphs;
		std::map<std::pair<uint32_t, uint32_t>, double> kernings;
		Ref<Texture2D> fontAtlas;
	};

	class Font : public Asset
	{
	public:
		Font();
		Font(FontInfo const& fontInfo);
		Font(std::filesystem::path const& filePath);
		~Font();

		FontInfo const& GetFontInfo() const;

		Ref<Texture2D> GetAtlasTexture() const;
		void SetTexture(std::filesystem::path const& fontTexturePath); //Temp
		static void SetDefaultFont(Ref<Font> defaultFont);
		static Ref<Font> GetDefaultFont();

	private:
		FontInfo mFontInfo;
	};
}

#endif
