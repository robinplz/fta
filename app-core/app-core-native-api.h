#ifndef _APP_CORE_NATIVE_API_H_
#define _APP_CORE_NATIVE_API_H_

#include "types.h"
#include <functional>

void setCallback(std::function<void()> onUpdate);
PointerPoint getPointerPoint();

#endif