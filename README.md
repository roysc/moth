Moth
----

A simulation/game engine exercise (whose features have crept a bit out of scope).

Entities and components are defined inside JSON files, and specify the behavior of a game or simulation. Originally this was intended to allow flexibility, letting components be changed easily. Eventually I realized I was implementing a full dynamic programming language, albeit a simple one.

The project is not complete, but it successfully parses a specification file and begins running an event loop. Time is a built-in component which ticks on each update, and the game terminated by a trigger at a certain stop time. If nothing else, this was a good excuse to play with Boost's qi parser, and work on a logger and some other small tools.
