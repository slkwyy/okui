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

#include <okui/StateMachine.h>
#include <okui/ml/ElementInterface.h>
#include <okui/ml/ElementTypeInterface.h>

#include <stdts/optional.h>

namespace okui::ml::elements {

/**
* The statemachine element can be used to manipulate attributes of a parent element with pre-defined
* states that are easily controlled via context:
*
*    <statemachine state="{state}">
*        <state id="standby" opacity="50%" />
*        <state id="present" opacity="100%" />
*        <state id="gone" opacity="50%" />
*        <transition from="standby" to="present" duration="200ms" interpolation="quadratic-ease-out" />
*        <transition from="present" to="gone" duration="200ms" interpolation="quadratic-ease-in" />
*    </statemachine>
*
* All attributes of the current state will be applied to the parent element, with the exception of
* the "id" attribute. If transitions are provided, attributes that support interpolation will be
* animated.
*
* Transition attributes do not support context variable rendering.
*
* Behavior is undefined if multiple state machines attempt to define the same attribute, or if a
* state machine attempts to define an attribute that is already defined in the parent.
*
* See examples/markup for more usage.
*/
class StateMachine : public ElementTypeInterface {
public:
    virtual std::unique_ptr<ElementInterface> create() const override {
        return std::make_unique<Element>();
    }

protected:
    class Element : public ElementInterface {
    public:
        virtual void initialize(const Context* context, const pugi::xml_node& xml) override;
        virtual void update() override;

        virtual const char* id() const override { return _id.c_str(); }

        virtual StateMachineInterface* stateMachine() override { return _stateMachine.get(); }

    private:
        struct State {
            struct Attributes {
                Attributes operator*(double scale) {
                    Attributes ret;
                    for (auto& kv : values) {
                        auto& sum = ret.values[kv.first];
                        sum = kv.second;
                        for (auto& component : sum) {
                            component.coefficient *= scale;
                        }
                    }
                    return ret;
                }

                Attributes operator+(const Attributes& other) {
                    Attributes ret;
                    for (auto& kv : values) {
                        auto& sum = ret.values[kv.first];
                        sum = kv.second;
                        auto it = other.values.find(kv.first);
                        if (it != other.values.end()) {
                            sum.insert(sum.end(), it->second.begin(), it->second.end());
                        }
                    }

                    for (auto& kv : other.values) {
                        if (!values.count(kv.first)) {
                            values[kv.first] = kv.second;
                        }
                    }

                    return ret;
                }

                std::unordered_map<stdts::string_view, std::vector<StateMachineDelegate::ValueComponent>> values;
            } attributes;

            auto members() { return std::tie(attributes); }
        };

        struct StateMachine : okui::StateMachine<State>, StateMachineInterface {
            explicit StateMachine(std::string initialState, bool trigger = false)
                : okui::StateMachine<State>(std::move(initialState))
                , triggerStateMachine{trigger}
            {}

            // StateMachine
            virtual void update(stdts::string_view id, State& state, bool& isAnimated) override;
            virtual void apply(State state) override;

            // StateMachineInterface
            virtual void setDelegate(StateMachineDelegate* delegate) override { this->delegate = delegate; }
            virtual bool drive() override                                     { return okui::StateMachine<State>::drive(); }
            virtual std::string state() const override                        { return okui::StateMachine<State>::state(); }
            virtual void setState(std::string id) override                    { okui::StateMachine<State>::setState(std::move(id)); }
            virtual void reset(std::string id) override                       { okui::StateMachine<State>::reset(id); }
            virtual bool hasState(stdts::string_view id) const override       { return okui::StateMachine<State>::hasState(id); }
            virtual bool hasTrigger(stdts::string_view id) const override     { return triggerStateMachine && hasState(id); }

            struct StateDefinition {
                std::unordered_map<std::string, std::string> attributes;
            };

            StateMachineDelegate*                            delegate = nullptr;
            std::unordered_map<std::string, StateDefinition> stateDefinitions;
            bool                                             triggerStateMachine = false;
        };

        const Context*                _context = nullptr;
        std::string                   _idAttribute;
        std::string                   _id;
        stdts::optional<std::string>  _stateAttribute;
        std::unique_ptr<StateMachine> _stateMachine;
    };
};

} // namespace okui::ml::elements
