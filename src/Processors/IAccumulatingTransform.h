#pragma once

#include <Processors/IProcessor.h>


namespace DB
{

/** Has one input and one output.
  * Pulls all blocks from input, and only then produce output.
  * Examples: ORDER BY, GROUP BY.
  */
class IAccumulatingTransform : public IProcessor
{
protected:
    InputPort & input;
    OutputPort & output;

    Chunk current_input_chunk;
    Chunk current_output_chunk;
    Chunk totals;
    bool has_input = false;
    bool finished_input = false;
    bool finished_generate = false;

    virtual void consume(Chunk chunk) = 0;
    virtual Chunk generate() = 0;

    /// This method can be called once per consume call. In case if some chunks are ready.
    void setReadyChunk(Chunk chunk);
    void finishConsume() { finished_input = true; }

public:
    Block in1;
    Block out;
    IAccumulatingTransform(Block input_header, Block output_header);

    Status prepare() override;
    void work() override;
    std::vector<Param> getParaList() override{
        // ParaVal pv73 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows_input",std::to_string(in1.rows())));
        vec.push_back(Param("colomns_input",std::to_string(in1.columns())));
        vec.push_back(Param("rows_output",std::to_string(out.rows())));
        vec.push_back(Param("colomns_output",std::to_string(out.columns())));
        // vec.push_back(Param("skip_empty_chunks",std::to_string(skip)));
        // vec.push_back(Param("num_outputs",std::to_string(pv73.num_outputs)));
        return vec;
    }

    /// Adds additional port for totals.
    /// If added, totals will have been ready by the first generate() call (in totals chunk).
    InputPort * addTotalsPort();

    InputPort & getInputPort() { return input; }
    OutputPort & getOutputPort() { return output; }
};

}
