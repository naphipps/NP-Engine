# NP-Engine

This game engine started out as my first video game, but since the project kept growing, it ended up splitting into a game engine and a video game. So, here is my game engine code, as I'll keep my video game stuff private :) Checkout the roadmap below to see what's in store!

- [Roadmap for NP-Engine found on Trello](https://trello.com/b/YJhL1R6V)
- Building is done with CMake. All dependencies are submodules (under the [vendor directory](https://github.com/naphipps/NP-Engine/tree/master/vendor)) except for Vulkan, so install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home). (Linux might require libgtk-3-dev or more but check the CMakeFiles.txt.)
- Goals/Features/Baseline of this engine:
	- [x] Detailed memory management. (I aim to use vendors that I can pipe memory management through my own stuff.)
	- [ ] Vulkan rendering (maybe OpenGL later, maybe DX12 for fun).
	- [x] Support for Windows and Linux (latest Ubunutu). Official support for Apple is deferred.
	- [x] ECS via [Entt](https://github.com/skypjack/entt)
	  - [ ] I am considering forking Entt to force it support my own containers, which all use my memory management.
	- [ ] 2D Physics via [Box2D](https://github.com/erincatto/box2d) with [liquidfun's](https://github.com/google/liquidfun) particle physics.
	  - I will also improve the pressure solvers to _probably_ IISPH or something that is better, and more accurate.
	- [ ] 3D Physics via [Bullet](https://github.com/bulletphysics/bullet3)
	- [x] A feature-rich JobSystem (task graph architecture):
	  - [x] priority-based jobs
	  - [x] controller over individual job workers
	  - [x] automatic scaling up/down for the platform it runs on
	  - [x] considerate of the main thread so thread scheduling does not throttle the main thread
	- [x] A profiler that outputs a JSON file for Chrome's Tracing tool. (Type "chrome://tracing/" in Chrome's url.)

***

**Note: This project is a work in progress. Tags will be implemented when the v0.0.0.1 backlog is complete so you can navigate official versions of the engine. Until then, commits are liable to break the build.**

**Here is the internal dependency of how things are arranged. Everything is subject to change.**

![](https://raw.githubusercontent.com/naphipps/NP-Engine/master/docs/uml-screenshot.png)