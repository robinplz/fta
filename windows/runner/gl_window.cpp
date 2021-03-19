#include "gl_window.h"

#include <windows.h>			/* must include this before GL/gl.h */
#include <GL/gl.h>

#include "../../app-core/app-core-native-api.h"

HGLRC hRC;
UINT_PTR GL_ROTATE_TIMER = 100;
UINT_PTR timerId;

VOID OnTimer(HWND hWnd, UINT msg, UINT_PTR timer, DWORD tick) {
    if (timer == GL_ROTATE_TIMER) {
        glRotatef(1.0f, 0.0f, 0.0f, 1.0f);
        InvalidateRect(hWnd, nullptr, false);
    }
}

HWND targetWindow = nullptr;
float vertices[2] = {0.0f, 1.0f};

void onUpdate() {
    if (!targetWindow) {
        return;
    }

    UINT dpiRaw = GetDpiForWindow(targetWindow);
    const float DPI = float(dpiRaw) / 96.0f;

    auto p = getPointerPoint();
    float x = p.x;
    float y = p.y;
    RECT clientRect;
    GetClientRect(targetWindow, &clientRect);
    float w = float(clientRect.right - clientRect.left) / DPI;
    float h = float(clientRect.bottom - clientRect.top) / DPI;
    y = h - y; // y-direction flipped
    float halfW = w * 0.5f;
    float halfH = h * 0.5f;
    x -= halfW;
    y -= halfH;
    float coordX = x / halfW;
    float coordY = y / halfH;
    vertices[0] = coordX;
    vertices[1] = coordY;

    PostMessage(targetWindow, WM_PAINT, 0, 0);
    // InvalidateRect(targetWindow, nullptr, false);
}

void display(HDC hDC)
{
    /* rotate a triangle around */
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	
    glBegin(GL_TRIANGLES);
    glIndexi(1);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(vertices[0], vertices[1]);
    glIndexi(2);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(-1, -1);
    glIndexi(3);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2i(1, -1);
    glEnd();
    glFlush();
    SwapBuffers(hDC);			/* nop if singlebuffered */
}

GLWindow::GLWindow(RunLoop* run_loop, const flutter::DartProject& project)
:FlutterWindow(run_loop, project) {

}

bool GLWindow::OnCreate() {
    if (!FlutterWindow::OnCreate()) {
        return false;
    }

    // init GL Context
    HDC hDC = GetDC(GetHandle());

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cColorBits   = 32;
    int pf = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pf, &pfd);
    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    ReleaseDC(GetHandle(), hDC);

    // timerId = SetTimer(GetHandle(), GL_ROTATE_TIMER, 16, OnTimer);

    targetWindow = GetHandle();
    setCallback(onUpdate);

    return true;
}

void GLWindow::OnDestroy() {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    // KillTimer(GetHandle(), timerId);

    FlutterWindow::OnDestroy();
}

void GLWindow::OnSize(int w, int h) {
    FlutterWindow::OnSize(w, h);
    glViewport(0, 0, w, h);
}

void GLWindow::OnPaint() {
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(GetHandle(), &ps);
    display(hDC);
    EndPaint(GetHandle(), &ps);
}