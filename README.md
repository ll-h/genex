# Genex
A header-only *generationally indexed container* library

## Motivation
Generationally indexed containers are a cache-friendly way of storing objects that are weakly referenced throughout your program. It may be part of an [ECS](https://en.wikipedia.org/wiki/Entity_component_system) as described during [this talk](https://youtu.be/aKLntZcp27M).

## Inspirations
- The aforementionned [talk](https://youtu.be/aKLntZcp27M)
- [Slotmap](https://github.com/orlp/slotmap) but with C++'s perfect-forwarding

## Documentation
The test cases are the perfect user-friendly documentation.

## Installation (Work in progress)
Copy-paste the content of the _include_ folder where you want to use this library.

## Missing Features
Loosely ordered by priority:
- Clean installation and use as a CMake module
- Iterator over the elements
- Actual documentation
- Embedding the free indexes within the freed objects as a singly-linked list
- Testing for weird types (non-copiabe, non-movable, ...)
