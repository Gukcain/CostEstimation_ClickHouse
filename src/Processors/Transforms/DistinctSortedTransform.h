#pragma once

#include <Processors/ISimpleTransform.h>
#include <Interpreters/SetVariants.h>
#include <Core/SortDescription.h>
#include <Core/ColumnNumbers.h>


namespace DB
{
// using namespace std;
class ParaVal37{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        SizeLimits set_size_limits;
        UInt64 limit_hint;
        Names source_columns;
        bool skip_empty_chunks = false;
        
        // ParaVal37();
};
/** This class is intended for implementation of SELECT DISTINCT clause and
  * leaves only unique rows in the stream.
  *
  * DistinctSortedTransform::isApplicable() have to be used to check if DistinctSortedTransform can be constructed with particular arguments,
  * otherwise the constructor can throw LOGICAL_ERROR exception
  *
  * Implementation for case, when input stream has rows for same DISTINCT key or at least its prefix,
  *  grouped together (going consecutively).
  *
  * To optimize the SELECT DISTINCT ... LIMIT clause we can
  * set limit_hint to non zero value. So we stop emitting new rows after
  * count of already emitted rows will reach the limit_hint.
  */
class DistinctSortedTransform : public ISimpleTransform
{
public:
    ParaVal37 pv37 = ParaVal37();
    std::vector<Param> getParaList() override{
        // ParaVal pv37 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv37.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv37.header.columns())));
        vec.push_back(Param("limit_hint",std::to_string(pv37.limit_hint)));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv37.skip_empty_chunks)));
        String str;
        for(const auto & key : pv37.source_columns){
            str += key;
        }
        vec.push_back(Param("column_names", str));
        vec.push_back(Param("size_limits-max_rows",std::to_string(pv37.set_size_limits.max_rows)));
        vec.push_back(Param("size_limits-max_bytes",std::to_string(pv37.set_size_limits.max_bytes)));
        return vec;
    }
    /// Empty columns_ means all columns.
    DistinctSortedTransform(
        const Block & header,
        const SortDescription & sort_description,
        const SizeLimits & set_size_limits_,
        UInt64 limit_hint_,
        const Names & column_names);

    String getName() const override { return "DistinctSortedTransform"; }

    static bool isApplicable(const Block & header, const SortDescription & sort_description, const Names & column_names);

protected:
    void transform(Chunk & chunk) override;

private:
    static bool rowsEqual(const ColumnRawPtrs & lhs, size_t n, const ColumnRawPtrs & rhs, size_t m);

    /// return true if has new data
    template <typename Method>
    bool buildFilter(
        Method & method,
        const ColumnRawPtrs & key_columns,
        const ColumnRawPtrs & clearing_hint_columns,
        IColumn::Filter & filter,
        size_t rows,
        ClearableSetVariants & variants) const;

    struct PreviousChunk
    {
        Chunk chunk;
        ColumnRawPtrs clearing_hint_columns;
    };
    PreviousChunk prev_chunk;

    ColumnNumbers column_positions;      /// DISTINCT columns positions in header
    ColumnNumbers sort_prefix_positions; /// DISTINCT columns positions which form sort prefix of sort description
    ColumnRawPtrs column_ptrs;           /// DISTINCT columns from chunk
    ColumnRawPtrs sort_prefix_columns;   /// DISTINCT columns from chunk which form sort prefix of sort description

    ClearableSetVariants data;
    Sizes key_sizes;
    UInt64 limit_hint;

    /// Restrictions on the maximum size of the output data.
    SizeLimits set_size_limits;
};

}
