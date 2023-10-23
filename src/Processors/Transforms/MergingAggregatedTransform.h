#pragma once
#include <Processors/IAccumulatingTransform.h>
#include <Interpreters/Aggregator.h>
#include <Processors/Transforms/AggregatingTransform.h>

namespace DB
{
// using namespace std;
class ParaVal54{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        size_t max_threads;
        AggregatingTransformParamsPtr ptr;
        
        // ParaVal54();
};
/** A pre-aggregate stream of blocks in which each block is already aggregated.
  * Aggregate functions in blocks should not be finalized so that their states can be merged.
  */
class MergingAggregatedTransform : public IAccumulatingTransform
{
public:
    ParaVal54 pv54 = ParaVal54();
    std::vector<Param> getParaList() override{
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv54.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv54.header.columns())));
        vec.push_back(Param("max_threads", std::to_string(pv54.max_threads)));
        if(pv54.ptr){
            String str;
            for(const auto & key : pv54.ptr.get()->params.keys){
                str += key;
            }
            vec.push_back(Param("keys", str));
            vec.push_back(Param("keys_size", std::to_string(pv54.ptr.get()->params.keys_size)));
            vec.push_back(Param("aggregates_size", std::to_string(pv54.ptr.get()->params.aggregates_size)));
            vec.push_back(Param("max_rows_to_group_by", std::to_string(pv54.ptr.get()->params.max_rows_to_group_by)));
            vec.push_back(Param("min_free_disk_space", std::to_string(pv54.ptr.get()->params.min_free_disk_space)));
        }
        
        return vec;
    }
    MergingAggregatedTransform(Block header_, AggregatingTransformParamsPtr params_, size_t max_threads_);
    String getName() const override { return "MergingAggregatedTransform"; }

protected:
    void consume(Chunk chunk) override;
    Chunk generate() override;

private:
    AggregatingTransformParamsPtr params;
    Poco::Logger * log = &Poco::Logger::get("MergingAggregatedTransform");
    size_t max_threads;

    AggregatedDataVariants data_variants;
    Aggregator::BucketToBlocks bucket_to_blocks;

    UInt64 total_input_rows = 0;
    UInt64 total_input_blocks = 0;

    BlocksList blocks;
    BlocksList::iterator next_block;

    bool consume_started = false;
    bool generate_started = false;
};

}
