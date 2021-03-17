#include "flutter_window.h"
#include "dwmapi.h"

#include <optional>

#include "flutter/generated_plugin_registrant.h"

FlutterWindow::FlutterWindow(RunLoop* run_loop,
                             const flutter::DartProject& project)
    : run_loop_(run_loop), project_(project) {}

FlutterWindow::~FlutterWindow() {}

bool FlutterWindow::OnCreate() {
  if (!Win32Window::OnCreate()) {
    return false;
  }

  RECT frame = GetClientArea();

  // The size here must match the window dimensions to avoid unnecessary surface
  // creation / destruction in the startup path.
  flutter_controller_ = std::make_unique<flutter::FlutterViewController>(
      frame.right - frame.left, frame.bottom - frame.top, project_);
  // Ensure that basic setup of the controller was successful.
  if (!flutter_controller_->engine() || !flutter_controller_->view()) {
    return false;
  }
  RegisterPlugins(flutter_controller_->engine());
  run_loop_->RegisterFlutterInstance(flutter_controller_->engine());

  // we'll make the content window a 'detached' one.
  HWND contentWindow = flutter_controller_->view()->GetNativeWindow();

  // remove WS_CHILD style
  LONG style = GetWindowLong(contentWindow, GWL_STYLE);
  style &= (~WS_CHILD);
  SetWindowLong(contentWindow, GWL_STYLE, style);

  // set the content window as a logical 'child' of main window.
  SetChildContent(contentWindow);
  SetParent(contentWindow, nullptr);

  // then make main window the 'owner' of the content window.
  SetWindowLongPtr(contentWindow, GWLP_HWNDPARENT, (LONG_PTR)GetHandle());
  SetWindowPos(contentWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

  // make windows positions aligned.
  AlignChildWindow();

  // then make content window background 'transparent' =>
  // Negative margins have special meaning to DwmExtendFrameIntoClientArea.
  // Negative margins create the "sheet of glass" effect, where the client area
  // is rendered as a solid surface with no window border.
  MARGINS margins = {-1};
  DwmExtendFrameIntoClientArea(contentWindow, &margins);

  return true;
}

void FlutterWindow::OnDestroy() {
  if (flutter_controller_) {
    run_loop_->UnregisterFlutterInstance(flutter_controller_->engine());
    flutter_controller_ = nullptr;
  }

  Win32Window::OnDestroy();
}

LRESULT
FlutterWindow::MessageHandler(HWND hwnd, UINT const message,
                              WPARAM const wparam,
                              LPARAM const lparam) noexcept {
  // Give Flutter, including plugins, an opporutunity to handle window messages.
  if (flutter_controller_) {
    std::optional<LRESULT> result =
        flutter_controller_->HandleTopLevelWindowProc(hwnd, message, wparam,
                                                      lparam);
    if (result) {
      return *result;
    }
  }

  switch (message) {
    case WM_FONTCHANGE:
      flutter_controller_->engine()->ReloadSystemFonts();
      break;
  }

  return Win32Window::MessageHandler(hwnd, message, wparam, lparam);
}
