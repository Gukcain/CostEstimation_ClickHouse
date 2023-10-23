#pragma once
#include <Processors/ISource.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal17{
    public:
        // T value;

        // const Block
        Block header;
        bool enable_auto_progress = true;

        // ParaVal17();
};

class NullSource : public ISource
{
public:
    
    ParaVal17 pv17 = ParaVal17();
    std::vector<Param> getParaList() override{
        // ParaVal pv17 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv17.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv17.header.columns())));
        vec.push_back(Param("enable_auto_progress",std::to_string(pv17.enable_auto_progress)));
        
        return vec;
    }
    explicit NullSource(Block header) : ISource(std::move(header)) {pv17.header = header;}
    String getName() const override { return "NullSource"; }

protected:
    Chunk generate() override { return Chunk(); }
};

}
