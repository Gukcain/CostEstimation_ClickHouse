#pragma once
#include <Processors/ISink.h>

namespace DB
{
    // using namespace std;
// template <typename T>
class ParaVal10{
    public:
        // T value;

        // const Block
        Block header;

        // ParaVal10();
};

/// Sink which closes input port and reads nothing.
class NullSink : public ISink
{
public:
    ParaVal10 pv10 = ParaVal10();
    explicit NullSink(Block header) : ISink(std::move(header)) {pv10.header = header;}
    String getName() const override { return "NullSink"; }
    std::vector<Param> getParaList() override{
        // ParaVal pv = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv10.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv10.header.columns())));
        return vec;
    }

    Status prepare() override
    {
        input.close();
        return Status::Finished;
    }
protected:
    void consume(Chunk) override {}
};

}
