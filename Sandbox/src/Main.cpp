/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Data/String.hpp"
#include "Data/Vector.hpp"
#include "Profiling/Profiler.hpp"
#include "Math/Color.hpp"
#include "Data/CommonData.hpp"
#include "World/Core/Component.hpp"
#include "Reflection/ReflectionSystem.hpp"
#include "DummySource.hpp"
#include "Core/StringID.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Memory/MemoryAllocatorPool.hpp"
#include "Resources/Core/IResource.hpp"
#include "Resources/Core/ResourcesReflection.hpp"
#include "Core/Editor.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include <Windows.h>

class Test : public Lina::IResource
{
public:
    int   a = 0;
    int   b = 12;
    float c = 2.0f;

    // Inherited via IResource
    virtual void SaveToStream(Lina::OStream& stream) override
    {
        stream << a << b << c;
    }
    virtual void LoadFromStream(Lina::IStream& stream) override
    {
        stream >> a >> b >> c;
    }

    // Inherited via IResource
    virtual void Flush() override
    {
    }
};

LINA_REGISTER_RESOURCE_TYPE(Test, 1, Lina::Vector<Lina::String>({"test"}), Lina::PackageType::Static);

void xd()
{
}

struct ParamPack
{
    int   a = 0;
    float b = 0;
};

void xd2(ParamPack a)
{
    int   ax = a.a;
    float bx = a.b;
    int   c  = 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{

    // Win32 stuff
    MSG msg    = {0};
    msg.wParam = 0;

   // Test t;
   // Test t2;
    // t.a = 99;
    // t.b = 18;
    // t.c = 9.0f;
    // t2.c = 229.0f;
    // t.SaveToFile("Resources/Test.test");
    // t2.SaveToFile("Resources/Test2.test");

    Lina::String path  = "Resources/Test.test";
    Lina::String path2 = "Resources/Test2.test";
    auto         id    = Lina::ResourceIdentifier{Lina::GetTypeID<Test>(), Lina::TO_SID(path), path};
    auto         id2    = Lina::ResourceIdentifier{Lina::GetTypeID<Test>(), Lina::TO_SID(path2), path2};

    Lina::ResourceManager::Get().LoadResources({id, id2}, true);
   // auto t = Lina::ResourceManager::Get().GetResource<Test>(Lina::TO_SID(path));
   // auto t2 = Lina::ResourceManager::Get().GetResource<Test>(Lina::TO_SID(path2));

 // Lina::Editor::Editor ed;
 // ed.PackageResources({id, id2});
    if (AllocConsole() == FALSE)
    {
        // err
    }

    // tf::Executor exec;
    // tf::Taskflow tf;
    //
    // Lina::EntityWorld* world = new Lina::EntityWorld();
    // Lina::Entity*      ent   = world->CreateEntity("My Entity");
    //
    // ent->SetPosition(Lina::Vector3(15, 25, 15));
    //
    // auto comp                = world->AddComponent<DummySource>(ent);
    // comp.Get().MovementSpeed = 2;
    // Lina::OStream test;
    // world->SaveToStream(test);
    // auto str = "TestWorld" + TO_STRING(0);
    // world->SaveToFile(str.c_str());
    // delete world;
    // test.Destroy();
    //
    //   while (true)
    //   {
    //       while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    //       {
    //           TranslateMessage(&msg);
    //           DispatchMessage(&msg);
    //       }
    //
    //       if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
    //           break;
    //   }

    FreeConsole();
}

#else

int main(int argc, char* argv[])
{
    return 0;
}

#endif