cmake_minimum_required (VERSION 3.20)
project (scrollbar CXX)

add_library (${PROJECT_NAME} SHARED
    plug.cpp
    ../libs/GUI/src/scrollbar.cpp
    ../libs/GraphLib/src/graphics_sfml.cpp
)

target_include_directories (${PROJECT_NAME}
    PRIVATE
        ../libs
        ../libs/GUI/include
        ../libs/GraphLib/include
        ../libs/geometry/include
        ../libs/MyLib
        ../libs/MyLib/Logger
        ../libs/MyLib/Assert
        ../libs/MyLib/My_stdio
)

find_package(OpenMP REQUIRED)
target_link_libraries (${PROJECT_NAME}
    PRIVATE
        sfml-system
        sfml-window
        sfml-graphics

        OpenMP::OpenMP_CXX
)

target_compile_features (${PROJECT_NAME}
    PRIVATE
        cxx_std_17
)

target_compile_options (${PROJECT_NAME}
    PRIVATE
        -fdiagnostics-color=always

        -Wall
        -Wextra
        -fstack-protector-strong
        -fcheck-new
        -fstrict-overflow

        -march=native
        $<$<CONFIG:Debug>:
            -Og
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            # -flto=auto
            -DNDEBUG
        >
)

target_link_options (${PROJECT_NAME}
    PRIVATE
        -march=native
        -Wl,-q
        $<$<CONFIG:Debug>:
            -Og
            -g3
            -DDEBUG
            -ggdb
            -fsanitize=address,leak,undefined
        >

        $<$<CONFIG:Release>:
            -O2
            -flto=auto
        >
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

# cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++
# cmake --build build
