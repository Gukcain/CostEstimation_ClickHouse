#pragma once
#include <Processors/Transforms/ExceptionKeepingTransform.h>
#include <Processors/ISimpleTransform.h>

namespace DB
{
// using namespace std;
class ParaVal39{
    public:
        // T value;

        // const Block
        Block header;
        
        // ParaVal39();
};
class ExpressionActions;
using ExpressionActionsPtr = std::shared_ptr<ExpressionActions>;

class ActionsDAG;

/** Executes a certain expression over the block.
  * The expression consists of column identifiers from the block, constants, common functions.
  * For example: hits * 2 + 3, url LIKE '%clickhouse%'
  * The expression processes each row independently of the others.
  */
class ExpressionTransform final : public ISimpleTransform
{
public:
    ParaVal39 pv39 = ParaVal39();
    std::vector<Param> getParaList() override{
        // ParaVal pv39 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv39.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv39.header.columns())));
        
        return vec;
    }
    ExpressionTransform(
            const Block & header_,
            ExpressionActionsPtr expression_);

    String getName() const override { return "ExpressionTransform"; }

    static Block transformHeader(Block header, const ActionsDAG & expression);

protected:
    void transform(Chunk & chunk) override;

private:
    ExpressionActionsPtr expression;
};

class ConvertingTransform final : public ExceptionKeepingTransform
{
public:
    ParaVal39 pv392 = ParaVal39();
    std::vector<Param> getParaList() override{
        // ParaVal pv39 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv392.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv392.header.columns())));
        
        return vec;
    }
    ConvertingTransform(
            const Block & header_,
            ExpressionActionsPtr expression_);

    String getName() const override { return "ConvertingTransform"; }

protected:
    void onConsume(Chunk chunk) override;
    GenerateResult onGenerate() override
    {
        GenerateResult res;
        res.chunk = std::move(cur_chunk);
        return res;
    }

private:
    ExpressionActionsPtr expression;
    Chunk cur_chunk;
};

}
