#pragma once
#include <Processors/IProcessor.h>

namespace DB
{

/// Transform which can generate several chunks on every consumed.
/// It can be assumed that class is used in following way:
///
///    for (chunk : input_chunks)
///    {
///        transform.consume(chunk);
///
///        while (transform.canGenerate())
///        {
///            transformed_chunk = transform.generate();
///            ... (process transformed chunk)
///        }
///    }
///
class IInflatingTransform : public IProcessor
{
protected:
    InputPort & input;
    OutputPort & output;

    Chunk current_chunk;
    bool has_input = false;
    bool generated = false;
    bool can_generate = false;

    virtual void consume(Chunk chunk) = 0;
    virtual bool canGenerate() = 0;
    virtual Chunk generate() = 0;

public:
    Block in2;
    Block out2;
    IInflatingTransform(Block input_header, Block output_header);
    std::vector<Param> getParaList() override{
        // ParaVal pv73 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows_input",std::to_string(in2.rows())));
        vec.push_back(Param("colomns_input",std::to_string(in2.columns())));
        vec.push_back(Param("rows_output",std::to_string(out2.rows())));
        vec.push_back(Param("colomns_output",std::to_string(out2.columns())));
        // vec.push_back(Param("skip_empty_chunks",std::to_string(skip)));
        // vec.push_back(Param("num_outputs",std::to_string(pv73.num_outputs)));
        return vec;
    }

    Status prepare() override;
    void work() override;

    InputPort & getInputPort() { return input; }
    OutputPort & getOutputPort() { return output; }
};

}
