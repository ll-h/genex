# Genex
A header-only *generationally indexed container* library

## Motivation
Generationally indexed containers are a cache-friendly way of storing objects that are weakly referenced throughout your program. It may be part of an [ECS](https://en.wikipedia.org/wiki/Entity_component_system) as described during [this talk](https://youtu.be/aKLntZcp27M).

## Inspirations
- The aforementionned [talk](https://youtu.be/aKLntZcp27M)
- [Slotmap](https://github.com/orlp/slotmap) but with C++'s perfect-forwarding

## Installation
### Work in progress
Copy-paste the content of the _include_ folder where you want to use this library.

## Missing Features
- Iterator over the elements
- Embedding the free indexes within the freed objects as a singly-linked list
- Clean installation as a CMake module
- Testing for wierd types (non-copiabe, non-movable, ...)
