#pragma once

#include <Processors/Merges/IMergingTransform.h>
#include <Processors/Merges/Algorithms/VersionedCollapsingAlgorithm.h>


namespace DB
{
    // using namespace std;
// template <typename T>
class ParaVal8{
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
        String sign_column;
        bool use_average_block_sizes = false;

        // ParaVal8();
};

/// Implementation of IMergingTransform via VersionedCollapsingAlgorithm.
class VersionedCollapsingTransform final : public IMergingTransform<VersionedCollapsingAlgorithm>
{
public:
    ParaVal8 pv8 = ParaVal8();
    /// Don't need version column. It's in primary key.
    VersionedCollapsingTransform(
        const Block & header, size_t num_inputs,
        SortDescription description_, const String & sign_column_,
        size_t max_block_size,
        WriteBuffer * out_row_sources_buf_ = nullptr,
        bool use_average_block_sizes = false)
        : IMergingTransform(
            num_inputs, header, header, /*have_all_inputs_=*/ true, /*limit_hint_=*/ 0,
            header,
            num_inputs,
            std::move(description_),
            sign_column_,
            max_block_size,
            out_row_sources_buf_,
            use_average_block_sizes)
    {
        pv8.header = header;
        pv8.num_inputs = num_inputs;
        // pv8.description = description_;
        pv8.max_block_size = max_block_size;
        // pv8.input_header = header;
        // pv8.output_header = header;
        pv8.have_all_inputs = true;
        pv8.limit_hint = 0;
        pv8.sign_column = sign_column_;
        pv8.use_average_block_sizes = use_average_block_sizes;
    }

    std::vector<Param> getParaList() override{
        // ParaVal pv8 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv8.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv8.header.columns())));
        vec.push_back(Param("num_inputs", std::to_string(pv8.num_inputs)));
        vec.push_back(Param("max_block_size", std::to_string(pv8.max_block_size)));
        vec.push_back(Param("have_all_inputs", std::to_string(pv8.have_all_inputs)));
        vec.push_back(Param("limit_hint", std::to_string(pv8.limit_hint)));
        vec.push_back(Param("sign_column", pv8.sign_column));
        vec.push_back(Param("use_average_block_sizes", std::to_string(pv8.use_average_block_sizes)));
        return vec;
    }

    String getName() const override { return "VersionedCollapsingTransform"; }
};

}
