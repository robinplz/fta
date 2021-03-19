cmake_minimum_required(VERSION 3.15)
project(runner LANGUAGES CXX)

get_filename_component(TOPDIR ${CMAKE_SOURCE_DIR} DIRECTORY)
set(APP_CORE_DIR "${TOPDIR}/app-core")
add_subdirectory(${APP_CORE_DIR} app_core_lib)

# the sub-directory of "app_core_lib' 
# set(APP_CORE_LIBRARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/app_core_lib")
# Published to parent scope for install step.
# set(APP_CORE_LIBRARY_DIR ${APP_CORE_LIBRARY_DIR} PARENT_SCOPE)

add_executable(${BINARY_NAME} WIN32
  "flutter_window.cpp"
  "gl_window.cpp"
  "main.cpp"
  "run_loop.cpp"
  "utils.cpp"
  "win32_window.cpp"
  "${FLUTTER_MANAGED_DIR}/generated_plugin_registrant.cc"
  "Runner.rc"
  "runner.exe.manifest"
)
apply_standard_settings(${BINARY_NAME})
target_compile_definitions(${BINARY_NAME} PRIVATE "NOMINMAX")
target_link_libraries(${BINARY_NAME} PRIVATE flutter flutter_wrapper_app Dwmapi opengl32 app_core)
target_include_directories(${BINARY_NAME} PRIVATE "${CMAKE_SOURCE_DIR}")
add_dependencies(${BINARY_NAME} flutter_assemble)
