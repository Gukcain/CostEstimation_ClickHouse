#pragma once

#include <Processors/Merges/IMergingTransform.h>
#include <Processors/Merges/Algorithms/CollapsingSortedAlgorithm.h>

namespace DB
{
// using namespace std;
class ParaVal2{
    public:
        // T value;

        // const Block
        Block header;
        size_t num_inputs;
        String sign_column;
        // SortDescription description;
        size_t max_block_size;
        bool only_positive_sign;
        // Block & input_header;
        // Block & output_header;
        bool have_all_inputs = true;
        UInt64 limit_hint = 0;
        bool use_average_block_sizes = false;

        // ParaVal2();
};

/// Implementation of IMergingTransform via CollapsingSortedAlgorithm.
class CollapsingSortedTransform final : public IMergingTransform<CollapsingSortedAlgorithm>
{
public:
    ParaVal2 pv2 = ParaVal2();

    CollapsingSortedTransform(
        const Block & header,
        size_t num_inputs,
        SortDescription description_,
        const String & sign_column,
        bool only_positive_sign,
        size_t max_block_size,
        WriteBuffer * out_row_sources_buf_ = nullptr,
        bool use_average_block_sizes = false)
        : IMergingTransform(
            num_inputs, header, header, /*have_all_inputs_=*/ true, /*limit_hint_=*/ 0,
            header,
            num_inputs,
            std::move(description_),
            sign_column,
            only_positive_sign,
            max_block_size,
            &Poco::Logger::get("CollapsingSortedTransform"),
            out_row_sources_buf_,
            use_average_block_sizes)
    {
        pv2.header = header;
        pv2.num_inputs = num_inputs;
        // pv2.description = description_;
        pv2.max_block_size = max_block_size;
        // pv2.input_header = header;
        // pv2.output_header = header;
        pv2.have_all_inputs = true;
        pv2.limit_hint = 0;
        pv2.only_positive_sign = only_positive_sign;
        pv2.sign_column = sign_column;
        pv2.use_average_block_sizes = use_average_block_sizes;
    }

    std::vector<Param> getParaList() override{
        // ParaVal pv2 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv2.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv2.header.rows())));
        vec.push_back(Param("num_inputs", std::to_string(pv2.num_inputs)));
        vec.push_back(Param("max_block_size", std::to_string(pv2.max_block_size)));
        vec.push_back(Param("have_all_inputs", std::to_string(pv2.have_all_inputs)));
        vec.push_back(Param("limit_hint", std::to_string(pv2.limit_hint)));
        vec.push_back(Param("only_positive_sign", std::to_string(pv2.only_positive_sign)));
        vec.push_back(Param("sign_column", pv2.sign_column));
        vec.push_back(Param("use_average_block_sizes", std::to_string(pv2.use_average_block_sizes)));
        return vec;
    }

    String getName() const override { return "CollapsingSortedTransform"; }
};

}
