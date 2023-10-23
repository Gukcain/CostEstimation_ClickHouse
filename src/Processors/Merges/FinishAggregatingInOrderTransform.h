#pragma once

#include <Processors/Merges/IMergingTransform.h>
#include <Processors/Merges/Algorithms/FinishAggregatingInOrderAlgorithm.h>
#include "Processors/Transforms/AggregatingTransform.h"

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal3{
    public:
        // T value;

        // const Block
        Block header;
        size_t num_inputs;
        // SortDescription description;
        size_t max_block_size;
        // Block & input_header;
        // Block & output_header;
        bool have_all_inputs = true;
        UInt64 limit_hint = 0;
        size_t max_block_bytes;
        AggregatingTransformParamsPtr ptr;

        // ParaVal3();
};

class ColumnAggregateFunction;

/// Implementation of IMergingTransform via FinishAggregatingInOrderAlgorithm.
class FinishAggregatingInOrderTransform final : public IMergingTransform<FinishAggregatingInOrderAlgorithm>
{
public:

    ParaVal3 pv3 = ParaVal3();

    FinishAggregatingInOrderTransform(
        const Block & header,
        size_t num_inputs,
        AggregatingTransformParamsPtr params,
        SortDescription description,
        size_t max_block_size,
        size_t max_block_bytes)
        : IMergingTransform(
            num_inputs, header, {}, /*have_all_inputs_=*/ true, /*limit_hint_=*/ 0,
            header,
            num_inputs,
            params,
            std::move(description),
            max_block_size,
            max_block_bytes)
    {
        pv3.header = header;
        pv3.num_inputs = num_inputs;
        // pv3.description = description_;
        pv3.max_block_size = max_block_size;
        // pv3.input_header = header;
        // pv3.output_header = header;
        pv3.have_all_inputs = true;
        pv3.limit_hint = 0;
        pv3.max_block_bytes = max_block_bytes;
        pv3.ptr = params;
    }

    String getName() const override { return "FinishAggregatingInOrderTransform"; }

    std::vector<Param> getParaList() override{
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv3.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv3.header.columns())));
        vec.push_back(Param("num_inputs", std::to_string(pv3.num_inputs)));
        vec.push_back(Param("max_block_size", std::to_string(pv3.max_block_size)));
        vec.push_back(Param("have_all_inputs", std::to_string(pv3.have_all_inputs)));
        vec.push_back(Param("limit_hint", std::to_string(pv3.limit_hint)));
        vec.push_back(Param("max_block_bytes", std::to_string(pv3.max_block_bytes)));
        if(pv3.ptr){
            String str;
            for(const auto & key : pv3.ptr.get()->params.keys){
                str += key;
            }
            vec.push_back(Param("keys", str));
            vec.push_back(Param("keys_size", std::to_string(pv3.ptr.get()->params.keys_size)));
            vec.push_back(Param("aggregates_size", std::to_string(pv3.ptr.get()->params.aggregates_size)));
            vec.push_back(Param("max_rows_to_group_by", std::to_string(pv3.ptr.get()->params.max_rows_to_group_by)));
            vec.push_back(Param("min_free_disk_space", std::to_string(pv3.ptr.get()->params.min_free_disk_space)));
            
        }
        return vec;
    }
};

}
