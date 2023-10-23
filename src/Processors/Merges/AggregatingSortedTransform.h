#pragma once

#include <Processors/Merges/IMergingTransform.h>
#include <Processors/Merges/Algorithms/AggregatingSortedAlgorithm.h>


#include <utility>
#include <any>
#include <vector>
#include <string>
#include <Processors/Param.h>


namespace DB
{

class ColumnAggregateFunction;

// using namespace std;
// template <typename T>
class ParaVal1{
    public:
        // T value;

        // const Block
        Block  header;
        size_t num_inputs;
        // SortDescription description;
        size_t max_block_size;
        // Block & input_header;
        // Block & output_header;
        bool have_all_inputs = true;
        UInt64 limit_hint = 0;

        // ParaVal1();
};


// using Param = std::pair<String, ParaVal>;

/// Implementation of IMergingTransform via AggregatingSortedAlgorithm.
class AggregatingSortedTransform final : public IMergingTransform<AggregatingSortedAlgorithm>
{
public:

    ParaVal1 pv = ParaVal1();

    AggregatingSortedTransform(
        const Block & header, size_t num_inputs,
        SortDescription description_, size_t max_block_size)
        : IMergingTransform(
            num_inputs, header, header, /*have_all_inputs_=*/ true, /*limit_hint_=*/ 0,
            header,
            num_inputs,
            std::move(description_),
            max_block_size)
    {
        pv.header = header;
        pv.num_inputs = num_inputs;
        // pv.description = description_;
        pv.max_block_size = max_block_size;
        // pv.input_header = header;
        // pv.output_header = header;
        pv.have_all_inputs = true;
        pv.limit_hint = 0;
    }

    // ParaVal pv;
    std::vector<Param> getParaList() override{
        // ParaVal pv = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv.header.columns())));
        vec.push_back(Param("num_inputs", std::to_string(pv.num_inputs)));
        vec.push_back(Param("max_block_size", std::to_string(pv.max_block_size)));
        vec.push_back(Param("have_all_inputs", std::to_string(pv.have_all_inputs)));
        vec.push_back(Param("limit_hint", std::to_string(pv.limit_hint)));
        return vec;
    }

    String getName() const override { return "AggregatingSortedTransform"; }
};

}
