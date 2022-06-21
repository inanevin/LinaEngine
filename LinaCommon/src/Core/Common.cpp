#include "Core/CommonApplication.hpp"
#include "Core/CommonPhysics.hpp"
#include "Core/CommonUtility.hpp"

// EASTL OPERATOR NEW[] REQUIREMENTS

void* __cdecl operator new[](size_t size, size_t, size_t, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
    return new uint8_t[size];
}

void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
    return new uint8_t[size];
}

namespace EA
{
    namespace StdC
    {
        // Provided by the EAStdC package or by the user.
        int Vsnprintf(char* EA_RESTRICT pDestination, size_t n, const char* EA_RESTRICT pFormat, va_list arguments)
        {
        return vsnprintf(pDestination, n, pFormat, arguments);
        }
        int Vsnprintf(char16_t* EA_RESTRICT pDestination, size_t n, const char16_t* EA_RESTRICT pFormat, va_list arguments)
        {
            return 0;
        }
        int Vsnprintf(char32_t* EA_RESTRICT pDestination, size_t n, const char32_t* EA_RESTRICT pFormat, va_list arguments)
        {
            return 0;
        }
#if EA_CHAR8_UNIQUE
        int Vsnprintf(char8_t* EA_RESTRICT pDestination, size_t n, const char8_t* EA_RESTRICT pFormat, va_list arguments)
        {
            return 0;
        }
#endif
#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
        int Vsnprintf(wchar_t* EA_RESTRICT pDestination, size_t n, const wchar_t* EA_RESTRICT pFormat, va_list arguments)
        {
            return 0;
        }
#endif
    } // namespace StdC
} // namespace EA

// EASTL OPERATOR NEW[] REQUIREMENTS

namespace Lina
{
    String LogLevelAsString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Critical:
            return "Critical";
        case LogLevel::Debug:
            return "Debug";
        case LogLevel::Error:
            return "Error";
        case LogLevel::Info:
            return "Info";
        case LogLevel::None:
            return "None";
        case LogLevel::Trace:
            return "Trace";
        case LogLevel::Warn:
            return "Warn";
        default:
            return "";
        }
    }
} // namespace Lina

namespace Lina::Physics
{
    String SIMULATION_TYPES[3] = {"None", "Static", "Dynamic"};
    String COLLISION_SHAPES[4] = {"Box", "Sphere", "Capsule", "ConvexMesh"};
} // namespace Lina::Physics

namespace Lina::Utility
{
    Folder::~Folder()
    {
        for (int i = 0; i < m_files.size(); i++)
        {
            delete m_files[i];
        }

        m_files.clear();

        for (int i = 0; i < m_folders.size(); i++)
            delete m_folders[i];

        m_folders.clear();
    }
} // namespace Lina::Utility