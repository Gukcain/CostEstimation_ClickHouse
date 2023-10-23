#pragma once
#include <Processors/ISink.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal9{
    public:
        // T value;

        // const Block
        Block header;

        // ParaVal9();
};

/// Sink which reads everything and do nothing with it.
class EmptySink : public ISink
{
public:
    ParaVal9 pv9 = ParaVal9();
    std::vector<Param> getParaList() override{
        // ParaVal pv9 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv9.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv9.header.columns())));
        return vec;
    }
    explicit EmptySink(Block header) : ISink(std::move(header)) {pv9.header = header;}
    
    String getName() const override { return "EmptySink"; }

protected:
    void consume(Chunk) override {}
};

}
