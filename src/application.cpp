#include "application.hpp"

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

    m_Window = glfwCreateWindow(
        Config::WindowWidth,
        Config::WindowHeight,
        Config::WindowTitle,
        nullptr,
        nullptr
    );

    if(!m_Window)
        throw std::runtime_error("Failed creating window");

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    glfwSetFramebufferSizeCallback(
        m_Window,
        frame_buffer_size_callback
    );

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
        glfwPollEvents();
        new_frame();
        m_debug_ui.RenderUI();
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

    glClearColor(
        0.45f,
        0.55f,
        0.60f,
        1.0f
    );

    glClear(GL_COLOR_BUFFER_BIT);

    /* We can add the render code here, in the main window */

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