#pragma once
#include <Processors/IInflatingTransform.h>
#include <Processors/Transforms/AggregatingTransform.h>
#include <Processors/Transforms/RollupTransform.h>
#include <Processors/Transforms/finalizeChunk.h>


namespace DB
{

// using namespace std;
// template <typename T>
class ParaVal35{
    public:
        // T value;

        // const Block
        Block header;
        bool use_nulls;

        // ParaVal35();
};

/// Takes blocks after grouping, with non-finalized aggregate functions.
/// Calculates all subsets of columns and aggregates over them.
class CubeTransform : public GroupByModifierTransform
{
    
public:
    ParaVal35 pv35 = ParaVal35();
    CubeTransform(Block header, AggregatingTransformParamsPtr params, bool use_nulls_);
    String getName() const override { return "CubeTransform"; }

    std::vector<Param> getParaList() override{
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv35.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv35.header.columns())));
        vec.push_back(Param("use_nulls",std::to_string(pv35.use_nulls)));
        return vec;
    }

protected:
    Chunk generate() override;

private:
    const ColumnsMask aggregates_mask;

    Columns current_columns;
    Columns current_zero_columns;

    UInt64 mask = 0;
    UInt64 grouping_set = 0;
};

}
