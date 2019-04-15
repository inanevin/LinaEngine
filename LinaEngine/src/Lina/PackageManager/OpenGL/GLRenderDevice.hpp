/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: GLRenderDevice
Timestamp: 4/14/2019 5:11:22 PM

*/

#pragma once

#ifndef GLRenderDevice_HPP
#define GLRenderDevice_HPP



namespace LinaEngine
{

#define MAKEFOURCC(a, b, c, d)                              \
                ((uint32)(uint8)(a) | ((uint32)(uint8)(b) << 8) |       \
				((uint32)(uint8)(c) << 16) | ((uint32)(uint8)(d) << 24 ))

#define MAKEFOURCCDXT(a) MAKEFOURCC('D', 'X', 'T', a)
#define FOURCC_DXT1 MAKEFOURCCDXT('1')
#define FOURCC_DXT2 MAKEFOURCCDXT('2')
#define FOURCC_DXT3 MAKEFOURCCDXT('3')
#define FOURCC_DXT4 MAKEFOURCCDXT('4')
#define FOURCC_DXT5 MAKEFOURCCDXT('5')

	enum PixelFormat
	{
		FORMAT_R,
		FORMAT_RG,
		FORMAT_RGB,
		FORMAT_RGBA,
		FORMAT_DEPTH,
		FORMAT_DEPTH_AND_STENCIL,
	};


	struct ShaderProgram
	{
		//LinaArray<uint32> m_Shaders;
		//LinaMap<LinaString, uint32> m_UniformMap;
		//LinaMap<LinaString, uint32> m_SamplerMap;
	};

	class GLRenderDevice
	{
	public:



		//GLRenderDevice(class Application* app);
		//~GLRenderDevice();

		/* Creates the main window. */
		//Window* CreateMainWindow(class InputDevice* inputDevice, const class WindowProperties& properties);

		/* Starts the rendering renderDevice. */
		//void Initialize();

		/* Called in each frame. */
		//void OnUpdate();

		/* Called when an event occurs */
		//void OnEvent(class Event& e);

		//uint32 CreateTexture2D(int32 width, int32 height, const void* data, PixelFormat pixelDataFormat, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress);
		//uint32 CreateDDSTexture2D(uint32 width, uint32 height, const unsigned char* buffer, uint32 fourCC, uint32 mipMapCount);
		//uint32 ReleaseTexture2D(uint32 texture2D);
		//void SetShader(uint32 shader);
		//void SetShaderSampler(uint32 shader, const LinaString& samplerName, uint32 texture, uint32 sampler, uint32 unit);

	private:

		//uint32 m_BoundShader = 0;
		//class Application* application;
		//Window* m_Window;
	};
}


#endif