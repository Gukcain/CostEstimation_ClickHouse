#pragma once

#include <Processors/Merges/IMergingTransform.h>
#include <Processors/Merges/Algorithms/SummingSortedAlgorithm.h>

namespace DB
{
    // using namespace std;
// template <typename T>
class ParaVal7{
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
        Names column_names_to_sum;
        Names partition_key_columns;

        // ParaVal7();
};

/// Implementation of IMergingTransform via SummingSortedAlgorithm.
class SummingSortedTransform final : public IMergingTransform<SummingSortedAlgorithm>
{
public:
    ParaVal7 pv7 = ParaVal7();

    SummingSortedTransform(
        const Block & header, size_t num_inputs,
        SortDescription description_,
        /// List of columns to be summed. If empty, all numeric columns that are not in the description are taken.
        const Names & column_names_to_sum,
        const Names & partition_key_columns,
        size_t max_block_size)
        : IMergingTransform(
            num_inputs, header, header, /*have_all_inputs_=*/ true, /*limit_hint_=*/ 0,
            header,
            num_inputs,
            std::move(description_),
            column_names_to_sum,
            partition_key_columns,
            max_block_size)
    {
        pv7.header = header;
        pv7.num_inputs = num_inputs;
        // pv7.description = description_;
        pv7.max_block_size = max_block_size;
        // pv7.input_header = header;
        // pv7.output_header = header;
        pv7.have_all_inputs = true;
        pv7.limit_hint = 0;
        pv7.column_names_to_sum = column_names_to_sum;
        pv7.partition_key_columns = partition_key_columns;
    }
    // ParaVal pv7;
    std::vector<Param> getParaList() override{
        // ParaVal pv7 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv7.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv7.header.columns())));
        vec.push_back(Param("num_inputs", std::to_string(pv7.num_inputs)));
        vec.push_back(Param("max_block_size", std::to_string(pv7.max_block_size)));
        vec.push_back(Param("have_all_inputs", std::to_string(pv7.have_all_inputs)));
        vec.push_back(Param("limit_hint", std::to_string(pv7.limit_hint)));
        String str1;
        for(const auto & key : pv7.column_names_to_sum){
            str1 += key;
        }
        String str2;
        for(const auto & key : pv7.partition_key_columns){
            str2 += key;
        }
        vec.push_back(Param("column_names_to_sum", str1));
        vec.push_back(Param("partition_key_columns", str1));
        return vec;
    }

    String getName() const override { return "SummingSortedTransform"; }
};

}
