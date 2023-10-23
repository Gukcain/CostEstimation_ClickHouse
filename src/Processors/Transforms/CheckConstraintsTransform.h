#pragma once

#include <Processors/Transforms/ExceptionKeepingTransform.h>
#include <Storages/ConstraintsDescription.h>
#include <Interpreters/StorageID.h>


namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal28{
    public:
        // T value;

        // const Block
        Block header;
        StorageID sid;

        // ParaVal28();
};
/** Check for constraints violation. If anything is found - throw an exception with detailed error message.
  * Otherwise just pass block to output unchanged.
  */

class CheckConstraintsTransform final : public ExceptionKeepingTransform
{
public:
    ParaVal28 pv28 = ParaVal28();
    std::vector<Param> getParaList() override{
        // ParaVal pv28 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv28.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv28.header.columns())));
        if (!pv28.sid.database_name.empty()){
            vec.push_back(Param("database.table",pv28.sid.getFullNameNotQuoted()));
        }

        return vec;
    }
    CheckConstraintsTransform(
            const StorageID & table_,
            const Block & header,
            const ConstraintsDescription & constraints_,
            ContextPtr context_);

    String getName() const override { return "CheckConstraintsTransform"; }

    void onConsume(Chunk chunk) override;
    GenerateResult onGenerate() override
    {
        GenerateResult res;
        res.chunk = std::move(cur_chunk);
        return res;
    }

private:
    StorageID table_id;
    const ASTs constraints_to_check;
    const ConstraintsExpressions expressions;
    size_t rows_written = 0;
    Chunk cur_chunk;
};
}
