cmake_minimum_required(VERSION 3.16)
project(minisynth_gui CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# --------------------------
# PortAudio
# --------------------------
find_package(PortAudio QUIET)
if (NOT PortAudio_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
endif()

# --------------------------
# GLFW
# --------------------------
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.9
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# --------------------------
# ImGui
# --------------------------
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.90.9
)
FetchContent_MakeAvailable(imgui)

add_library(imgui_lib STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp

  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_lib PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

# --------------------------
# OpenGL
# --------------------------
find_package(OpenGL REQUIRED)

# Link dependencies that the ImGui backends actually use
target_link_libraries(imgui_lib PUBLIC glfw OpenGL::GL)

if(APPLE)
  target_link_libraries(imgui_lib PUBLIC
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
  )
  target_compile_definitions(imgui_lib PUBLIC GL_SILENCE_DEPRECATION)
endif()

# --------------------------
# Your executable
# --------------------------
add_executable(minisynth
  main.cpp
  Synth.cpp
  Oscillator.cpp
  gui.cpp
)

target_include_directories(minisynth PRIVATE .)

# Make absolutely sure minisynth sees ImGui headers (even in dumb IDE modes)
target_include_directories(minisynth PRIVATE
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

# PortAudio linkage
if (PortAudio_FOUND)
  target_link_libraries(minisynth PRIVATE PortAudio::PortAudio)
else()
  target_include_directories(minisynth PRIVATE ${PORTAUDIO_INCLUDE_DIRS})
  target_link_directories(minisynth PRIVATE ${PORTAUDIO_LIBRARY_DIRS})
  target_link_libraries(minisynth PRIVATE ${PORTAUDIO_LIBRARIES})
endif()

# ImGui (already pulls glfw + OpenGL)
target_link_libraries(minisynth PRIVATE imgui_lib)
