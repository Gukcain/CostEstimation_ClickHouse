#pragma once

#include <Core/SortDescription.h>
#include <Interpreters/Aggregator.h>
#include <Processors/ISimpleTransform.h>
#include <Processors/Transforms/AggregatingTransform.h>
#include <Processors/Transforms/finalizeChunk.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal25{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        size_t max_block_size;
        size_t max_block_bytes;
        size_t current_variant;
        AggregatingTransformParamsPtr ptr;
        
        // ParaVal25();
};
struct InputOrderInfo;
using InputOrderInfoPtr = std::shared_ptr<const InputOrderInfo>;

struct ChunkInfoWithAllocatedBytes : public ChunkInfo
{
    explicit ChunkInfoWithAllocatedBytes(Int64 allocated_bytes_)
        : allocated_bytes(allocated_bytes_) {}
    Int64 allocated_bytes;
};

class AggregatingInOrderTransform : public IProcessor
{
public:

    ParaVal25 pv25 = ParaVal25();
    std::vector<Param> getParaList() override{
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv25.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv25.header.columns())));
        vec.push_back(Param("max_block_size", std::to_string(pv25.max_block_size)));
        vec.push_back(Param("max_block_bytes", std::to_string(pv25.max_block_bytes)));
        vec.push_back(Param("current_variant", std::to_string(pv25.current_variant)));
        if(pv25.ptr){
            String str;
            for(const auto & key : pv25.ptr.get()->params.keys){
                str += key;
            }
            vec.push_back(Param("keys", str));
            vec.push_back(Param("keys_size", std::to_string(pv25.ptr.get()->params.keys_size)));
            vec.push_back(Param("aggregates_size", std::to_string(pv25.ptr.get()->params.aggregates_size)));
            vec.push_back(Param("max_rows_to_group_by", std::to_string(pv25.ptr.get()->params.max_rows_to_group_by)));
            vec.push_back(Param("min_free_disk_space", std::to_string(pv25.ptr.get()->params.min_free_disk_space)));
        }
        
        return vec;
    }

    AggregatingInOrderTransform(Block header, AggregatingTransformParamsPtr params,
                                const SortDescription & sort_description_for_merging,
                                const SortDescription & group_by_description_,
                                size_t max_block_size_, size_t max_block_bytes_,
                                ManyAggregatedDataPtr many_data, size_t current_variant);

    AggregatingInOrderTransform(Block header, AggregatingTransformParamsPtr params,
                                const SortDescription & sort_description_for_merging,
                                const SortDescription & group_by_description_,
                                size_t max_block_size_, size_t max_block_bytes_);

    ~AggregatingInOrderTransform() override;

    String getName() const override { return "AggregatingInOrderTransform"; }

    Status prepare() override;

    void work() override;

    void consume(Chunk chunk);

private:
    void generate();
    void finalizeCurrentChunk(Chunk chunk, size_t key_end);

    size_t max_block_size;
    size_t max_block_bytes;
    size_t cur_block_size = 0;
    size_t cur_block_bytes = 0;

    MutableColumns res_key_columns;
    MutableColumns res_aggregate_columns;

    AggregatingTransformParamsPtr params;
    ColumnsMask aggregates_mask;

    /// For sortBlock()
    SortDescription sort_description;
    SortDescriptionWithPositions group_by_description;
    bool group_by_key = false;
    Block group_by_block;
    ColumnRawPtrs key_columns_raw;

    Aggregator::AggregateColumns aggregate_columns;

    ManyAggregatedDataPtr many_data;
    AggregatedDataVariants & variants;

    UInt64 src_rows = 0;
    UInt64 src_bytes = 0;
    UInt64 res_rows = 0;

    bool need_generate = false;
    bool block_end_reached = false;
    bool is_consume_started = false;
    bool is_consume_finished = false;

    Block res_header;
    Chunk current_chunk;
    Chunk to_push_chunk;

    Poco::Logger * log = &Poco::Logger::get("AggregatingInOrderTransform");
};


class FinalizeAggregatedTransform : public ISimpleTransform
{
public:
    FinalizeAggregatedTransform(Block header, AggregatingTransformParamsPtr params_);

    void transform(Chunk & chunk) override;
    String getName() const override { return "FinalizeAggregatedTransform"; }

private:
    AggregatingTransformParamsPtr params;
    ColumnsMask aggregates_mask;
};


}
