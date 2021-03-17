#include <flutter/dart_project.h>
#include <flutter/flutter_view_controller.h>
#include <windows.h>

#include "gl_window.h"
#include "run_loop.h"
#include "utils.h"

int APIENTRY wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev,
                      _In_ wchar_t *command_line, _In_ int show_command) {
  // Attach to console when present (e.g., 'flutter run') or create a
  // new console when running with a debugger.
  if (!::AttachConsole(ATTACH_PARENT_PROCESS) && ::IsDebuggerPresent()) {
    CreateAndAttachConsole();
  }

  // Initialize COM, so that it is available for use in the library and/or
  // plugins.
  ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  RunLoop run_loop;

  flutter::DartProject project(L"data");

  std::vector<std::string> command_line_arguments =
      GetCommandLineArguments();

  project.set_dart_entrypoint_arguments(std::move(command_line_arguments));

  Win32Window::Point origin(10, 10);
  Win32Window::Size size(1280, 720);

  GLWindow flutterWindow(&run_loop, project);
  if (!flutterWindow.CreateAndShow(L"fta", origin, size)) {
    return EXIT_FAILURE;
  }
  flutterWindow.SetQuitOnClose(true);

  run_loop.Run();

  ::CoUninitialize();
  return EXIT_SUCCESS;
}
