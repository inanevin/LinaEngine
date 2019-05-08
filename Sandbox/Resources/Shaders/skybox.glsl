/*
 * Copyright (C) 2019 Inan Evin
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "common.glh"


#if defined(VS_BUILD)
Layout(0) attribute vec3 position;
Layout(4) attribute mat4 transformMat;

out vec3 TexCoords;

void main()
{
    TexCoords = position;
    vec4 pos = vec4(position, 1.0) * transformMat;
    gl_Position = pos.xyww;
}  

#elif defined(FS_BUILD)
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}
#endif
