#include "Core/ApplicationDelegate.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/CommonCore.hpp"

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
		rm.RegisterResourceType<EntityWorld>(25, {"linaworld"}, PackageType::Package1);
	}

	void ApplicationDelegate::RegisterAppResources(ResourceManager& rm)
	{
		Vector<ResourceIdentifier> list;

		/* Priority Resources */
		list.push_back(ResourceIdentifier("Resources/Core/Textures/StubBlack.png", GetTypeID<Texture>(), 0, false, ResourceTag::Priority));
		list.push_back(ResourceIdentifier("Resources/Core/Shaders/GUIStandard.linashader", GetTypeID<Shader>(), 0, true, ResourceTag::Priority));
		list.push_back(ResourceIdentifier(DEFAULT_FONT_PATH, GetTypeID<Font>(), 0, true, ResourceTag::Priority));

		/* Core Resources */
		list.push_back(ResourceIdentifier("Resources/Core/Textures/StubLinaLogo.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/StubLinaLogoWhite.png", GetTypeID<Texture>(), 0, true, ResourceTag::Core));
		list.push_back(ResourceIdentifier("Resources/Core/Models/LinaLogo.glb", GetTypeID<Model>(), 0, false, ResourceTag::Core));
		list.push_back(ResourceIdentifier("Resources/Core/Textures/Checkered.png", GetTypeID<Texture>(), 0, false, ResourceTag::Priority));

		for (auto& r : list)
			r.sid = TO_SID(r.path);

		rm.RegisterAppResources(list);
	}

	bool ApplicationDelegate::FillResourceCustomMeta(StringID sid, OStream& stream)
	{
		if (sid == DEFAULT_FONT_SID)
		{
			Font::Metadata customMeta = {
				.points		 = {{.size = 14, .dpiLimit = 10.1f}, {.size = 14, .dpiLimit = 1.8f}, {.size = 14, .dpiLimit = 10.0f}},
				.isSDF		 = false,
				.glyphRanges = {linatl::make_pair(160, 360)},

			};
			customMeta.SaveToStream(stream);
			return true;
		}
		// NOTE: 160, 380 is the glyph range for nunito sans

		if (sid == DEFAULT_TEXTURE_CHECKERED || sid == DEFAULT_TEXTURE_LINALOGO || sid == "Resources/Core/Textures/StubLinaLogoWhite.png"_hs)
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
