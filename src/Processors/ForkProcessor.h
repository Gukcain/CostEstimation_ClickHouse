#pragma once

#include <Processors/IProcessor.h>


namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal69{
    public:
        // T value;

        // const Block
        Block header;
        size_t num_ports;

        // ParaVal69();
};
/** Has one input and arbitrary non zero number of outputs.
  * All of them have the same structure.
  *
  * Pulls data input and copies it to every output.
  * You may have heard about it under the name 'tee'.
  *
  * Doesn't do any heavy calculations.
  * Preserves an order of data.
  */
class ForkProcessor : public IProcessor
{
public:
    ParaVal69 pv69 = ParaVal69();
    std::vector<Param> getParaList() override{
        // ParaVal pv69 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv69.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv69.header.columns())));
        vec.push_back(Param("num_outputs",std::to_string(pv69.num_ports)));
        return vec;
    }
    ForkProcessor(const Block & header, size_t num_outputs)
        : IProcessor(InputPorts{header}, OutputPorts(num_outputs, header))
    {
      pv69.header = header;
      pv69.num_ports = num_outputs;
    }

    String getName() const override { return "Fork"; }

    Status prepare() override;

    InputPort & getInputPort() { return inputs.front(); }
};

}


