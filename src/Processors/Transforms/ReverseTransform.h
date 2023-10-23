#pragma once
#include <Processors/ISimpleTransform.h>

namespace DB
{
// using namespace std;
class ParaVal58{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        bool skip_empty_chunks = false;
        
        // ParaVal58();
};
/// Reverse rows in chunk.
class ReverseTransform : public ISimpleTransform
{
public:
    ParaVal58 pv58 = ParaVal58();
    std::vector<Param> getParaList() override{
        // ParaVal pv58 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv58.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv58.header.columns())));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv58.skip_empty_chunks)));
        
        return vec;
    }
    explicit ReverseTransform(const Block & header) : ISimpleTransform(header, header, false) {
        pv58.header = header;
        pv58.skip_empty_chunks = false;
    }
    String getName() const override { return "ReverseTransform"; }

protected:
    void transform(Chunk & chunk) override;
};

}
