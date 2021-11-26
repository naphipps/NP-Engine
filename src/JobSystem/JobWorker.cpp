//
//  job_worker.cpp
//  Project Space
//
//  Created by Nathan Phipps on 9/2/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#include "NP-Engine/JobSystem/JobWorker.hpp"

namespace np
{
	namespace js
	{
		// must be initialized in cpp
		container::array<JobWorker*, concurrency::ThreadPool::MAX_THREAD_COUNT> JobWorker::_other_workers;
		ui64 JobWorker::_other_workers_size = 0;
	} // namespace js
} // namespace np
