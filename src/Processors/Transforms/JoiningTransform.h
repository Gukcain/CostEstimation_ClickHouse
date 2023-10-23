#pragma once
#include <Processors/IProcessor.h>
#include "Interpreters/IJoin.h"


namespace DB
{
// using namespace std;

class IJoin;
using JoinPtr = std::shared_ptr<IJoin>;

class NotJoinedBlocks;
class IBlocksStream;
using IBlocksStreamPtr = std::shared_ptr<IBlocksStream>;
class ParaVal46{
    public:
        // T value;

        // const Block
        Block input_header;
        Block output_header;
        JoinPtr join;
        size_t max_block_size;
        bool on_totals = false;
        bool default_totals = false;
        size_t num_streams;
        
        // ParaVal46();
};
/// Join rows to chunk form left table.
/// This transform usually has two input ports and one output.
/// First input is for data from left table.
/// Second input has empty header and is connected with FillingRightJoinSide.
/// We can process left table only when Join is filled. Second input is used to signal that FillingRightJoinSide is finished.
class JoiningTransform : public IProcessor
{
public:

    /// Count streams and check which is last.
    /// The last one should process non-joined rows.
    class FinishCounter
    {
    public:
        explicit FinishCounter(size_t total_) : total(total_) {}

        bool isLast()
        {
            return finished.fetch_add(1) + 1 >= total;
        }

    private:
        const size_t total;
        std::atomic<size_t> finished{0};
    };

    using FinishCounterPtr = std::shared_ptr<FinishCounter>;

    ParaVal46 pv461 = ParaVal46();
    std::vector<Param> getParaList() override{
        // ParaVal pv461 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows_input",std::to_string(pv461.input_header.rows())));
        vec.push_back(Param("colomns_input",std::to_string(pv461.input_header.columns())));
        vec.push_back(Param("rows_output",std::to_string(pv461.output_header.rows())));
        vec.push_back(Param("colomns_output",std::to_string(pv461.output_header.columns())));
        vec.push_back(Param("default_totals",std::to_string(pv461.default_totals)));
        vec.push_back(Param("on_totals", std::to_string(pv461.on_totals)));
        vec.push_back(Param("max_block_size",std::to_string(pv461.max_block_size)));
        if(join){
            vec.push_back(Param("total_row_count_in_memory",std::to_string(join.get()->getTotalRowCount())));
            vec.push_back(Param("total_byte_count_in_memory",std::to_string(join.get()->getTotalByteCount())));
            String str;
            if(join.get()->pipelineType()==JoinPipelineType::FilledRight){
                str = "FilledRight";
            }else if(join.get()->pipelineType()==JoinPipelineType::FillRightFirst){
                str = "FillRightFirst";
            }else if(join.get()->pipelineType()==JoinPipelineType::YShaped){
                str = "YShaped";
            }
            vec.push_back(Param("joinPipelineType",str));
        }
        
        
        return vec;
    }
    JoiningTransform(
        const Block & input_header,
        const Block & output_header,
        JoinPtr join_,
        size_t max_block_size_,
        bool on_totals_ = false,
        bool default_totals_ = false,
        FinishCounterPtr finish_counter_ = nullptr);

    ~JoiningTransform() override;

    String getName() const override { return "JoiningTransform"; }

    static Block transformHeader(Block header, const JoinPtr & join);

    OutputPort & getFinishedSignal();

    Status prepare() override;
    void work() override;

protected:
    void transform(Chunk & chunk);

private:
    Chunk input_chunk;
    Chunk output_chunk;
    bool has_input = false;
    bool has_output = false;
    bool stop_reading = false;
    bool process_non_joined = true;

    JoinPtr join;
    bool on_totals;
    /// This flag means that we have manually added totals to our pipeline.
    /// It may happen in case if joined subquery has totals, but out string doesn't.
    /// We need to join default values with subquery totals if we have them, or return empty chunk is haven't.
    bool default_totals;
    bool initialized = false;

    ExtraBlockPtr not_processed;

