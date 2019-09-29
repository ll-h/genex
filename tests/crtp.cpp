#include "genex_crtp.hpp"

template<class Derived>
struct base : genex::crtp_base<Derived> {
    void call_derived_do_stuff() {
        this->as_derived().Derived::do_stuff();
    }

    void call_derived_do_const_stuff(int& val) const {
        this->as_derived().Derived::do_const_stuff(val);
    }

    void call_derived_do_private_stuff() {
        this->as_derived().Derived::do_private_stuff();
    }
};

struct derived : base<derived> {
    int val{9};

    void do_stuff() {
        val = 0;
    }

    void do_const_stuff(int & v) const {
        v = val;
    }

private:
    friend base<derived>;
    void do_private_stuff() {
        //we just need this to compile
    }
};

void call_do_stuff(base<derived>& obj) {
    obj.call_derived_do_stuff();
}

void call_do_const_stuff(base<derived> const & obj, int & val) {
    obj.call_derived_do_const_stuff(val);
}

void call_do_private_stuff(base<derived>& obj) {
    obj.call_derived_do_private_stuff();
}

int main() {
    derived mut_val;
    call_do_stuff(mut_val);
    if (mut_val.val != 0)
        return mut_val.val;

    int ret = 77;
    call_do_const_stuff(mut_val, ret);

    return ret;
}

