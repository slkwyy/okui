/**
* Copyright 2017 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <okui/config.h>

#include <okui/Command.h>
#include <okui/KeyCode.h>

#include <string>
#include <vector>

namespace okui {

class MenuItem;

class Menu {
public:
    Menu() {}
    Menu(const std::vector<MenuItem>& items) : _items(items) {}

    /**
    * Returns the top level items of the menu bar.
    */
    const std::vector<MenuItem>& items() const { return _items; }
    std::vector<MenuItem>& items() { return _items; }

    bool isEmpty() const { return _items.empty(); }

private:
    std::vector<MenuItem> _items;
};

class MenuItem {
public:
    enum class State {
        kOff,   // nothing
        kMixed, // dash
        kOn,    // check mark
    };

    /**
    * Creates a non-interactive separator item.
    */
    MenuItem() {}

    /**
    * Creates an item with the given label and command.
    */
    MenuItem(std::string label
            , Command command
            , KeyCode keyCode = KeyCode::kNone
            , KeyModifiers keyModifiers = KeyModifier::kNone
            , std::function<State()> stateFunc = {})
        : _label{std::move(label)}
        , _command{command}
        , _keyCode{keyCode}
        , _keyModifiers{keyModifiers}
        , _stateFunc{std::move(stateFunc)}
    {}

    /**
    * Creates an item with the given label and command.
    */
    MenuItem(std::string label
            , Command command
            , CommandContext commandContext
            , KeyCode keyCode = KeyCode::kNone
            , KeyModifiers keyModifiers = KeyModifier::kNone
            , std::function<State()> stateFunc = {})
        : _label{std::move(label)}
        , _command{command}
        , _commandContext{commandContext}
        , _keyCode{keyCode}
        , _keyModifiers{keyModifiers}
        , _stateFunc{std::move(stateFunc)}
    {}

    /**
    * Creates an item with the given label and submenu.
    */
    MenuItem(const char* label, const Menu& submenu) : _label(label), _submenu(submenu) {}

    /**
    * Indicates whether or not the item is a separator rather than interactive item.
    */
    bool isSeparator() const { return _label.empty(); }

    /**
    * Returns the label for the item (if it's not a separator).
    */
    const std::string& label() const { return _label; }

    /**
    * Returns the command that the item should dispatch when selected.
    */
    const Command& command() const { return _command; }

    /**
    * Returns the context for the command that the item should dispatch when selected.
    */
    const CommandContext& commandContext() const { return _commandContext; }

    /**
    * Returns the key code that can be used to dispatch the menu item's command.
    */
    const KeyCode& keyCode() const { return _keyCode; }

    const KeyModifiers& keyModifiers() const { return _keyModifiers; }

    const Menu& submenu() const { return _submenu; }
    Menu& submenu() { return _submenu; }

    State state() const { return _stateFunc ? _stateFunc() : State::kOff; }

private:
    std::string    _label;
    Command        _command = kCommandNone;
    CommandContext _commandContext;
    KeyCode        _keyCode = KeyCode::kNone;
    KeyModifiers   _keyModifiers = KeyModifier::kNone;
    Menu           _submenu;
    std::function<State()> _stateFunc = {};
};

} // namespace okui
