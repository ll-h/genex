#ifndef GENEX_CRTP_HPP
#define GENEX_CRTP_HPP

namespace genex::detail {

// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
template<class Derived>
struct crtp_base {
protected:
    using derived_type = Derived;

    Derived& as_derived() & {
        return static_cast<Derived&>(*this);
    }

    Derived const & as_derived() const & {
        return static_cast<Derived const &>(*this);
    }

    Derived&& as_derived() && {
        return static_cast<Derived&&>(*this);
    }

    Derived const && as_derived() const && {
        return static_cast<Derived const &&>(*this);
    }
};

} // end namespace genex::detail

#endif // GENEX_CRTP_HPP
