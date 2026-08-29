#pragma once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "config.hpp"
#include "debug_ui.hpp"
#include "scene/SceneManager.hpp"
#include "camera.hpp"

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

    void process_input(GLFWwindow* window);
    
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    static Application* get_app(GLFWwindow* window);

    void handle_mouse_move(double xposIn, double yposIn);
    void handle_scroll(double xoffset, double yoffset);
    void toggle_cursor_mode();

private:

    GLFWwindow* m_Window = nullptr;
    DebugUI m_debug_ui;
    SceneManager m_scene_manager;
    Camera m_camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    float m_mixValue = 0.2f;
    float m_last_time = 0.0f, m_delta_time = 0.0f;
    float m_lastX =  800.0f / 2.0, m_lastY = 600.0f / 2.0;
    bool m_firstMouse = true;
    bool m_cursorDisabled = true;
    bool m_tabKeyPressed = false;
};
