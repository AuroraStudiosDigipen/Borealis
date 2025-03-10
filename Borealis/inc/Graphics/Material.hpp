/******************************************************************************/
/*!
\file		Material.hpp
\author 	Chua Zheng Yang
\par    	email: c.zhengyang\@digipen.edu
\date   	July 22, 2024
\brief		Declares the class for Material for rendering

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include <unordered_map>
#include <glm/glm.hpp>

#include <Assets/Asset.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Shader.hpp>
#include <Core/Core.hpp>

namespace Borealis
{
	class Material : public Asset
	{
	public:
		enum TextureMaps : uint8_t
		{
			Albedo,
			Specular,
			Metallic,
			NormalMap,
			HeightMap,
			Occlusion,
			DetailMask,
			Emission,
			UnknownTextureMap
		};

		enum Props : uint8_t
		{
			Tiling,
			Offset,
			Smoothness,
			Shininess,
			HexSize,
			Sharpness,
			HasEmission,
			HasHeightMap,
			HasNormalMap,
			HasOcclusion,
			HasDetailMask,
			UnknownProperty
		};

		/*!***********************************************************************
			\brief
				Constructor for a Material with a shader
			\param[in] shader
				Shader to be used for rendering
		*************************************************************************/
		Material(Ref<Shader> shader): mShader(shader) { Init(); }

		/*!***********************************************************************
			\brief
				Constructor for a Material from a path
			\param[in] path
				Path of the material
		*************************************************************************/
		Material(std::filesystem::path path);

		/*!***********************************************************************
			\brief
				Destructor for Material
		*************************************************************************/
		~Material() {}

		/*!***********************************************************************
			\brief
				Create a new material
			\param[in] path
				Path to store the material file
		*************************************************************************/
		static Ref<Material> CreateNewMaterial(std::filesystem::path const& path);

		/*!***********************************************************************
			\brief
				Serialize the material
			\param[in] path
				Path to store the material file
		*************************************************************************/
		void SerializeMaterial(std::filesystem::path const& path);

		// Setter for texture maps
		/*!***********************************************************************
			\brief
				Setter for texture maps
			\param[in] map
				Type of map
			\texture[in] texture
				The texture map
		*************************************************************************/
		void SetTextureMap(TextureMaps map, Ref<Texture2D> texture) { mTextureMaps[map] = texture; }

		/*!***********************************************************************
			\brief
				Setter for texture map color
			\param[in] map
				Type of map
			\texture[in] color
				The color
		*************************************************************************/
		void SetTextureMapColor(TextureMaps map, const glm::vec4& color) { mTextureMapColor[map] = color; }

		/*!***********************************************************************
			\brief
				Setter for texture map floats 
			\param[in] map
				Type of map
			\texture[in] value
				float to set
		*************************************************************************/
		void SetTextureMapFloat(TextureMaps map, float value) { mTextureMapFloat[map] = value; }

		/*!***********************************************************************
			\brief
				Setter for properties floats
			\param[in] prop
				Type of property
			\texture[in] value
				The value to set
		*************************************************************************/
		void SetPropertyFloat(Props prop, float value) { mPropertiesFloat[prop] = value; }

		/*!***********************************************************************
			\brief
				Setter for properties vec2s
			\param[in] prop
				Type of property
			\texture[in] value
				The value to set
		*************************************************************************/
		void SetPropertyVec2(Props prop, glm::vec2 value) { mPropertiesVec2[prop] = value; }
		
		/*!***********************************************************************
			\brief
				This function sets the uniforms in the material's shader
			\param[in] shader
				Temp variable to take in same shader as renderer
		*************************************************************************/
		void SetUniforms(Ref<Shader> shader);

		/*!***********************************************************************
			\brief
				Setter for material's name
			\param[in] name
				Material's name
		*************************************************************************/
		void SetName(std::string name) { mName = name; }

		/*!***********************************************************************
			\brief
				Gets the name of the material
			\return
				Name of material
		*************************************************************************/
		std::string GetName() const { return mName; }

		/*!***********************************************************************
			\brief
				Gets the texture maps of the material
			\return
				Texture maps of material
		*************************************************************************/
		std::unordered_map<TextureMaps, Ref<Texture2D>> GetTextureMaps() const { return mTextureMaps; }

		void RemoveTextureMap(TextureMaps map) { mTextureMaps.erase(map); }

		/*!***********************************************************************
			\brief
				Gets the texture map colors of the material
			\return
				Texture map colors of material
		*************************************************************************/
		std::unordered_map<TextureMaps, glm::vec4> GetTextureMapColor() const { return mTextureMapColor; }

		/*!***********************************************************************
			\brief
				Gets the texture map floats of the material
			\return
			    Texture map floats of material
		*************************************************************************/
		std::unordered_map<TextureMaps, float> GetTextureMapFloats() const { return mTextureMapFloat; }

		/*!***********************************************************************
			\brief
				Gets the property floats of the material
			\return
				Property floats of material
		*************************************************************************/
		std::unordered_map<Props, float> GetPropertiesFloats() const { return mPropertiesFloat; }

		/*!***********************************************************************
			\brief
				Gets the properties vec2 of the material
			\return
				Properties vec2 of material
		*************************************************************************/
		std::unordered_map<Props, glm::vec2> GetPropertiesVec2() const { return mPropertiesVec2; }

		std::size_t hash = 0;

		bool isModified = false;

		bool isTransparent = false;

		Ref<Shader> GetShader() { return mShader; }

		int GetIndex() { return mMaterialIndex; }
		void SetIndex(int index) { mMaterialIndex = index; }

		/*!***********************************************************************
			\brief
				Returns the texture map enums in strings
			\return
				Texture map enums
		*************************************************************************/
		static std::string TextureMapToString(TextureMaps map);

		/*!***********************************************************************
			\brief
				Returns the Property enums in enum
			\return
				Property enums
		*************************************************************************/
		static TextureMaps StringToTextureMaps(const std::string& str);

		/*!***********************************************************************
			\brief
				Returns the Property enums in strings
			\return
				Property enums
		*************************************************************************/
		static std::string PropsToString(Props prop);

		/*!***********************************************************************
			\brief
				Returns the Property enums in enum
			\return
				Property enums
		*************************************************************************/
		static Props StringToProps(const std::string& str);

		static Ref<Asset> Load(std::filesystem::path const& cachePath, AssetMetaData const& assetMetaData);

		bool mNonRepeatingTiles = false;
	private:
		/*!***********************************************************************
			\brief
				Initializes the material class
		*************************************************************************/
		void Init();

		std::unordered_map<TextureMaps, Ref<Texture2D>> mTextureMaps; //Texture maps
		std::unordered_map<TextureMaps, glm::vec4> mTextureMapColor; //Texture map colors
		std::unordered_map<TextureMaps, float> mTextureMapFloat; //Texture map floats
		std::unordered_map<Props, float> mPropertiesFloat; //Properties
		std::unordered_map<Props, glm::vec2> mPropertiesVec2; //Properties
		Ref<Shader> mShader; //Shader
		std::string mName;
		int mMaterialIndex = -1;

		RTTR_ENABLE(Asset)
	}; //class Material

	struct MaterialUBOData
	{
		glm::vec4 albedoColor;
		glm::vec4 specularColor;
		glm::vec4 emissionColor;

		glm::vec2 tiling;
		glm::vec2 offset;

		float smoothness = 0;
		float shininess = 0;
		float metallic = 0;

		int nonRepeatingTiles = false;

		int hasAlbedoMap = false;
		int hasEmissionMap = false;
		int hasNormalMap = false;
		int hasMetallicMap = false;
	};
} //namespace Borealis
#endif