# NP-Engine

This game engine started out as my first video game, but since the project kept growing, it ended up splitting into a game engine and a video game. So, here is my game engine code, as I'll keep my video game stuff private :) Checkout the roadmap below to see what's in store!

- [Roadmap for NP-Engine found on Trello](https://trello.com/b/YJhL1R6V)
- Building is done with CMake. All dependencies are submodules (under the [vendor directory](https://github.com/naphipps/NP-Engine/tree/master/vendor)) except for Vulkan, so install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home). (Linux might require libgtk-3-dev or more but check the CMakeFiles.txt.)
- Goals/Features/Baseline of this engine:
	- [x] Detailed memory management. (I aim to use vendors that I can pipe memory management through my own stuff.)
	- [ ] Vulkan rendering - **currently under heavy construction.** (OpenGL and DX11/DX12 _probably much_ later as means to improve our API).
	- [x] Support for Windows and Linux (latest Debian). Official support for Apple is deferred.
	- [x] ECS via [Entt](https://github.com/skypjack/entt)
		- [ ] I am seriously considering forking Entt to force it support my own containers, which all use my memory management.
	- [ ] 2D Physics via [Box2D](https://github.com/erincatto/box2d) with [liquidfun's](https://github.com/google/liquidfun) particle physics.
		- I will use Box2D V3 when it is available.
		- I will also improve the pressure solvers to _probably_ IISPH or something that is better, faster, and more accurate.
	- [ ] 3D Physics via [Bullet](https://github.com/bulletphysics/bullet3)
	- [x] A feature-rich JobSystem (task graph architecture):
		- [x] Lightweight. Job submition is as small as an ui32 and smart ptr.
		- [x] Power-efficient. JobWorkers sleep when no Jobs are available, and are woken up when Jobs are submitted.
		- [x] Priority-based jobs
		- [x] Braided-parallelism support (you can create a job anywhere, adding job-dependencies anytime)
		- [x] Considerate of the main thread so thread scheduling does not throttle/crowd the main thread
		- [x] Jobs can be checked for completion so the caller can easily know when a Job has completed.
	- [x] A profiler that outputs a JSON file for Chrome's Tracing tool. (Type "chrome://tracing/" in Chrome's url.)
		- [ ] I am going to migrate to [wolfpld's Tracy Profiler](https://github.com/wolfpld/tracy)
	- [x] Networking
		- [ ] Ensure it is as flexible as curl or something.
	- [ ] A wiki (_coming soon_) for all documentation needs, including high-level examples. (I _might_ make a separate repo for working samples.)

***

**Note: This project is pre-pre-alpha. Tags will be implemented when the initial backlog is complete so you can navigate official versions of the engine. Until then, commits are liable to break the build.**

**Here is the internal dependency of how things are arranged. Everything is subject to change, and this is liable to be out of date.**

![](https://raw.githubusercontent.com/naphipps/NP-Engine/master/docs/uml-screenshot.png)