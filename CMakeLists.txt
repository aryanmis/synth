cmake_minimum_required(VERSION 3.16)
project(synth LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(synth
    main.cpp
    SynthVoice.cpp
)

target_include_directories(synth PRIVATE /opt/homebrew/include)
target_link_directories(synth PRIVATE /opt/homebrew/lib)
target_link_libraries(synth PRIVATE portaudio)
