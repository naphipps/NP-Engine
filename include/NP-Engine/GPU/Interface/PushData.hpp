//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/7/24
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_PUSH_DATA_HPP
#define NP_ENGINE_GPU_INTERFACE_PUSH_DATA_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "Stage.hpp"

namespace np::gpu
{
    struct PushDataEntry
    {
        Stage stage = Stage::None;
        con::vector<ui8> bytes{};
    };

    struct PushData
    {
        con::vector<PushDataEntry> entries{};

        virtual con::vector<ui8> GetAllEntryBytes() const
        {
            con::vector<ui8> bytes{};
            for (const PushDataEntry& entry : entries)
                for (ui8 byte : entry.bytes)
                    bytes.emplace_back(byte);
            return bytes;
        }
    };
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_PUSH_DATA_HPP */
