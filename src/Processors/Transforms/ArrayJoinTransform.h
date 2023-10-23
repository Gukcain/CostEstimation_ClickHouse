#pragma once
#include <Processors/ISimpleTransform.h>

namespace DB
{
// using namespace std;
class ParaVal27{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        bool on_totals = false;
        bool skip_empty_chunks = false;
        
        // ParaVal27();
};
class ArrayJoinAction;
using ArrayJoinActionPtr = std::shared_ptr<ArrayJoinAction>;

/// Execute ARRAY JOIN
class ArrayJoinTransform : public ISimpleTransform
{
public:
    ParaVal27 pv27 = ParaVal27();
    std::vector<Param> getParaList() override{
        // ParaVal pv27 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv27.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv27.header.columns())));
        vec.push_back(Param("on_totals",std::to_string(pv27.on_totals)));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv27.skip_empty_chunks)));
        
        return vec;
    }
    ArrayJoinTransform(
            const Block & header_,
            ArrayJoinActionPtr array_join_,
            bool on_totals_ = false);

    String getName() const override { return "ArrayJoinTransform"; }

    static Block transformHeader(Block header, const ArrayJoinActionPtr & array_join);

protected:
    void transform(Chunk & chunk) override;

private:
    ArrayJoinActionPtr array_join;
};

}
