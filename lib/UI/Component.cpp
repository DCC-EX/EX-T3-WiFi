#include <Component.h>

void Component::redraw() { }

bool Component::touch(TOUCHINFO& ti) { return false; }

bool Component::release(TOUCHINFO& ti) { return false; }

bool Component::encoderRotate(Encoder::Rotation rotation) { return false; }

bool Component::encoderPress(Encoder::ButtonPress press) { return false; }

bool Component::swipe(Swipe swipe) { return false; }
