#include "application.hpp"
#include "GLFW/glfw3.h"

Application::Application()
{
    init_opengl();
    init_imgui();
}

Application::~Application()
{
    shutdown();
}

void Application::init_opengl()
{
    glfwSetErrorCallback(error_callback);

    if(!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set as core profile

    m_Window = glfwCreateWindow(
        Config::WindowWidth,
        Config::WindowHeight,
        Config::WindowTitle,
        nullptr,
        nullptr
    );

    if(!m_Window)
        throw std::runtime_error("Failed creating window");

    glfwSetWindowUserPointer(m_Window, this);

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    glfwSetFramebufferSizeCallback(m_Window, frame_buffer_size_callback);
    glfwSetCursorPosCallback(m_Window, mouse_callback);
    glfwSetScrollCallback(m_Window, scroll_callback);

    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwSwapInterval(1); // vsync
}

void Application::init_imgui()
{
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard |
        ImGuiConfigFlags_NavEnableGamepad |
        ImGuiConfigFlags_DockingEnable |
        ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(
        m_Window,
        true
    );

    ImGui_ImplOpenGL3_Init(
        Config::GLSLVersion
    );

    io.Fonts->AddFontFromFileTTF(
        Config::CodeNewRomanFontPath,
        16.0f
    );

    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;

    io.Fonts->AddFontFromFileTTF(
        Config::ChineseFontPath,
        16.0f, 
        &config, 
        io.Fonts->GetGlyphRangesChineseSimplifiedCommon()
    );

    io.Fonts->Build();
}

void Application::run()
{
    while(!glfwWindowShouldClose(m_Window))
    {
        float currentTime = static_cast<float>(glfwGetTime());
        m_delta_time = currentTime - m_last_time;
        m_last_time = currentTime;
        glfwPollEvents();
        
        new_frame();
        process_input(m_Window);

        // Do we need to init it when we switch it even if we do not do the clean up? Maybe reload it is better way?
        m_scene_manager.update(m_delta_time, m_camera);
        
        m_debug_ui.RenderUI(m_scene_manager);

        render();
    }
}

void Application::render()
{
    int width;
    int height;

    glfwGetFramebufferSize(
        m_Window,
        &width,
        &height
    );

    glViewport(
        0,
        0,
        width,
        height
    );

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* We can add the render code here, in the main window */
    m_scene_manager.render();

    // Start render the imui
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(
        ImGui::GetDrawData()
    );

    ImGuiIO& io = ImGui::GetIO();
    if(io.ConfigFlags &
       ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(
            backup
        );
    }

    glfwSwapBuffers(m_Window);
}

void Application::new_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Application::shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if(m_Window)
        glfwDestroyWindow(m_Window);

    glfwTerminate();
}

Application* Application::get_app(GLFWwindow* window)
{
    return static_cast<Application*>(glfwGetWindowUserPointer(window));
}

void Application::error_callback(
    int error,
    const char* description)
{
    fprintf(
        stderr,
        "GLFW Error %d: %s\n",
        error,
        description
    );
}

void Application::frame_buffer_size_callback(
    GLFWwindow*,
    int width,
    int height)
{
    glViewport(
        0,
        0,
        width,
        height
    );
}

void Application::toggle_cursor_mode()
{
    if (m_cursorDisabled)
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_cursorDisabled = false;
        m_firstMouse = true;
    }
    else
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_cursorDisabled = true;
        m_firstMouse = true;
    }
}

void Application::process_input(GLFWwindow* window)
{
    auto esc_pressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    if (esc_pressed) 
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !m_tabKeyPressed)
    {
        toggle_cursor_mode();
        m_tabKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
    {
        m_tabKeyPressed = false;
    }
    
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureKeyboard)
    {
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        m_mixValue += 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if(m_mixValue >= 1.0f)
            m_mixValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        m_mixValue -= 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (m_mixValue <= 0.0f)
            m_mixValue = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.ProcessKeyboard(FORWARD, m_delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.ProcessKeyboard(BACKWARD, m_delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.ProcessKeyboard(LEFT, m_delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.ProcessKeyboard(RIGHT, m_delta_time);
}

void Application::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    Application* app = get_app(window);
    if (app)
    {
        app->handle_mouse_move(xposIn, yposIn);
    }
}

void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;

    Application* app = get_app(window);
    if (app)
    {
        app->handle_scroll(xoffset, yoffset);
    }
}

void Application::handle_mouse_move(double xposIn, double yposIn)
{
     if (!m_cursorDisabled)
        return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_firstMouse)
    {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos;
    m_lastX = xpos;
    m_lastY = ypos;

    m_camera.ProcessMouseMovement(xoffset, yoffset);
}

void Application::handle_scroll(double xoffset, double yoffset)
{
    m_camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
