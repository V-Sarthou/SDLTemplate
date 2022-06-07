cmake_minimum_required(VERSION 3.12)

project(MySDLProject VERSION 0.1.0 LANGUAGES C)

include(GNUInstallDirs)
include(FetchContent)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Link with MT when using MSVC
if(MSVC)
  set(CompilerFlags
      CMAKE_C_FLAGS
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_RELEASE
      )
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  endforeach()
endif()

set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Add GLAD
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        master
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
  FetchContent_Populate(glad)
endif()
set(GLAD_API "gl=4.6" CACHE STRING "")
set(GLAD_PROFILE "core" CACHE STRING "")
set(GLAD_EXPORT OFF CACHE BOOL "" FORCE)
add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR})

# Add SDL2
FetchContent_Declare(
  sdl2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        main
)
FetchContent_GetProperties(sdl2)
if(NOT sdl2_POPULATED)
  FetchContent_Populate(sdl2)
endif()
set(SDL_FORCE_STATIC_VCRT ON CACHE BOOL "" FORCE)
set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "" FORCE)
set(SDL_STATIC_PIC ON CACHE BOOL "" FORCE)
set(SDL_LIBC ON CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR})

if(WIN32)
  set(CMAKE_WIN32_EXECUTABLE ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(src)
