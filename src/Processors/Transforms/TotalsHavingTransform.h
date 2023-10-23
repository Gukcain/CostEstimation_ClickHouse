#pragma once

#include <Processors/ISimpleTransform.h>
#include <Processors/Transforms/finalizeChunk.h>
#include <Common/Arena.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal62{
    public:
        // T value;

        // const Block
        Block header;
        // bool enable_auto_progress = true;
        bool overflow_row;
        String filter_column;
        bool remove_filter;
        TotalsMode totals_mode;
        double auto_include_threshold;
        bool final;
        bool skip_empty_chunks = true;

        // ParaVal62();
};
class Arena;
using ArenaPtr = std::shared_ptr<Arena>;

class ExpressionActions;
using ExpressionActionsPtr = std::shared_ptr<ExpressionActions>;

class ActionsDAG;

enum class TotalsMode;

/** Takes blocks after grouping, with non-finalized aggregate functions.
  * Calculates total values according to totals_mode.
  * If necessary, evaluates the expression from HAVING and filters rows. Returns the finalized and filtered blocks.
  */
class TotalsHavingTransform : public ISimpleTransform
{
public:
    ParaVal62 pv62 = ParaVal62();
    std::vector<Param> getParaList() override{
        // ParaVal pv62 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv62.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv62.header.columns())));
        // vec.push_back(Param("enable_auto_progress",to_string(pv62.enable_auto_progress)));
        vec.push_back(Param("auto_include_threshold",std::to_string(pv62.auto_include_threshold)));
        vec.push_back(Param("enable_auto_progress",pv62.filter_column));
        vec.push_back(Param("remove_filter",std::to_string(pv62.remove_filter)));
        String str;
        if(totals_mode == TotalsMode::AFTER_HAVING_AUTO){
            str = "AFTER_HAVING_AUTO";
        }else if(totals_mode == TotalsMode::AFTER_HAVING_INCLUSIVE){
            str = "AFTER_HAVING_INCLUSIVE";
        }else if(totals_mode == TotalsMode::AFTER_HAVING_EXCLUSIVE){
            str = "AFTER_HAVING_EXCLUSIVE";
        }else if(totals_mode == TotalsMode::BEFORE_HAVING){
            str = "BEFORE_HAVING";
        }
        vec.push_back(Param("totals_mode",str));
        vec.push_back(Param("final",std::to_string(pv62.final)));
        vec.push_back(Param("overflow_row",std::to_string(pv62.overflow_row)));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv62.skip_empty_chunks)));
        return vec;
    }
    TotalsHavingTransform(
        const Block & header,
        const ColumnsMask & aggregates_mask_,
        bool overflow_row_,
        const ExpressionActionsPtr & expression_,
        const std::string & filter_column_,
        bool remove_filter_,
        TotalsMode totals_mode_,
        double auto_include_threshold_,
        bool final_);

    String getName() const override { return "TotalsHavingTransform"; }

    OutputPort & getTotalsPort() { return outputs.back(); }

    Status prepare() override;
    void work() override;

    static Block transformHeader(Block block, const ActionsDAG * expression, const std::string & filter_column_name, bool remove_filter, bool final, const ColumnsMask & aggregates_mask);

protected:
    void transform(Chunk & chunk) override;

    bool finished_transform = false;
    bool total_prepared = false;
    Chunk totals;

private:
    void addToTotals(const Chunk & chunk, const IColumn::Filter * filter);
    void prepareTotals();

    /// Params
    const ColumnsMask aggregates_mask;
    bool overflow_row;
    ExpressionActionsPtr expression;
    String filter_column_name;
    bool remove_filter;
    TotalsMode totals_mode;
    double auto_include_threshold;
    bool final;

    size_t passed_keys = 0;
    size_t total_keys = 0;

    size_t filter_column_pos = 0;

    Block finalized_header;

    /// Here are the values that did not pass max_rows_to_group_by.
    /// They are added or not added to the current_totals, depending on the totals_mode.
    Chunk overflow_aggregates;

    /// Here, total values are accumulated. After the work is finished, they will be placed in totals.
    MutableColumns current_totals;
};

}
