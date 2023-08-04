# Genex
A header-only *generationally indexed container* library

## Motivation
Generationally indexed containers are a cache-friendly way of storing objects that are weakly referenced throughout your program. It may be part of an [ECS](https://en.wikipedia.org/wiki/Entity_component_system) as described during [this talk](https://youtu.be/aKLntZcp27M).

## Inspirations
- The aforementionned [talk](https://youtu.be/aKLntZcp27M)
- [Slotmap](https://github.com/orlp/slotmap) but with C++'s perfect-forwarding

## Documentation
The test cases are the perfect user-friendly documentation. For single element creation and access you may look at [this file](tests/generic/gic_base_tests.hpp) and for iterations you may look at [this one](tests/generic/gic_iterator_tests.hpp).

## Installation (Work in progress)
Copy-paste the content of the _include_ folder where you want to use this library.

## Testing
```shell
cmake .
make check
```

## Missing Features
Loosely ordered by priority:
- Clean installation and use as a CMake module
- Actual documentation
- Testing for weird types (non-copyabe, non-movable, ...)
