#pragma once

#include "KeyCodes.h"

namespace vertex {

class KeyEvent {
public:
    enum class KeyAction { UNDEFINED, PRESS, REPEAT, RELEASE };
    KeyEvent(Key keyCode, KeyAction);
    Key getKeyCode() const;
    KeyAction getAction() const;
    void setKeyCode(Key newKeyCode);
    void setAction(KeyAction newAction);

private:
    Key mKeyCode{Key::Unknown};
    KeyAction mAction{KeyAction::UNDEFINED};
};
}
