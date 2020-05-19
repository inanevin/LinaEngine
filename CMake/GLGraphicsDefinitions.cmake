#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Author: Inan Evin
# www.inanevin.com
# 
# Copyright (C) 2018 Inan Evin
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
# and limitations under the License.
#-------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#----------------------------------- BUFFER USAGE DEFINITIONS ----------------------------------- #
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_BINDTEXTURE_TEXTURE2D=0x0DE1)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_BINDTEXTURE_CUBEMAP=0x8513)

#----------------------------------- BUFFER USAGE DEFINITIONS ----------------------------------- #
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_USAGE_STATIC_DRAW=0x88E4)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_USAGE_STREAM_DRAW=0x88E0)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_USAGE_DYNAMIC_DRAW=0x88E8)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_USAGE_STATIC_COPY=0x88E6)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_USAGE_STREAM_COPY=0x88E6)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_USAGE_DYNAMIC_COPY=0x88EA)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_USAGE_STATIC_READ=0x88E5)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_USAGE_STREAM_READ=0x88E1)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_USAGE_DYNAMIC_READ=0x88E9)

#----------------------------------- SAMPLER FILTER DEFINITIONS ----------------------------------- #
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_SAMPLER_FILTER_NEAREST=0x2600)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_SAMPLER_FILTER_LINEAR=0x2601)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_SAMPLER_FILTER_NEAREST_MIPMAP_NEAREST=0x2700)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_SAMPLER_FILTER_LINEAR_MIPMAP_NEAREST=0x2701)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_SAMPLER_FILTER_NEAREST_MIPMAP_LINEAR=0x2702)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_SAMPLER_FILTER_LINEAR_MIPMAP_LINEAR=0x2703)
                                                  
#----------------------------------- SAMPLER WRAP MODE DEFINITIONS ----------------------------------- # 
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_SAMPLER_WRAP_CLAMP=0x812F)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_SAMPLER_WRAP_REPEAT=0x2901)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_SAMPLER_WRAP_CLAMP_MIRROR=0x8743)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_SAMPLER_WRAP_REPEAT_MIRROR=0x8370)
                                                
#----------------------------------- PRIMITIVE DEFINITIONS ----------------------------------- #												
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_TRIANGLES=0x0004)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_POINTS=0x0000)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_LINE_STRIP=0x0003)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_LINE_LOOP=0x0002)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_LINES=0x0001)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_LINE_STRIP_ADJACENCY=0x000B)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_LINES_ADJACENCY=0x000A)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_TRIANGLE_STRIP=0x0005)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_TRIANGLE_FAN=0x0006)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY=0x000D)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_TRIANGLES_ADJACENCY=0x000C)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_PRIMITIVE_PATCHES=0x000E)
                                               
#----------------------------------- FACE CULLING DEFINITIONS ----------------------------------- #											   
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_FACE_CULL_NONE=)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_FACE_CULL_BACK=0x0405)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_FACE_CULL_FRONT=0x0404)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_FACE_CULL_FRONT_AND_BACK=0x0408)
                                             
#----------------------------------- DRAW FUNCTIONS DEFINITIONS ----------------------------------- #											 
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_DRAW_FUNC_NEVER=0x0200)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_DRAW_FUNC_ALWAYS=0x0207)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_DRAW_FUNC_LESS=0x0201)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_DRAW_FUNC_GREATER=0x0204)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_DRAW_FUNC_LEQUAL=0x0203)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_DRAW_FUNC_GEQUAL=0x0206)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_DRAW_FUNC_EQUAL=0x0202)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_DRAW_FUNC_NOT_EQUAL=0x0205)
                                   
#----------------------------------- FRAME ATTACHMENT DEFINITIONS ----------------------------------- #								   
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_ATTACHMENT_COLOR=0x8CE0)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_ATTACHMENT_DEPTH=0x8D00)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_ATTACHMENT_STENCIL=0x8D20)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_ATTACHMENT_DEPTHANDSTENCIL=0x821A)

#----------------------------------- RENDER BUFFER STORAGE DEFINITIONS ----------------------------------- #								   
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STORAGE_DC16=0x81A5)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_sTORAGE_DC24=0x81A6)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STORAGE_DC32F=0x8CAC)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STORAGE_D24S8=0x88F0)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STORAGE_D32FS8=0x8CAD)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STORAGE_SI8=0x8D48)
#----------------------------------- BLEND FUNCTION DEFINITIONS ----------------------------------- #
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_BLEND_FUNC_NONE=)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_BLEND_FUNC_ONE=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_BLEND_FUNC_SRC_ALPHA=0x0302)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_BLEND_FUNC_ONE_MINUS_SRC_ALPHA=0x0303)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_BLEND_FUNC_ONE_MINUS_DST_ALPHA=0x0305)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_BLEND_FUNC_DST_ALPHA=0x0304)

#----------------------------------- STENCIL DEFINITIONS ----------------------------------- #
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STENCIL_KEEP=0x1E00)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STENCIL_ZERO=0)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STENCIL_REPLACE=0x1E01)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STENCIL_INCR=0x1E02)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STENCIL_INCR_WRAP=0x8507)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STENCIL_DECR_WRAP=0x8508)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STENCIL_DECR=0x1E03)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINA_GRAPHICS_STENCIL_INVERT=0x150A)