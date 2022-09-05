//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_UID_GENERATOR_HPP
#define NP_ENGINE_UID_GENERATOR_HPP

#include "NP-Engine/Random/Random.hpp"

#include "NP-Engine/Vendor/StduuidInclude.hpp"

namespace np::uid
{
	class UidGenerator : public uuids::basic_uuid_random_generator<rng::Random64>
	{
	private:
		rng::Random64 _rng;

	public:
		UidGenerator(): uuids::basic_uuid_random_generator<rng::Random64>(_rng) {}

		rng::Random64& GetRng()
		{
			return _rng;
		}

		const rng::Random64& GetRng() const
		{
			return _rng;
		}
	};
} // namespace np::uid

#endif /* NP_ENGINE_UID_GENERATOR_HPP */