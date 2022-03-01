# NP-Engine

This game engine started out as my first video game, but because the project kept growing, it ended up splitting into a game engine and a video game. So, here is my game engine code, as I'll keep my video game stuff private :) Checkout the roadmap below to see what's in store! Let me know if you have any comments, questions, or concerns by creating an issue.

- [Roadmap for NP-Engine found on Trello](https://trello.com/b/YJhL1R6V)
- Building is done with CMake. All dependencies are submodules (under the [vendor directory](https://github.com/naphipps/NP-Engine/tree/master/vendor)) except for Vulkan, so install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home).
- Goals/Features/Baseline of this engine:
	- [x] Detailed memory management. (I aim to use vendors that I can pipe memory management through my own stuff.)
	- [ ] Vulkan rendering (maybe OpenGL later).
	- [x] Cross platform (Apple, Linux (latest Ubuntu at least), Windows 10).
	- [x] ECS via [Entt](https://github.com/skypjack/entt)
	- [ ] 2D Physics via [Box2D](https://github.com/erincatto/box2d) with [liquidfun's](https://github.com/google/liquidfun) particle physics. I will also improve the pressure solvers to _probably_ IISPH or something that is better, and more accurate.
	- [x] A priority-based JobSystem that automatically scales up/down for the platform.
	- [x] A profiler that outputs a JSON file for Chrome's Tracing tool. (Type "chrome://tracing/" in Chrome's url.)

***

**Note: This project is a work in progress. Tags will be implemented when the v0.0.0.1 backlog is complete so you can navigate official versions of the engine. Until then, commits are liable to break the build.**