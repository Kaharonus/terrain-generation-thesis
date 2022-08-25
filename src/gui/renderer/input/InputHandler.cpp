#include <iostream>
#include <renderer/input/InputHandler.h>
#include <tuple>

using namespace MapGenerator::Renderer;

InputHandler::InputHandler() {

    for (int i = 0; i < 512; i++) {
        keys[i] = false;
    }

    for (int i = 0; i < 3; i++) {
        mouseButtons[i] = false;
    }
}

InputHandler::~InputHandler() {
}

void InputHandler::mouseMove(int x, int y) {
    if (absoluteX == 0 && absoluteY == 0) {
        absoluteX = x;
        absoluteY = y;
        return;
    }
    deltaX = x - absoluteX;
    deltaY = y - absoluteY;
    absoluteX = x;
    absoluteY = y;
    // std::cout << "Mouse moved to " << x << " " << y << std::endl;
    // TODO implement
}

void InputHandler::mousePress(MouseButton button, int x, int y) {
    mouseButtons[button] = true;
    this->absoluteX = x;
    this->absoluteY = y;
}

void InputHandler::mouseRelease(MouseButton button, int x, int y) {
    mouseButtons[button] = false;
    this->absoluteX = x;
    this->absoluteY = y;
}

void InputHandler::mouseWheel(int wheel, int direction, int x, int y) {
    // todo implement
}

void InputHandler::keyPress(Key key) {
    keys[key] = true;
}

void InputHandler::keyRelease(Key key) {
    keys[key] = false;
}

bool InputHandler::isKeyPressed(Key key) {
    return keys[key];
}

bool InputHandler::isKeyUp(Key key) {
    return !keys[key];
}

bool InputHandler::isMouseButtonPressed(MouseButton button) {
    return mouseButtons[button];
}

bool InputHandler::isMouseButtonUp(MouseButton button) {
    return !mouseButtons[button];
}

std::tuple<int, int> InputHandler::getPositionDelta() {
    return std::make_tuple(deltaX, deltaY);
}

std::tuple<int, int> InputHandler::getAbsolutePosition() {
    return std::make_tuple(absoluteX, absoluteY);
}

InputHandler::Key InputHandler::fromQtKey(int key) {
    if (key >= KEY_SPACE && key <= KEY_RIGHT_BRACKET) {
        return static_cast<Key>(key);
    }
    switch (key) {
    case 0x01000000:
        return KEY_ESCAPE;
    case 0x01000005:
        return KEY_ENTER;
    case 0x01000006:
        return KEY_INSERT;
    case 0x01000020:
        return KEY_SHIFT;
    case 0x01000021:
        return KEY_CONTROL;

    }

    return KEY_UNKNOWN;
}
