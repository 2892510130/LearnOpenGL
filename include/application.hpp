#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "config.hpp"
#include "debug_ui.hpp"

class Application
{

public:

    Application();
    ~Application();

    void run();

private:

    void init_imgui();
    void init_opengl();

    void shutdown();

    void new_frame();
    void render();

    static void error_callback(
        int error,
        const char* description
    );

    static void frame_buffer_size_callback(
        GLFWwindow* window,
        int width,
        int height
    );

private:

    GLFWwindow* m_Window = nullptr;
    DebugUI m_debug_ui;
};