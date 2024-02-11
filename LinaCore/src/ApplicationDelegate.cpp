#include "Core/ApplicationDelegate.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"

namespace Lina
{
	void ApplicationDelegate::RegisterResourceTypes(ResourceManager& rm)
	{
		rm.RegisterResourceType<Model>(50, {"glb"}, PackageType::Package1);
		rm.RegisterResourceType<Shader>(25, {"linashader"}, PackageType::Package1);
		rm.RegisterResourceType<Texture>(100, {"png", "jpeg", "jpg"}, PackageType::Package1);
		rm.RegisterResourceType<TextureSampler>(100, {"linasampler"}, PackageType::Package1);
		rm.RegisterResourceType<Font>(10, {"ttf", "otf"}, PackageType::Package1);
		rm.RegisterResourceType<Material>(25, {"linamaterial"}, PackageType::Package1);
	}

	Vector<ResourceIdentifier> ApplicationDelegate::GetPriorityResources()
	{
		Vector<ResourceIdentifier> list;

		list.push_back(ResourceIdentifier("Resources/Core/Fonts/NunitoSansRegular.ttf", GetTypeID<Font>(), 0, true));
		list.push_back(ResourceIdentifier("Resources/Core/Fonts/RubikSansRegular.ttf", GetTypeID<Font>(), 0, true));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/StubBlack.png", GetTypeID<Texture>(), 0));
		list.push_back(ResourceIdentifier("Resources/Core/Shaders/GUIStandard.linashader", GetTypeID<Shader>(), 0, true));
		// list.push_back(ResourceIdentifier("Resources/Core/Shaders/UnlitStandard.linashader", GetTypeID<Shader>(), 0));
		// list.push_back(ResourceIdentifier("Resources/Core/Shaders/ScreenQuads/SQTexture.linashader", GetTypeID<Shader>(), 0));

		for (auto& ident : list)
			ident.sid = TO_SID(ident.path);

		return list;
	}

	Vector<ResourceIdentifier> ApplicationDelegate::GetCoreResources()
	{
		Vector<ResourceIdentifier> list;

		list.push_back(ResourceIdentifier("Resources/Core/Textures/StubLinaLogo.png", GetTypeID<Texture>(), 0, true));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/StubLinaLogoWhite.png", GetTypeID<Texture>(), 0, true));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/StubLinaLogoText.png", GetTypeID<Texture>(), 0, true));
		list.push_back(ResourceIdentifier("Resources/Core/Models/LinaLogo.glb", GetTypeID<Model>(), 0));

		for (auto& ident : list)
			ident.sid = TO_SID(ident.path);

		return list;
	}

	bool ApplicationDelegate::FillResourceCustomMeta(StringID sid, OStream& stream)
	{

		if (sid == "Resources/Core/Fonts/NunitoSansRegular.ttf"_hs)
		{
			Font::Metadata customMeta = {
				.points		 = {{.size = 12, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.15f}, {.size = 16, .dpiLimit = 1.35f}, {.size = 20, .dpiLimit = 10.0f}},
				.isSDF		 = false,
				.glyphRanges = {linatl::make_pair(160, 360)},
			};
			customMeta.SaveToStream(stream);
			return true;
		}

		if (sid == "Resources/Core/Fonts/RubikSanRegular.ttf"_hs)
		{
			Font::Metadata customMeta = {
				.points = {{.size = 12, .dpiLimit = 1.1f}, {.size = 14, .dpiLimit = 1.15f}, {.size = 16, .dpiLimit = 1.35f}, {.size = 18, .dpiLimit = 10.0f}},
				.isSDF	= false,
			};
			customMeta.SaveToStream(stream);
			return true;
		}

		// NOTE: 160, 380 is the glyph range for nunito sans

		if (sid == "Resources/Core/Textures/StubLinaLogo.png"_hs || sid == "Resources/Core/Textures/StubLinaLogoWhite.png"_hs || sid == "Resources/Core/Textures/StubLinaLogoText.png"_hs)
		{
			Texture::Metadata meta = {
				.samplerSID = DEFAULT_GUI_SAMPLER_SID,
			};

			meta.SaveToStream(stream);
			return true;
		}

		if (sid == DEFAULT_SHADER_GUI)
		{
			Shader::Metadata meta;

			meta.variants["RenderTarget"_hs] = ShaderVariant{
				.blendDisable = false,
				.depthDisable = true,
				.targetType	  = ShaderWriteTargetType::RenderTarget,
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CCW,

			};

			meta.variants["Swapchain"_hs] = ShaderVariant{
				.blendDisable = false,
				.depthDisable = true,
				.targetType	  = ShaderWriteTargetType::Swapchain,
				.cullMode	  = LinaGX::CullMode::None,
				.frontFace	  = LinaGX::FrontFace::CCW,
			};

			meta.descriptorSetAllocationCount = 50;
			meta.SaveToStream(stream);
			return true;
		}

		return false;
	}

} // namespace Lina
