#pragma once

#include <Processors/IProcessor.h>


namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal67{
    public:
        // T value;

        // const Block
        Block header;
        size_t num_inputs;

        // ParaVal67();
};
/** Has arbitrary non zero number of inputs and one output.
  * All of them have the same structure.
  *
  * Pulls all data from first input, then all data from second input, etc...
  * Doesn't do any heavy calculations.
  * Preserves an order of data.
  */
class ConcatProcessor : public IProcessor
{
public:
    ParaVal67 pv67 = ParaVal67();
    std::vector<Param> getParaList() override{
        // ParaVal pv67 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv67.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv67.header.columns())));
        vec.push_back(Param("num_inputs",std::to_string(pv67.num_inputs)));
        
        return vec;
    }
    ConcatProcessor(const Block & header, size_t num_inputs);

    String getName() const override { return "Concat"; }

    Status prepare() override;

    OutputPort & getOutputPort() { return outputs.front(); }

private:
    InputPorts::iterator current_input;
};

}

