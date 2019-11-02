#ifndef PERFECT_BACKWARD_HPP
#define PERFECT_BACKWARD_HPP

// The equivalent of perfect-forwarding with std::forward, but for returning.
// See: https://youtu.be/hwT8K3-NH1w
#define PERFECT_BACKWARD( x ) static_cast<decltype(x)>(x)

#endif // PERFECT_BACKWARD_HPP
