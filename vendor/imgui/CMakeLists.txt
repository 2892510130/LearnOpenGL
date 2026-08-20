project(ImGuiLibrary)

set(LIBRARY_SOURCES
    imgui.cpp 
    imgui_demo.cpp 
    imgui_tables.cpp 
    imgui_widgets.cpp 
    imgui_draw.cpp
    backends/imgui_impl_glfw.cpp 
    backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${LIBRARY_SOURCES})

target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp
)

target_link_libraries(imgui PUBLIC
    glfw
    GLAD
    gdi32
    Opengl32
)