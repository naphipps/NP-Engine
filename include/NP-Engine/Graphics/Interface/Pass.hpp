//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_PASS_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_PASS_HPP

namespace np::gpu
{
	class Pass
	{
	protected:

		Pass() {}

	public:

		//TODO: we need to better understand subpasses and how we can feed the output of one into the input of a later one

	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_PASS_HPP */