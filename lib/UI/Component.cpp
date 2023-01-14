#include <Component.h>

void Component::redraw() { }

bool Component::touch(uint8_t count, GTPoint* points) { return false; }

bool Component::release(uint8_t count, GTPoint* points) { return false; }

bool Component::encoderRotate(Encoder::Rotation rotation) { return false; }

bool Component::encoderPress(Encoder::ButtonPress press) { return false; }

bool Component::swipe(Swipe swipe) { return false; }
