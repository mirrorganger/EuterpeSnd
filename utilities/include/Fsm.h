#ifndef UTILITES_FSM
#define UTILITES_FSM

#include <variant>
#include <optional>

namespace utilities{
    template <typename Derived, typename StateType> class Fsm {
    public:
        const StateType &getState() const { return _currenState; }

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
                    _currenState);

            if (nextState)
                _currenState = *std::move(nextState);
        }

    private:
        StateType _currenState;
    };

}

#endif