cmake_minimum_required(VERSION 3.26)

project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# file(GLOB_RECURSE SOURCE_FILES 
#      "src/*.cpp"
# )

# list(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/vender/some_library/unwanted.cpp)

add_subdirectory(vendor/GLAD)
add_subdirectory(vendor/stb)
add_subdirectory(vendor/glm)

add_executable(intro 
    src/main.cpp
)

add_definitions(-DGLEW_STATIC)
target_compile_definitions(intro PRIVATE
    ROOT_PATH="${CMAKE_SOURCE_DIR}"
)

target_include_directories(intro PUBLIC include)
target_include_directories(intro PRIVATE 
    vendor/GLFW/include
    vendor/stb
    vendor/glm
)
target_link_directories(intro PRIVATE vendor/GLFW/lib-mingw-w64)
target_link_libraries(intro PRIVATE 
    GLAD
    glfw3
    stb
    glm
    gdi32
    Opengl32
)
