#pragma once
#include <memory>
#include <Core/ColumnNumbers.h>
#include <Processors/IAccumulatingTransform.h>
#include <Processors/Transforms/AggregatingTransform.h>
#include <Processors/Transforms/finalizeChunk.h>

namespace DB
{
// using namespace std;
class ParaVal59{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        bool use_nulls;
        AggregatingTransformParamsPtr ptr;
        
        // ParaVal59();
};
struct GroupByModifierTransform : public IAccumulatingTransform
{
    ParaVal59 pv59 = ParaVal59();
    std::vector<Param> getParaList() override{
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv59.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv59.header.columns())));
        vec.push_back(Param("use_nulls", std::to_string(pv59.use_nulls)));
        if(pv59.ptr){
            String str;
            for(const auto & key : pv59.ptr.get()->params.keys){
                str += key;
            }
            vec.push_back(Param("keys", str));
            vec.push_back(Param("keys_size", std::to_string(pv59.ptr.get()->params.keys_size)));
            vec.push_back(Param("aggregates_size", std::to_string(pv59.ptr.get()->params.aggregates_size)));
            vec.push_back(Param("max_rows_to_group_by", std::to_string(pv59.ptr.get()->params.max_rows_to_group_by)));
            vec.push_back(Param("min_free_disk_space", std::to_string(pv59.ptr.get()->params.min_free_disk_space)));
        }
        
        return vec;
    }
    GroupByModifierTransform(Block header, AggregatingTransformParamsPtr params_, bool use_nulls_);

protected:
    void consume(Chunk chunk) override;

    void mergeConsumed();

    Chunk merge(Chunks && chunks, bool is_input, bool final);

    MutableColumnPtr getColumnWithDefaults(size_t key, size_t n) const;

    AggregatingTransformParamsPtr params;

    bool use_nulls;

    ColumnNumbers keys;

    std::unique_ptr<Aggregator> output_aggregator;

    Block intermediate_header;

    Chunks consumed_chunks;
    Chunk current_chunk;
};

/// Takes blocks after grouping, with non-finalized aggregate functions.
/// Calculates subtotals and grand totals values for a set of columns.
class RollupTransform : public GroupByModifierTransform
{
public:
    std::vector<Param> getParaList() override{
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv59.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv59.header.columns())));
        vec.push_back(Param("use_nulls", std::to_string(pv59.use_nulls)));
        if(pv59.ptr){
            String str;
            for(const auto & key : pv59.ptr.get()->params.keys){
                str += key;
            }
            vec.push_back(Param("keys", str));
            vec.push_back(Param("keys_size", std::to_string(pv59.ptr.get()->params.keys_size)));
            vec.push_back(Param("aggregates_size", std::to_string(pv59.ptr.get()->params.aggregates_size)));
            vec.push_back(Param("max_rows_to_group_by", std::to_string(pv59.ptr.get()->params.max_rows_to_group_by)));
            vec.push_back(Param("min_free_disk_space", std::to_string(pv59.ptr.get()->params.min_free_disk_space)));
        }
        return vec;
    }
    RollupTransform(Block header, AggregatingTransformParamsPtr params, bool use_nulls_);
    String getName() const override { return "RollupTransform"; }

protected:
    Chunk generate() override;

private:
    const ColumnsMask aggregates_mask;

    size_t last_removed_key = 0;
    size_t set_counter = 0;
};

}
