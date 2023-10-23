#pragma once

#include <Processors/IProcessor.h>
#include <Interpreters/SetVariants.h>
#include <Core/ColumnNumbers.h>
#include <Parsers/ASTSelectIntersectExceptQuery.h>


namespace DB
{
// using namespace std;

class IntersectOrExceptTransform : public IProcessor
{
using Operator = ASTSelectIntersectExceptQuery::Operator;
class ParaVal45{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        Operator op;
        
        // ParaVal45();
};
public:
    ParaVal45 pv45 = ParaVal45();
    std::vector<Param> getParaList() override{
        // ParaVal pv45 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv45.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv45.header.columns())));
        String str;
        if(pv45.op==Operator::UNKNOWN){
            str = "UNKNOWN";
        }else if(pv45.op==Operator::EXCEPT_ALL){
            str = "EXCEPT_ALL";
        }else if(pv45.op==Operator::INTERSECT_ALL){
            str = "INTERSECT_ALL";
        }else if(pv45.op==Operator::EXCEPT_DISTINCT){
            str = "EXCEPT_DINSTINCT";
        }else if(pv45.op==Operator::INTERSECT_DISTINCT){
            str = "INTERSECT_DISTINCT";
        }
        vec.push_back(Param("on_totals",str));
        
        return vec;
    }
    IntersectOrExceptTransform(const Block & header_, Operator operator_);

    String getName() const override { return "IntersectOrExcept"; }

protected:
    Status prepare() override;

    void work() override;

private:
    Operator current_operator;

    ColumnNumbers key_columns_pos;
    std::optional<SetVariants> data;
    Sizes key_sizes;

    Chunk current_input_chunk;
    Chunk current_output_chunk;

    bool finished_second_input = false;
    bool has_input = false;

    void accumulate(Chunk chunk);

    void filter(Chunk & chunk);

    template <typename Method>
    void addToSet(Method & method, const ColumnRawPtrs & key_columns, size_t rows, SetVariants & variants) const;

    template <typename Method>
    size_t buildFilter(Method & method, const ColumnRawPtrs & columns,
        IColumn::Filter & filter, size_t rows, SetVariants & variants) const;
};

}
