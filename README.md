# NP-Engine

This game engine started out as my first video game, but since the project kept growing, it ended up splitting into a game engine and a video game. So, here is my game engine code, as I'll keep my video game stuff private :) Checkout the roadmap below to see what's in store!

- [Roadmap for NP-Engine found on Trello](https://trello.com/b/YJhL1R6V)
- Building is done with CMake. All dependencies are submodules (under the [vendor directory](https://github.com/naphipps/NP-Engine/tree/master/vendor)) except for Vulkan, so install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home). (Linux might require libgtk-3-dev or more but check the CMakeFiles.txt.)
- Goals/Features/Baseline of this engine:
	- [x] Support for Windows and Linux (latest Debian). Official support for Apple is deferred.
	- [x] Detailed memory management. (I aim to use vendors that I can pipe memory management through my own stuff.)
		- Types of allocators:
			- [x] Red-Black Tree (efficient general-purpose)
			- [x] Implicit List (efficient bookkeeping, but allocations are a minimum size)
			- [x] Explicit List and Explicit Segmented List
			- [x] Linear (allocate in a straight line until unable -- can only deallocate all)
			- [x] Fallback (use the primary unlessit fails, then use fallback)
			- [x] Segregated (use primary for up-to threshold sized allocations, else use fallback)
			- [x] TraitAllocator (uses a statically-registered allocator under the hood)
			- [x] StdAllocator to tap into all std objects
			- [x] CAllocator (the classic malloc/realloc/free we all know and love)
			- [x] AccumulatingAllocator will use whatever type T allocator to give it under the hood, and will grow as your needs grow.
		- [x] Templated Object Pools. These return smart pointers so you do not have to remember to deallocate/destruct objects. They will automatically go back to the pool when that smart pointer goes out of scope or is reset.
			- [x] Accumulating Pool will continue to grow as your needs grow.
		- [x] Smart pointer. Counters and object are stored in one contiguous block. Smart pointers come in the Strong Pointer (sptr) and Weak Pointer (wptr) variants. Strong Pointers act like the classic shared_ptr, and Weak act like the classic weak_ptr. No unique_ptr, just use Strong Pointer.
		- [x] Delegates
			- [x] Can call functions, methods, and capture-less lambdas
			- [x] Provide access to a payload
			- [x] Provide context to callbacks via an id value
			- [x] Can return any type via template (all primitive delegates are provide i.e. BlDelegate, I32Delegate, FltDelegate)
			- _lambdas with capture clauses will not be supported_
	- [ ] Vulkan rendering - **currently under heavy construction.** (OpenGL and DX11/DX12 _probably much_ later as means to improve our API).
		- [ ] Add list of features here
	- [x] ECS via [Entt](https://github.com/skypjack/entt)
		- [ ] I am seriously considering forking Entt to force it support my own containers, which all use my memory management.
	- [ ] 2D Physics via [Box2D](https://github.com/erincatto/box2d) with [liquidfun's](https://github.com/google/liquidfun) particle physics.
		- I will use Box2D V3 when it is available.
		- I will also improve the pressure solvers to _probably_ IISPH or something that is better, faster, and more accurate.
	- [ ] 3D Physics via [Bullet](https://github.com/bulletphysics/bullet3)
	- [x] A feature-rich JobSystem (task graph architecture):
		- [x] Lightweight. Job submition is as small as an ui32, smart ptr, and boolean.
		- [x] Power-efficient. JobWorkers sleep when no Jobs are available, and are woken up when Jobs are submitted.
		- [x] Priority-based jobs: Highest, Higher, Normal, Lower, Lowest.
		- [x] Braided-parallelism support: you can create a job anywhere, adding job-dependencies anytime.
		- [x] Considerate of the main thread so thread scheduling does not throttle/crowd the main thread
		- [x] Jobs can be directly checked for completion so the caller can easily know when a Job has completed.
		- [x] JobWorkers can be directly assigned immediate jobs.
		- [x] JobWorkers can steal their coworkers' next immediate job.
		- [x] Jobs can be marked as CanBeStolen or not, just in case you want only a specific worker to execute a job. (Priority-Based jobs are stored in the JobSystem, so there's no concept of stealing them.)
		- [x] Every JobWorker's list of coworkers can be customized any time.
		- [x] JobWorkers pass their id into the Job they execute so the Job's callback can know which JobWorker is executing it.
	- [x] A profiler that outputs a JSON file for Chrome's Tracing tool. (Type "chrome://tracing/" in Chrome's url.)
		- [ ] I am going to migrate to [wolfpld's Tracy Profiler](https://github.com/wolfpld/tracy)
	- [x] Networking
		- [ ] Add TLS / cert stuff / etc
		- [ ] Add list of features here
	- [ ] A wiki (_coming soon_) for all documentation needs, including high-level examples. (I _might_ make a separate repo for working samples.)

***

**Note: This project is pre-pre-alpha. Tags will be implemented when the initial backlog is complete so you can navigate official versions of the engine. Until then, commits are liable to break the build.**

**Here is the internal dependency of how things are arranged. Everything is subject to change, and this is liable to be out of date.**

![](https://raw.githubusercontent.com/naphipps/NP-Engine/master/docs/uml-screenshot.png)