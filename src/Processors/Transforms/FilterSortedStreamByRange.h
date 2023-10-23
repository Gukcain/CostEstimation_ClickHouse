#pragma once

#include <Interpreters/ExpressionActions.h>
#include <Processors/ISimpleTransform.h>
#include <Processors/Transforms/ExpressionTransform.h>
#include <Processors/Transforms/FilterTransform.h>

namespace DB
{
// using namespace std;
class ParaVal42{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        
        bool skip_empty_chunks = true;
        String filter_column_name;
        bool remove_filter_column;
        bool on_totals = false;
        
        // ParaVal42();
};
/// Could be used when the predicate given by expression_ is true only on the one continuous range of input rows.
/// The following optimization applies: when a new chunk of data comes in, we firstly execute the expression_ only on the first and the last row -
/// if it evaluates to true on both rows then the whole chunk is immediately passed to further steps.
/// Otherwise, we apply the expression_ to all rows.
class FilterSortedStreamByRange : public ISimpleTransform
{
public:
    ParaVal42 pv42 = ParaVal42();
    std::vector<Param> getParaList() override{
        // ParaVal pv42 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv42.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv42.header.columns())));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv42.skip_empty_chunks)));
        vec.push_back(Param("filter_column_name", pv42.filter_column_name));
        vec.push_back(Param("remove_filter_column",std::to_string(pv42.remove_filter_column)));
        vec.push_back(Param("on_totals",std::to_string(pv42.on_totals)));
        return vec;
    }
    FilterSortedStreamByRange(
        const Block & header_,
        ExpressionActionsPtr expression_,
        String filter_column_name_,
        bool remove_filter_column_,
        bool on_totals_ = false)
        : ISimpleTransform(
            header_,
            FilterTransform::transformHeader(header_, &expression_->getActionsDAG(), filter_column_name_, remove_filter_column_),
            true)
        , filter_transform(header_, expression_, filter_column_name_, remove_filter_column_, on_totals_)
    {
        pv42.header = header_;
        pv42.on_totals = on_totals_;
        pv42.filter_column_name = filter_column_name_;
        pv42.remove_filter_column = remove_filter_column_;
        pv42.skip_empty_chunks = true;
    }

    String getName() const override { return "FilterSortedStreamByRange"; }

    void transform(Chunk & chunk) override
    {
        const UInt64 rows_before_filtration = chunk.getNumRows();
        if (rows_before_filtration < 2)
        {
            filter_transform.transform(chunk);
            return;
        }

        // Evaluate expression on just the first and the last row.
        // If both of them satisfies conditions, then skip calculation for all the rows in between.
        auto quick_check_columns = chunk.cloneEmptyColumns();
        auto src_columns = chunk.detachColumns();
        for (auto row : {static_cast<UInt64>(0), rows_before_filtration - 1})
            for (size_t col = 0; col < quick_check_columns.size(); ++col)
                quick_check_columns[col]->insertFrom(*src_columns[col].get(), row);
        chunk.setColumns(std::move(quick_check_columns), 2);
        filter_transform.transform(chunk);
        const bool all_rows_will_pass_filter = chunk.getNumRows() == 2;

        chunk.setColumns(std::move(src_columns), rows_before_filtration);

        // Not all rows satisfy conditions.
        if (!all_rows_will_pass_filter)
            filter_transform.transform(chunk);
    }

private:
    FilterTransform filter_transform;
};


}
