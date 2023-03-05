#ifndef UTILITES_FSM
#define UTILITES_FSM

#include <variant>
#include <optional>
#include <iostream>

namespace utilities{


    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

    template <typename Derived, typename StateType> class Fsm {
    public:
        const StateType &getState() const { return _currentState; }

        template <typename Event>
        void transition(Event &&event) {
            Derived &_this = static_cast<Derived &>(*this);
            auto nextState = std::visit(
                    [&](auto &_state) -> std::optional<StateType> {
                        /*
                         * event is an unviersal reference
                         * it can be bound to an rvalue reference or an lvalue reference.
                         * so we need std::forward to conditonally cast to an rvalue reference,
                         * std::forward only cast to rvalue reference, if it's argument was initialized with an rvalue.
                        */
                        return _this.processEvent(_state, std::forward<Event>(event));
                    },
                    _currentState);

            if (nextState){
                //std::cout << std::visit(printName(),_currentState) << "->";
                _currentState = *std::move(nextState);
                //std::cout << std::visit(printName(),_currentState) << std::endl;
            }
        }

    private:
        StateType _currentState;
    };

}

#endif