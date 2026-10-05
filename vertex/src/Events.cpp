#include "vertex/Events.h"

namespace vertex {
KeyEvent::KeyEvent(Key keyCode, KeyAction action) : mKeyCode(keyCode), mAction(action) {}

Key KeyEvent::getKeyCode() const {
    return mKeyCode;
}

KeyEvent::KeyAction KeyEvent::getAction() const {
    return mAction;
}

void KeyEvent::setKeyCode(Key newKeyCode) {
    mKeyCode = newKeyCode;
}

void KeyEvent::setAction(KeyAction newAction) {
    mAction = newAction;
}
}
