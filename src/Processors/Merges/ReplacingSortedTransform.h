#pragma once

#include <Processors/Merges/IMergingTransform.h>
#include <Processors/Merges/Algorithms/ReplacingSortedAlgorithm.h>


namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal6{
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
        String version_column;
        bool use_average_block_sizes = false;

        // ParaVal6();
};

/// Implementation of IMergingTransform via ReplacingSortedAlgorithm.
class ReplacingSortedTransform final : public IMergingTransform<ReplacingSortedAlgorithm>
{
public:
    ParaVal6 pv6 = ParaVal6();
    ReplacingSortedTransform(
        const Block & header, size_t num_inputs,
        SortDescription description_, const String & version_column,
        size_t max_block_size,
        WriteBuffer * out_row_sources_buf_ = nullptr,
        bool use_average_block_sizes = false)
        : IMergingTransform(
            num_inputs, header, header, /*have_all_inputs_=*/ true, /*limit_hint_=*/ 0,
            header,
            num_inputs,
            std::move(description_),
            version_column,
            max_block_size,
            out_row_sources_buf_,
            use_average_block_sizes)
    {
        pv6.header = header;
        pv6.num_inputs = num_inputs;
        // pv6.description = description_;
        pv6.max_block_size = max_block_size;
        // pv6.input_header = header;
        // pv6.output_header = header;
        pv6.have_all_inputs = true;
        pv6.limit_hint = 0;
        pv6.version_column = version_column;
        pv6.use_average_block_sizes = use_average_block_sizes;
    }

    std::vector<Param> getParaList() override{
        // ParaVal pv6 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv6.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv6.header.columns())));
        vec.push_back(Param("num_inputs", std::to_string(pv6.num_inputs)));
        vec.push_back(Param("max_block_size", std::to_string(pv6.max_block_size)));
        vec.push_back(Param("have_all_inputs", std::to_string(pv6.have_all_inputs)));
        vec.push_back(Param("limit_hint", std::to_string(pv6.limit_hint)));
        vec.push_back(Param("version_column", pv6.version_column));
        vec.push_back(Param("use_average_block_sizes", std::to_string(pv6.use_average_block_sizes)));
        return vec;
    }

    String getName() const override { return "ReplacingSorted"; }
};

}
