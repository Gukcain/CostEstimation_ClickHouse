#pragma once
#include <Processors/ISimpleTransform.h>
#include <Processors/RowsBeforeLimitCounter.h>
#include <Core/SortDescription.h>
#include <Common/PODArray.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal56{
    public:
        // T value;

        // const Block
        Block header;
        UInt64 limit = 0;

        // ParaVal56();
};
/** Sorts each block individually by the values of the specified columns.
  */
class PartialSortingTransform : public ISimpleTransform
{
public:
    ParaVal56 pv56 = ParaVal56();
    std::vector<Param> getParaList() override{
        // ParaVal pv56 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv56.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv56.header.columns())));
        vec.push_back(Param("limit",std::to_string(pv56.limit)));
        
        return vec;
    }
    /// limit - if not 0, then you can sort each block not completely, but only `limit` first rows by order.
    PartialSortingTransform(
        const Block & header_,
        const SortDescription & description_,
        UInt64 limit_ = 0);

    String getName() const override { return "PartialSortingTransform"; }

    void setRowsBeforeLimitCounter(RowsBeforeLimitCounterPtr counter) { read_rows.swap(counter); }

protected:
    void transform(Chunk & chunk) override;

private:
    const SortDescription description;
    SortDescriptionWithPositions description_with_positions;
    const UInt64 limit;
    RowsBeforeLimitCounterPtr read_rows;

    Columns sort_description_threshold_columns;

    /// This are just buffers which reserve memory to reduce the number of allocations.
    PaddedPODArray<UInt64> rows_to_compare;
    PaddedPODArray<Int8> compare_results;
    IColumn::Filter filter;

    /// If limit < min_limit_for_partial_sort_optimization, skip optimization with threshold_block.
    /// Because for small LIMIT partial sorting may be very faster then additional work
    /// which is made if optimization is enabled (comparison with threshold, filtering).
    static constexpr size_t min_limit_for_partial_sort_optimization = 1500;
};

}
