cmake_minimum_required(VERSION 3.26)

project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(vendor/GLAD)
add_subdirectory(vendor/stb)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/assimp)

add_definitions(-DGLEW_STATIC)

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

    target_link_directories(${TARGET_NAME} PRIVATE 
        vendor/GLFW/lib-mingw-w64
    )

    target_link_libraries(${TARGET_NAME} PRIVATE 
        GLAD
        glfw3
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