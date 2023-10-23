#pragma once
#include <Processors/ISimpleTransform.h>

namespace DB
{
// using namespace std;
class ParaVal40{
    public:
        // T value;

        // const Block
        Block header;
        
        // ParaVal40();
};
class ExtremesTransform : public ISimpleTransform
{

public:
    ParaVal40 pv40 = ParaVal40();
    std::vector<Param> getParaList() override{
        // ParaVal pv40 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv40.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv40.header.columns())));
        
        return vec;
    }
    explicit ExtremesTransform(const Block & header);

    String getName() const override { return "ExtremesTransform"; }

    OutputPort & getExtremesPort() { return outputs.back(); }

    Status prepare() override;
    void work() override;

protected:
    void transform(Chunk & chunk) override;

    bool finished_transform = false;
    Chunk extremes;

private:
    MutableColumns extremes_columns;
};

}

