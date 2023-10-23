#pragma once
#include <Processors/ISimpleTransform.h>
#include <QueryPipeline/SizeLimits.h>
#include <Core/ColumnNumbers.h>
#include <Interpreters/SetVariants.h>

namespace DB
{
// using namespace std;
class ParaVal38{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        SizeLimits set_size_limits;
        UInt64 limit_hint;
        Names source_columns;
        bool skip_empty_chunks = false;
        
        // ParaVal38();
};
class DistinctTransform : public ISimpleTransform
{
public:
    ParaVal38 pv38 = ParaVal38();
    std::vector<Param> getParaList() override{
        // ParaVal pv38 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv38.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv38.header.columns())));
        vec.push_back(Param("limit_hint",std::to_string(pv38.limit_hint)));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv38.skip_empty_chunks)));
        String str;
        for(const auto & key : pv38.source_columns){
            str += key;
        }
        vec.push_back(Param("columns", str));
        vec.push_back(Param("size_limits-max_rows",std::to_string(pv38.set_size_limits.max_rows)));
        vec.push_back(Param("size_limits-max_bytes",std::to_string(pv38.set_size_limits.max_bytes)));
        return vec;
    }
    DistinctTransform(
        const Block & header_,
        const SizeLimits & set_size_limits_,
        UInt64 limit_hint_,
        const Names & columns_);

    String getName() const override { return "DistinctTransform"; }

protected:
    void transform(Chunk & chunk) override;

private:
    ColumnNumbers key_columns_pos;
    SetVariants data;
    Sizes key_sizes;
    const UInt64 limit_hint;

    /// Restrictions on the maximum size of the output data.
    SizeLimits set_size_limits;

    template <typename Method>
    void buildFilter(
        Method & method,
        const ColumnRawPtrs & key_columns,
        IColumn::Filter & filter,
        size_t rows,
        SetVariants & variants) const;
};

}
