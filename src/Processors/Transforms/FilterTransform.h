#pragma once
#include <Processors/ISimpleTransform.h>
#include <Columns/FilterDescription.h>

namespace DB
{
// using namespace std;
class ParaVal43{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        
        bool skip_empty_chunks = true;
        String filter_column_name;
        bool remove_filter_column;
        bool on_totals = false;
        
        // ParaVal43();
};
class ExpressionActions;
using ExpressionActionsPtr = std::shared_ptr<ExpressionActions>;

class ActionsDAG;

/** Implements WHERE, HAVING operations.
  * Takes an expression, which adds to the block one ColumnUInt8 column containing the filtering conditions.
  * The expression is evaluated and result chunks contain only the filtered rows.
  * If remove_filter_column is true, remove filter column from block.
  */
class FilterTransform : public ISimpleTransform
{
public:
    ParaVal43 pv43 = ParaVal43();
    std::vector<Param> getParaList() override{
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv43.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv43.header.columns())));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv43.skip_empty_chunks)));
        vec.push_back(Param("filter_column_name", filter_column_name));
        vec.push_back(Param("remove_filter_column",std::to_string(pv43.remove_filter_column)));
        vec.push_back(Param("on_totals",std::to_string(pv43.on_totals)));
        return vec;
    }
    FilterTransform(
        const Block & header_, ExpressionActionsPtr expression_, String filter_column_name_,
        bool remove_filter_column_, bool on_totals_ = false, std::shared_ptr<std::atomic<size_t>> rows_filtered_ = nullptr);

    static Block transformHeader(
            Block header,
            const ActionsDAG * expression,
            const String & filter_column_name,
            bool remove_filter_column);

    String getName() const override { return "FilterTransform"; }

    Status prepare() override;

    void transform(Chunk & chunk) override;

private:
    ExpressionActionsPtr expression;
    String filter_column_name;
    bool remove_filter_column;
    bool on_totals;

    ConstantFilterDescription constant_filter_description;
    size_t filter_column_position = 0;

    std::shared_ptr<std::atomic<size_t>> rows_filtered;

    /// Header after expression, but before removing filter column.
    Block transformed_header;

    bool are_prepared_sets_initialized = false;

    void doTransform(Chunk & chunk);
    void removeFilterIfNeed(Chunk & chunk) const;
};

}
