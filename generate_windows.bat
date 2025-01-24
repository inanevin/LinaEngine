@echo off

rem Lina uses LLVM-19.1.7. Download from GitHub releases for your platform and pass its directory
rem into this script. Check the GitHub README for more information.

rem need to install clang-cl from visual studio components.

rem Ensure LLVM path is provided as an argument
rem if "%1"=="" (
rem     echo Error: Please provide the path to your LLVM installation as an argument.
rem     echo Usage: %0 "C:/path/to/llvm"
rem     exit /b 1
rem )

rem Set up paths
set BUILD_DIR=build_x64
set GENERATOR="Visual Studio 17 2022"
set TOOLCHAIN_FILE=CMake/Toolchain.cmake
rem set LLVM_DIR=%1
rem set CCOMPILER=%LLVM_DIR%/bin/clang.exe
rem set CPPCOMPILER=%LLVM_DIR%/bin/clang++.exe
rem set AR=%LLVM_DIR%/bin/llvm-ar.exe
rem set RANLIB=%LLVM_DIR%/bin/llvm-ranlib.exe
rem set LINKER=%LLVM_DIR%/bin/lld-link.exe
rem set LIBCXX_INCLUDE=%LLVM_DIR%/include/c++/v1
rem set LIBCXX_LIB=%LLVM_DIR%/lib

rem Check if the LLVM directory exists and contains the necessary tools
rem if not exist "%LLVM_DIR%" (
rem     echo Error: LLVM installation not found or incomplete at %LLVM_DIR%.
rem     exit /b 1
rem )
rem if not exist "%CCOMPILER%" (
rem     echo Error: clang compiler not found at %CCOMPILER%.
rem     exit /b 1
rem )
rem if not exist "%CPPCOMPILER%" (
rem     echo Error: clang++ compiler not found at %CPPCOMPILER%.
rem     exit /b 1
rem )

rem Create the build directory if it doesn't exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

rem Unset environment variables
set CFLAGS=
set CXXFLAGS=
set LDFLAGS=
rem set CC=%CCOMPILER%
rem set CXX=%CPPCOMPILER%

rem Run CMake
cmake ^
      -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN_FILE%" ^
      -DCMAKE_EXE_LINKER_FLAGS="%LDFLAGS%" ^
      -DCMAKE_EXE_LINKER_FLAGS_DEBUGASAN="%LDFLAGS%" ^
      -DCMAKE_EXE_LINKER_FLAGS_DEBUGPROFILE="%LDFLAGS%" ^
      -DCMAKE_EXE_LINKER_FLAGS_DEBUGFAST="%LDFLAGS%" ^
      -B "%BUILD_DIR%" -G %GENERATOR% -T v143

rem For using LLVM's libc++
rem Add -I$LIBCXX_INCLUDE to CFLAGS
rem Add -stdlib=libc++ -nostdinc++ -I$LIBCXX_INCLUDE to CXX FLAGS
rem Add -stdlib=libc++ -L$LIBCXX_LIB -lc++ -lc++abi -Wl,-rpath,$LIBCXX_LIB to LDFLAGS
rem Currently on MacOS this doesn't work, some errors with libc++ dylibs crashing app. 
rem -L$LIBCXX_LIB/unwind -lunwind
rem -fms-compatibility-version=19.1

rem Build the project (uncomment if needed)
rem cmake --build "%BUILD_DIR%" --config Debug --verbose

echo Generation complete.
