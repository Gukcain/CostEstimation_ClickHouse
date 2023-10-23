#pragma once

#include <Processors/Merges/IMergingTransform.h>
#include <Processors/Merges/Algorithms/MergingSortedAlgorithm.h>


namespace DB
{

// template <typename T>
class ParaVal5{
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
        UInt64 limit = 0;
        bool quiet = false;
        bool use_average_block_sizes = false;

        // ParaVal5();
};

/// Implementation of IMergingTransform via MergingSortedAlgorithm.
class MergingSortedTransform final : public IMergingTransform<MergingSortedAlgorithm>
{
public:
    ParaVal5 pv5 = ParaVal5();
    MergingSortedTransform(
        const Block & header,
        size_t num_inputs,
        const SortDescription & description,
        size_t max_block_size,
        SortingQueueStrategy sorting_queue_strategy,
        UInt64 limit_ = 0,
        WriteBuffer * out_row_sources_buf_ = nullptr,
        bool quiet_ = false,
        bool use_average_block_sizes = false,
        bool have_all_inputs_ = true);

    String getName() const override { return "MergingSortedTransform"; }

    std::vector<Param> getParaList() override{
        // ParaVal pv5 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv5.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv5.header.columns())));
        vec.push_back(Param("num_inputs", std::to_string(pv5.num_inputs)));
        vec.push_back(Param("max_block_size", std::to_string(pv5.max_block_size)));
        vec.push_back(Param("have_all_inputs", std::to_string(pv5.have_all_inputs)));
        vec.push_back(Param("limit", std::to_string(pv5.limit)));
        vec.push_back(Param("quiet", std::to_string(pv5.quiet)));
        vec.push_back(Param("use_average_block_sizes", std::to_string(pv5.use_average_block_sizes)));
        return vec;
    }

protected:
    void onNewInput() override;
    void onFinish() override;

private:
    bool quiet = false;
};

}
