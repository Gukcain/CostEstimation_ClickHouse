#pragma once

#include <Processors/Merges/IMergingTransform.h>
#include <Processors/Merges/Algorithms/GraphiteRollupSortedAlgorithm.h>

namespace DB
{
    // using namespace std;
// template <typename T>
class ParaVal4{
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
        time_t time_of_merge;

        // ParaVal4();
};

/// Implementation of IMergingTransform via GraphiteRollupSortedAlgorithm.
class GraphiteRollupSortedTransform final : public IMergingTransform<GraphiteRollupSortedAlgorithm>
{
public:
    ParaVal4 pv4 = ParaVal4();
    GraphiteRollupSortedTransform(
        const Block & header, size_t num_inputs,
        SortDescription description_, size_t max_block_size,
        Graphite::Params params_, time_t time_of_merge_)
        : IMergingTransform(
            num_inputs, header, header, /*have_all_inputs_=*/ true, /*limit_hint_=*/ 0,
            header,
            num_inputs,
            std::move(description_),
            max_block_size,
            std::move(params_),
            time_of_merge_)
    {
        pv4.header = header;
        pv4.num_inputs = num_inputs;
        // pv4.description = description_;
        pv4.max_block_size = max_block_size;
        // pv4.input_header = header;
        // pv4.output_header = header;
        pv4.have_all_inputs = true;
        pv4.limit_hint = 0;
        pv4.time_of_merge = time_of_merge_;
    }

    std::vector<Param> getParaList() override{
        // ParaVal pv4 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv4.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv4.header.columns())));
        vec.push_back(Param("num_inputs", std::to_string(pv4.num_inputs)));
        vec.push_back(Param("max_block_size", std::to_string(pv4.max_block_size)));
        vec.push_back(Param("have_all_inputs", std::to_string(pv4.have_all_inputs)));
        vec.push_back(Param("limit_hint", std::to_string(pv4.limit_hint)));
        vec.push_back(Param("time_of_merge", std::to_string(pv4.time_of_merge)));
        return vec;
    }
    
    String getName() const override { return "GraphiteRollupSortedTransform"; }
};

}
