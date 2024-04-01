#include "Window.hpp"

#include <SDL3/SDL.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <glad/glad.h>

#include "Core/DPIHandler.hpp"
#include "Core/Debug/Instrumentor.hpp"
#include "Core/Log.hpp"

namespace App {

Window::Window(const Settings& settings)
    : m_settings(DPIHandler::get_dpi_aware_window_size(settings)) {
  APP_PROFILE_FUNCTION();

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  m_window = SDL_CreateWindow(settings.title.c_str(),
      m_settings.width,
      m_settings.height,
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

  // NOLINTNEXTLINE
  m_gl_context = SDL_GL_CreateContext(m_window);
  if (m_gl_context == nullptr) {
    APP_ERROR("Could not create SDL OpenGL context.");
    return;
  }

  SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress));
  SDL_GL_MakeCurrent(m_window, m_gl_context);
  SDL_GL_SetSwapInterval(1);  // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io{ImGui::GetIO()};

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable |
                    ImGuiConfigFlags_ViewportsEnable;

  // @info: https://github.com/ocornut/imgui/issues/2361
  io.ConfigDockingTransparentPayload = true;

  // Absolute imgui.ini path to preserve settings independent of app location.
  static const std::string imgui_ini_filename{m_user_config_path.generic_string() + "imgui.ini"};
  io.IniFilename = imgui_ini_filename.c_str();

  // ImGUI font
  const float font_scaling_factor{SDL_GetWindowDisplayScale(m_window)};
  const float font_size{18.0F * font_scaling_factor};
  const std::string font_path{Resources::font_path("Manrope.ttf").generic_string()};

  io.Fonts->AddFontFromFileTTF(font_path.c_str(), font_size);
  io.FontDefault = io.Fonts->AddFontFromFileTTF(font_path.c_str(), font_size);
  io.FontGlobalScale = 1.0F / font_scaling_factor;

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForOpenGL(m_window, m_gl_context);
  ImGui_ImplOpenGL3_Init("#version 410 core");
}

Window::~Window() {
  APP_PROFILE_FUNCTION();

  SDL_GL_DeleteContext(m_gl_context);
  SDL_DestroyWindow(m_window);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}

void Window::update() {
  APP_PROFILE_FUNCTION();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  const ImGuiIO& io{ImGui::GetIO()};

  if (!m_minimized) {
    ImGui::DockSpaceOverViewport();

    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Exit", "Cmd+Q")) {
          on_close();
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View")) {
        ImGui::MenuItem("Some Panel", nullptr, &m_show_some_panel);
        ImGui::MenuItem("ImGui Demo Panel", nullptr, &m_show_demo_panel);
        ImGui::MenuItem("Debug Panel", nullptr, &m_show_debug_panel);
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }

    // Whatever GUI to implement here ...
    if (m_show_some_panel) {
      ImGui::Begin("Some panel", &m_show_some_panel);
      ImGui::Text("Hello World");
      ImGui::End();
    }

    // ImGUI demo panel
    if (m_show_demo_panel) {
      ImGui::ShowDemoWindow(&m_show_demo_panel);
    }

    // Debug panel
    if (m_show_debug_panel) {
      ImGui::Begin("Debug panel", &m_show_debug_panel);
      ImGui::Text("User config path: %s", m_user_config_path.c_str());
      ImGui::Text("Global font scaling %f", io.FontGlobalScale);
      ImGui::End();
    }
  }

  // Rendering
  ImGui::Render();

  glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
  glClearColor(0.5F, 0.5F, 0.5F, 1.00F);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0) {
    SDL_Window* backup_current_window{SDL_GL_GetCurrentWindow()};
    SDL_GLContext backup_current_context{SDL_GL_GetCurrentContext()};
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
  }

  SDL_GL_SwapWindow(m_window);
}

void Window::on_minimize() {
  APP_PROFILE_FUNCTION();

  m_minimized = true;
}

void Window::on_shown() {
  APP_PROFILE_FUNCTION();

  m_minimized = false;
}

void Window::on_close() {
  APP_PROFILE_FUNCTION();

  SDL_Event window_close_event;
  window_close_event.type = SDL_EVENT_QUIT;
  SDL_PushEvent(&window_close_event);
}

void Window::on_event(const SDL_WindowEvent& event) {
  APP_PROFILE_FUNCTION();

  switch (event.type) {
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
      return on_close();
    case SDL_EVENT_WINDOW_MINIMIZED:
      return on_minimize();
    case SDL_EVENT_WINDOW_SHOWN:
      return on_shown();
    default:
      // Do nothing otherwise
      return;
  }
}

SDL_Window* Window::get_native_window() const {
  APP_PROFILE_FUNCTION();

  return m_window;
}

}  // namespace App
