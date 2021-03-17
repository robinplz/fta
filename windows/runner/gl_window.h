#ifndef _GL_WINDOW_H_
#define _GL_WINDOW_H_

#include "flutter_window.h"

class GLWindow: public FlutterWindow {
public:
    GLWindow(RunLoop* run_loop, const flutter::DartProject& project);

protected:
    bool OnCreate() override;
    void OnDestroy() override;
    void OnPaint() override;
    void OnSize(int w, int h) override;
};

#endif