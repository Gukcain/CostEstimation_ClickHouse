#pragma once
#include <Processors/ISimpleTransform.h>
#include <Core/SortDescription.h>
#include <Core/InterpolateDescription.h>
#include <Interpreters/FillingRow.h>

namespace DB
{
// using namespace std;
class ParaVal41{
    public:
        // T value;

        // const Block
        Block header;
        bool on_totals;
        bool skip_empty_chunks = true;
        
        // ParaVal41();
};
/** Implements modifier WITH FILL of ORDER BY clause.
 *  It fills gaps in data stream by rows with missing values in columns with set WITH FILL and default values in other columns.
 *  Optionally FROM, TO and STEP values can be specified.
 */
class FillingTransform : public ISimpleTransform
{
public:
    ParaVal41 pv41 = ParaVal41();
    std::vector<Param> getParaList() override{
        // ParaVal pv41 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv41.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv41.header.columns())));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv41.skip_empty_chunks)));
        vec.push_back(Param("on_totals",std::to_string(pv41.on_totals)));
        return vec;
    }
    FillingTransform(const Block & header_, const SortDescription & sort_description_, InterpolateDescriptionPtr interpolate_description_, bool on_totals_);

    String getName() const override { return "FillingTransform"; }

    Status prepare() override;

    static Block transformHeader(Block header, const SortDescription & sort_description);

protected:
    void transform(Chunk & Chunk) override;

private:
    void setResultColumns(Chunk & chunk, MutableColumns & fill_columns, MutableColumns & interpolate_columns, MutableColumns & other_columns) const;
    void saveLastRow(const MutableColumns & fill_columns, const MutableColumns & interpolate_columns, const MutableColumns & other_columns);

    const SortDescription sort_description; /// Contains only columns with WITH FILL.
    const InterpolateDescriptionPtr interpolate_description; /// Contains INTERPOLATE columns
    const bool on_totals; /// FillingTransform does nothing on totals.

    FillingRow filling_row; /// Current row, which is used to fill gaps.
    FillingRow next_row; /// Row to which we need to generate filling rows.

    using Positions = std::vector<size_t>;
    Positions fill_column_positions;
    Positions interpolate_column_positions;
    Positions other_column_positions;
    std::vector<std::pair<size_t, NameAndTypePair>> input_positions; /// positions in result columns required for actions
    ExpressionActionsPtr interpolate_actions;
    bool first = true;
    bool generate_suffix = false;

    Columns last_row;

    /// Determines should we insert filling row before start generating next rows.
    bool should_insert_first = false;
};

}
