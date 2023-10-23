#pragma once

#include <Processors/IProcessor.h>
#include <Processors/RowsBeforeLimitCounter.h>
#include <Core/SortDescription.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal70{
    public:
        // T value;

        // const Block
        Block header;
        UInt64 limit;
        UInt64 offset;
        size_t num_streams = 1;
        bool always_read_till_end = false;
        bool with_ties = false;

        // ParaVal70();
};
/// Implementation for LIMIT N OFFSET M
/// This processor support multiple inputs and outputs (the same number).
/// Each pair of input and output port works independently.
/// The reason to have multiple ports is to be able to stop all sources when limit is reached, in a query like:
///     SELECT * FROM system.numbers_mt WHERE number = 1000000 LIMIT 1
///
/// always_read_till_end - read all data from input ports even if limit was reached.
/// with_ties, description - implementation of LIMIT WITH TIES. It works only for single port.
class LimitTransform : public IProcessor
{
private:
    UInt64 limit;
    UInt64 offset;

    bool always_read_till_end;

    bool with_ties;
    const SortDescription description;

    Chunk previous_row_chunk;  /// for WITH TIES, contains only sort columns
    std::vector<size_t> sort_column_positions;

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

    Chunk makeChunkWithPreviousRow(const Chunk & current_chunk, UInt64 row_num) const;
    ColumnRawPtrs extractSortColumns(const Columns & columns) const;
    bool sortColumnsEqualAt(const ColumnRawPtrs & current_chunk_sort_columns, UInt64 current_chunk_row_num) const;

public:
    ParaVal70 pv70 = ParaVal70();
    std::vector<Param> getParaList() override{
        // ParaVal pv70 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv70.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv70.header.columns())));
        vec.push_back(Param("limit",std::to_string(pv70.limit)));
        vec.push_back(Param("offset",std::to_string(pv70.offset)));
        vec.push_back(Param("num_streams",std::to_string(pv70.num_streams)));
        vec.push_back(Param("always_read_till_end",std::to_string(pv70.always_read_till_end)));
        vec.push_back(Param("with_ties",std::to_string(pv70.with_ties)));
        return vec;
    }
    LimitTransform(
        const Block & header_, UInt64 limit_, UInt64 offset_, size_t num_streams = 1,
        bool always_read_till_end_ = false, bool with_ties_ = false,
        SortDescription description_ = {});

    String getName() const override { return "Limit"; }

    Status prepare(const PortNumbers & /*updated_input_ports*/, const PortNumbers & /*updated_output_ports*/) override;
    Status prepare() override; /// Compatibility for TreeExecutor.
    Status preparePair(PortsData & data);
    void splitChunk(PortsData & data);

    InputPort & getInputPort() { return inputs.front(); }
    OutputPort & getOutputPort() { return outputs.front(); }

    void setRowsBeforeLimitCounter(RowsBeforeLimitCounterPtr counter) { rows_before_limit_at_least.swap(counter); }
};

}
