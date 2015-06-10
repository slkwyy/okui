#include "onair/okui/Window.h"

#include "onair/okui/Application.h"
#include "onair/okui/Platform.h"

namespace onair {
namespace okui {

Window::Window(Application* application) : _application(application) {
    _contentView._window = this;
}

Window::~Window() {
    close();
}

void Window::open() {
    application()->platform()->openWindow(this, _title.c_str(), _x, _y, _width, _height);
    _updateContentLayout();
}

void Window::close() {
    closing();
    application()->platform()->closeWindow(this);
}

void Window::setPosition(int x, int y) {
    _x = x;
    _y = y;
    application()->platform()->setWindowPosition(this, x, y);
}

void Window::setSize(int width, int height) {
    _width = width;
    _height = height;
    application()->platform()->setWindowSize(this, width, height);
}

void Window::setTitle(const char* title) {
    _title = title;
    application()->platform()->setWindowTitle(this, title);
}

void Window::setMenu(const Menu& menu) {
    _menu = menu;
    application()->platform()->setWindowMenu(this, menu);
}

std::shared_ptr<Texture> Window::loadTextureResource(const char* name) {
    auto hashable = std::string("resource:") + name;

    if (auto hit = _textureCache.get(hashable)) {
        return hit;
    }

    auto resource = application()->loadResource(name);
    if (!resource) {
        return nullptr;
    }
    auto ret = _textureCache.add(Texture(resource), hashable);
    _texturesToLoad.insert(ret);
    return ret;
}

std::shared_ptr<Texture> Window::loadTextureFromMemory(std::shared_ptr<const std::string> data) {
    auto hashable = std::string("memory:") + std::to_string(reinterpret_cast<uintptr_t>(data->data())) + ":" + std::to_string(data->size());

    if (auto hit = _textureCache.get(hashable)) {
        return hit;
    }

    auto ret = _textureCache.add(Texture(data), hashable);
    _texturesToLoad.insert(ret);
    return ret;
}

std::shared_ptr<BitmapFont> Window::loadBitmapFontResource(const char* textureName, const char* metadataName) {
    auto hashable = std::string("resource:") + textureName + "$!@#" + metadataName;

    if (auto hit = _bitmapFontCache.get(hashable)) {
        return hit;
    }

    auto texture = loadTextureResource(textureName);
    if (!texture) {
        return nullptr;
    }
    auto metadata = application()->loadResource(metadataName);
    return _bitmapFontCache.add(BitmapFont(texture, *metadata), hashable);
}

void Window::setFocus(View* focus) {
    if (_focus == focus) { return; }
    if (_focus) {
        _focus->focusLost();
    }
    _focus = focus;
    if (_focus) {
        _focus->focusGained();
    }
}

Point<int> Window::windowToView(View* view, int x, int y) {
    if (!view) {
        return Point<int>(x, y);
    }
    auto superPoint = windowToView(view->superview(), x, y);
    return view->superviewToLocal(superPoint.x, superPoint.y);
}

void Window::beginDragging(View* view) {
    _draggedViews.insert(view);
}

void Window::endDragging(View* view) {
    _draggedViews.erase(view);
}

void Window::dispatchMouseDown(MouseButton button, int x, int y) {
    _contentView.dispatchMouseDown(button, x, y);
    for (auto& observer : _draggedViews) {
        auto point = windowToView(observer, x, y);
        observer->mouseDrag(point.x, point.y);
    }
}

void Window::dispatchMouseUp(MouseButton button, int x, int y) {
    _contentView.dispatchMouseUp(button, x, y);
    for (auto& observer : _draggedViews) {
        auto point = windowToView(observer, x, y);
        observer->mouseDrag(point.x, point.y);
    }
    _draggedViews.clear();
}

void Window::dispatchMouseMovement(int x, int y) {
    _contentView.dispatchMouseMovement(x, y);
    for (auto& observer : _draggedViews) {
        auto point = windowToView(observer, x, y);
        observer->mouseDrag(point.x, point.y);
    }
}

void Window::dispatchMouseWheel(int xPos, int yPos, int xWheel, int yWheel) {
    _contentView.dispatchMouseWheel(xPos, yPos, xWheel, yWheel);
}

Responder* Window::nextResponder() {
    return application();
}

void Window::keyDown(KeyCode key, KeyModifiers mod, bool repeat) {
    if (key == KeyCode::kTab && _initialFocus && !_focus) {
        if (mod & KeyModifier::kShift) {
            if (auto view = _initialFocus->previousAvailableFocus()) {
                view->focus();
                return;
            }
        }
        if (_initialFocus->isVisible() && _initialFocus->canBecomeFocus()) {
            _initialFocus->focus();
            return;
        } else if (auto view = _initialFocus->nextAvailableFocus()) {
            view->focus();
            return;
        }
    }
    
    Responder::keyDown(key, mod, repeat);
}

void Window::ensureTextures() {
    for (auto& texture : _texturesToLoad) {
        texture->load(&_openGLTextureCache);
    }
    _texturesToLoad.clear();
}

void Window::_render() {
    ensureTextures();

    render();

    Rectangle<int> area(0, 0, _contentView.bounds().width * _renderScale, _contentView.bounds().height * _renderScale);
    RenderTarget target(area.width, area.height);
    _contentView.renderAndRenderSubviews(&target, area);
}

void Window::_didResize(int width, int height) {
    _width = width;
    _height = height;
    _updateContentLayout();
}

void Window::_updateContentLayout() {
    int w, h;
    application()->platform()->getWindowRenderSize(this, &w, &h);
    _renderScale = (double)w / _width;
    _contentView.setBounds(0, 0, _width, _height);
    layout();
}

}}