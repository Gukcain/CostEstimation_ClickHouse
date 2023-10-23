#pragma once

#include <Processors/IProcessor.h>
#include <Processors/RowsBeforeLimitCounter.h>
#include <Core/SortDescription.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal71{
    public:
        // T value;

        // const Block
        Block header;
        UInt64 offset;
        size_t num_streams = 1;

        // ParaVal71();
};
/// Implementation for OFFSET N (without limit)
/// This processor support multiple inputs and outputs (the same number).
/// Each pair of input and output port works independently.
class OffsetTransform : public IProcessor
{
private:
    UInt64 offset;
    UInt64 rows_read = 0; /// including the last read block

    RowsBeforeLimitCounterPtr rows_before_limit_at_least;

    /// State of port's pair.
    /// Chunks from different port pairs are not mixed for better cache locality.
    struct PortsData
    {
        Chunk current_chunk;

        InputPort * input_port = nullptr;
        OutputPort * output_port = nullptr;
        bool is_finished = false;
    };

    std::vector<PortsData> ports_data;
    size_t num_finished_port_pairs = 0;

public:
    ParaVal71 pv71 = ParaVal71();
    std::vector<Param> getParaList() override{
        // ParaVal pv71 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv71.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv71.header.columns())));
        vec.push_back(Param("offset",std::to_string(pv71.offset)));
        vec.push_back(Param("num_streams",std::to_string(pv71.num_streams)));
        
        return vec;
    }
    OffsetTransform(const Block & header_, UInt64 offset_, size_t num_streams = 1);

    String getName() const override { return "Offset"; }

    Status prepare(const PortNumbers & /*updated_input_ports*/, const PortNumbers & /*updated_output_ports*/) override;
    Status prepare() override; /// Compatibility for TreeExecutor.
    Status preparePair(PortsData & data);
    void splitChunk(PortsData & data) const;

    InputPort & getInputPort() { return inputs.front(); }
    OutputPort & getOutputPort() { return outputs.front(); }

    void setRowsBeforeLimitCounter(RowsBeforeLimitCounterPtr counter) { rows_before_limit_at_least.swap(counter); }
};

}
