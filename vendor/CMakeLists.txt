include(FetchContent)

# Dependencies

FetchContent_Declare(
  doctest
  GIT_REPOSITORY "https://github.com/onqtam/doctest.git"
  GIT_TAG v2.5.2
)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
  GIT_TAG 12.1.0
)

FetchContent_Declare(
  glad
  GIT_REPOSITORY "https://github.com/krieselreihe/glad.git"
  GIT_TAG v1.0.0
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  GIT_TAG v1.92.8-docking
)

FetchContent_Declare(
  SDL3
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
  GIT_TAG release-3.4.8
)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG v1.17.0
)

# Settings

# For CMake to be able to override options for older packages.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(DOCTEST_NO_INSTALL ON)
set(FMT_INSTALL OFF)
set(SPDLOG_FMT_EXTERNAL ON)

add_subdirectory(imgui-setup)

# Populate

FetchContent_MakeAvailable(doctest fmt glad imgui SDL3 spdlog)