    FinishCounterPtr finish_counter;
    IBlocksStreamPtr non_joined_blocks;
    size_t max_block_size;

    Block readExecute(Chunk & chunk);
};

/// Fills Join with block from right table.
/// Has single input and single output port.
/// Output port has empty header. It is closed when all data is inserted in join.
class FillingRightJoinSideTransform : public IProcessor
{
public:
    ParaVal46 pv462 = ParaVal46();
    std::vector<Param> getParaList() override{
        // ParaVal pv462 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows_input",std::to_string(pv462.input_header.rows())));
        vec.push_back(Param("colomns_input",std::to_string(pv462.input_header.columns())));
        if(pv462.join){
            vec.push_back(Param("total_row_count_in_memory",std::to_string(pv462.join.get()->getTotalRowCount())));
            vec.push_back(Param("total_byte_count_in_memory",std::to_string(pv462.join.get()->getTotalByteCount())));
            String str;
            if(pv462.join.get()->pipelineType()==JoinPipelineType::FilledRight){
                str = "FilledRight";
            }else if(pv462.join.get()->pipelineType()==JoinPipelineType::FillRightFirst){
                str = "FillRightFirst";
            }else if(pv462.join.get()->pipelineType()==JoinPipelineType::YShaped){
                str = "YShaped";
            }
            vec.push_back(Param("joinPipelineType",str));
        }
        
        
        
        return vec;
    }
    FillingRightJoinSideTransform(Block input_header, JoinPtr join_);
    String getName() const override { return "FillingRightJoinSide"; }

    InputPort * addTotalsPort();

    Status prepare() override;
    void work() override;

private:
    JoinPtr join;
    Chunk chunk;
    bool stop_reading = false;
    bool for_totals = false;
    bool set_totals = false;
};


class DelayedBlocksTask : public ChunkInfo
{
public:

    explicit DelayedBlocksTask() : finished(true) {}
    explicit DelayedBlocksTask(IBlocksStreamPtr delayed_blocks_) : delayed_blocks(std::move(delayed_blocks_)) {}

    IBlocksStreamPtr delayed_blocks = nullptr;

    bool finished = false;
};

using DelayedBlocksTaskPtr = std::shared_ptr<const DelayedBlocksTask>;


/// Reads delayed joined blocks from Join
class DelayedJoinedBlocksTransform : public IProcessor
{
public:
    ParaVal46 pv463 = ParaVal46();
    std::vector<Param> getParaList() override{
        // ParaVal pv463 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("num_streams",std::to_string(pv463.num_streams)));
        if(pv463.join){
            vec.push_back(Param("total_row_count_in_memory",std::to_string(pv463.join.get()->getTotalRowCount())));
            vec.push_back(Param("total_byte_count_in_memory",std::to_string(pv463.join.get()->getTotalByteCount())));
            String str;
            if(pv463.join.get()->pipelineType()==JoinPipelineType::FilledRight){
                str = "FilledRight";
            }else if(pv463.join.get()->pipelineType()==JoinPipelineType::FillRightFirst){
                str = "FillRightFirst";
            }else if(pv463.join.get()->pipelineType()==JoinPipelineType::YShaped){
                str = "YShaped";
            }
            vec.push_back(Param("joinPipelineType",str));
        }
        
        
        
        return vec;
    }
    explicit DelayedJoinedBlocksTransform(size_t num_streams, JoinPtr join_);

    String getName() const override { return "DelayedJoinedBlocksTransform"; }

    Status prepare() override;
    void work() override;

private:
    JoinPtr join;

    IBlocksStreamPtr delayed_blocks = nullptr;
    bool finished = false;
};

class DelayedJoinedBlocksWorkerTransform : public IProcessor
{
public:
    explicit DelayedJoinedBlocksWorkerTransform(Block output_header);

    String getName() const override { return "DelayedJoinedBlocksWorkerTransform"; }

    Status prepare() override;
    void work() override;

private:
    DelayedBlocksTaskPtr task;
    Chunk output_chunk;

    bool finished = false;
};

}
