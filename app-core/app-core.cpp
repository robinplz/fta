#include "app-core-api.h"
#include "app-core-native-api.h"

// global vars
PointerPoint _p = {0};
std::function<void()> _onUpdate = nullptr;

// app-core-api
void sendPointerEvent(float x, float y) {
    _p.x = x;
    _p.y = y;
    
    if (_onUpdate) {
        _onUpdate();
    }
}

// app-core-native-api
void setCallback(std::function<void()> onUpdate) {
    _onUpdate = onUpdate;
}

PointerPoint getPointerPoint() {
    return _p;
}