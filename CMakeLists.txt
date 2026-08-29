cmake_minimum_required(VERSION 3.26)

project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/stb)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/imgui)

function(add_exes TARGET_NAME SOURCE_FILE)

    add_executable(${TARGET_NAME} 
        ${SOURCE_FILE}
    )

    target_compile_definitions(${TARGET_NAME} PRIVATE
        ROOT_PATH="${CMAKE_SOURCE_DIR}"
    )

    target_include_directories(${TARGET_NAME} PUBLIC 
        include
    )

    target_include_directories(${TARGET_NAME} PRIVATE 
        vendor/GLFW/include
        vendor/stb
        vendor/glm
        vendor/assimp/include
    )

    target_link_libraries(${TARGET_NAME} PRIVATE 
        GLAD
        glfw
        stb
        glm
        assimp
        gdi32
        Opengl32
    )

endfunction()

add_exes(intro src/intro.cpp)
add_exes(advance src/advance.cpp)
add_exes(light_advanced src/light_advanced.cpp)

# ImGui supported exe

add_executable(imguiGraphicTest
    src/imgui_graphic.cpp
    src/application.cpp
    src/debug_ui.cpp
    src/texture.cpp
    src/scene/BasicScene.cpp
    src/scene/ShadowScene.cpp
    src/scene/PointShadowScene.cpp
)

target_link_libraries(imguiGraphicTest
    imgui
    stb
    glm
    assimp
)

target_include_directories(imguiGraphicTest PRIVATE
    ./include
    vendor/stb
    vendor/glm
    vendor/assimp/include
)
