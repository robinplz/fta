#include "gl_window.h"

#include <windows.h>			/* must include this before GL/gl.h */
#include <GL/gl.h>

HGLRC hRC;

void display(HDC hDC)
{
    /* rotate a triangle around */
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	glRotatef(1.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glIndexi(1);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(0,  1);
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

    return true;
}

void GLWindow::OnDestroy() {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);

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