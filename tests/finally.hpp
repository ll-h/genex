#ifndef FINALLY_HPP
#define FINALLY_HPP

#include <utility>
#include <functional>

template<typename F>
class finally {
private:
    F on_destruction;

public:
    finally(F&& f) : on_destruction(std::move(f)) {}
    finally(F const& f) : on_destruction(f) {}

    ~finally() {
        std::invoke(std::forward<F>(on_destruction));
    }
};

#endif // FINALLY_HPP
