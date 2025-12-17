cmake_minimum_required (VERSION 3.20)
project (physics CXX)

add_subdirectory(libs/mipt-ded-zemax)

add_executable (${PROJECT_NAME}
    src/main.cpp
    src/raytracing_spheres.cpp
    src/plane.cpp
    src/triangles_set.cpp
    src/movement.cpp
    src/info_table.cpp
    src/color_picker.cpp
    src/toolbar.cpp
    src/plugin_upload.cpp
    src/circle.cpp

    libs/GUI/src/draw.cpp
    libs/GUI/src/scrollbar.cpp
    libs/geometry/src/vector.cpp
    libs/geometry/src/matrix.cpp
    libs/MyLib/Logger/logging.cpp
    libs/MyLib/Assert/print_error.cpp
)

target_include_directories (${PROJECT_NAME}
    PRIVATE
        libs
        libs/GUI/include
        libs/GUI/graphics
        libs/geometry/include
        libs/MyLib
        libs/MyLib/Logger
        libs/MyLib/Assert
        libs/MyLib/My_stdio
        libs/mipt-ded-zemax/include
)

target_link_libraries (${PROJECT_NAME}
    PRIVATE
        dl

        cum
        hui
)

target_link_directories(${PROJECT_NAME}
    PRIVATE
        ./libs/mipt-ded-zemax/build
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

        -ldl

        -rdynamic

        -Wl,-export-dynamic

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
        -export-dynamic
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
