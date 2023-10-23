#pragma once
#include <cstddef>
#include <Processors/IProcessor.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal31{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        size_t num_outputs;
        
        // ParaVal31();
};
/// Transform which has single input and num_outputs outputs.
/// Read chunk from input and copy it to all outputs.
class CopyTransform : public IProcessor
{
public:
    ParaVal31 pv31 = ParaVal31();
    std::vector<Param> getParaList() override{
        // ParaVal pv31 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv31.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv31.header.columns())));
        vec.push_back(Param("num_outputs",std::to_string(pv31.num_outputs)));
        
        return vec;
    }
    CopyTransform(const Block & header, size_t num_outputs);

    String getName() const override { return "Copy"; }
    Status prepare() override;

    InputPort & getInputPort() { return inputs.front(); }

private:
    Chunk chunk;
    bool has_data = false;
    std::vector<char> was_output_processed;

    Status prepareGenerate();
    Status prepareConsume();
};

}
