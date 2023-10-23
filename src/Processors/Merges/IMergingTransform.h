#pragma once

#include <Processors/Merges/Algorithms/IMergingAlgorithm.h>
#include <Processors/IProcessor.h>
#include <Common/Stopwatch.h>
#include "Core/Types.h"
#include "base/types.h"

namespace DB
{

/// Base class for IMergingTransform.
/// It is needed to extract all non-template methods in single translation unit.
class IMergingTransformBase : public IProcessor
{
public:
    IMergingTransformBase(
        size_t num_inputs,
        const Block & input_header,
        const Block & output_header,
        bool have_all_inputs_,
        UInt64 limit_hint_);

    IMergingTransformBase(
        const Blocks & input_headers,
        const Block & output_header,
        bool have_all_inputs_,
        UInt64 limit_hint_);

    OutputPort & getOutputPort() { return outputs.front(); }

    /// Methods to add additional input port. It is possible to do only before the first call of `prepare`.
    void addInput();
    /// Need to be called after all inputs are added. (only if have_all_inputs was not specified).
    void setHaveAllInputs();

    Status prepare() override;

protected:
    virtual void onNewInput(); /// Is called when new input is added. Only if have_all_inputs = false.
    virtual void onFinish() {} /// Is called when all data is processed.

    /// Processor state.
    struct State
    {
        Chunk output_chunk;
        IMergingAlgorithm::Input input_chunk;
        bool has_input = false;
        bool is_finished = false;
        bool need_data = false;
        bool no_data = false;
        size_t next_input_to_read = 0;

        IMergingAlgorithm::Inputs init_chunks;
    };

    State state;

private:
    struct InputState
    {
        explicit InputState(InputPort & port_) : port(port_) {}

        InputPort & port;
        bool is_initialized = false;
    };

    std::vector<InputState> input_states;
    std::atomic<bool> have_all_inputs;
    bool is_initialized = false;
    UInt64 limit_hint = 0;

    IProcessor::Status prepareInitializeInputs();
};

/// Implementation of MergingTransform using IMergingAlgorithm.
template <typename Algorithm>
class IMergingTransform : public IMergingTransformBase
{
public:
    template <typename ... Args>
    IMergingTransform(
        size_t num_inputs,
        const Block & input_header,
        const Block & output_header,
        bool have_all_inputs_,
        UInt64 limit_hint_,
        Args && ... args)
        : IMergingTransformBase(num_inputs, input_header, output_header, have_all_inputs_, limit_hint_)
        , algorithm(std::forward<Args>(args) ...)
    {
        // numinputs = num_inputs;
        // in3 = input_header;
        // out3 = output_header;
        // have_allinputs = have_all_inputs_;
        // limithint = limit_hint_;
    }

    template <typename ... Args>
    IMergingTransform(
        const Blocks & input_headers,
        const Block & output_header,
        bool have_all_inputs_,
        UInt64 limit_hint_,
        bool empty_chunk_on_finish_,
        Args && ... args)
        : IMergingTransformBase(input_headers, output_header, have_all_inputs_, limit_hint_)
        , empty_chunk_on_finish(empty_chunk_on_finish_)
        , algorithm(std::forward<Args>(args) ...)
    {
        // numinputs = num_inputs;
        // in3 = input_header;
        // out3 = output_header;
        // have_allinputs = have_all_inputs_;
        // limithint = limit_hint_;
    }

    // std::vector<Param> getParaList() override{
    //     // ParaVal pv = ParaVal();
    //     // vec.push_back(TestC("header", header));
    //     std::vector<Param> vec;
    //     if(in3){
    //         vec.push_back(Param("rows_input",std::to_string(in3.rows())));
    //         vec.push_back(Param("colomns_input",std::to_string(in3.columns())));
    //     }
    //     vec.push_back(Param("rows_output",std::to_string(out3.rows())));
    //     vec.push_back(Param("colomns_output",std::to_string(out3.columns())));
    //     if(numinputs!=(UInt64)-1){
    //         vec.push_back(Param("num_inputs", std::to_string(numinputs)));
    //     }
        
    //     vec.push_back(Param("max_block_size", std::to_string(max_blocksize)));
    //     vec.push_back(Param("have_all_inputs", std::to_string(have_allinputs)));
    //     vec.push_back(Param("limit_hint", std::to_string(limithint)));
    //     vec.push_back(Param("empty_chunk_on_finish", std::to_string(empty_chunk_on_finish)));
    //     return vec;
    // }

    void work() override
    {
        if (!state.init_chunks.empty())
            algorithm.initialize(std::move(state.init_chunks));

        if (state.has_input)
        {
            // std::cerr << "Consume chunk with " << state.input_chunk.getNumRows()
            //           << " for input " << state.next_input_to_read << std::endl;
            algorithm.consume(state.input_chunk, state.next_input_to_read);
            state.has_input = false;
        }
        else if (state.no_data && empty_chunk_on_finish)
        {
            IMergingAlgorithm::Input current_input;
            algorithm.consume(current_input, state.next_input_to_read);
            state.no_data = false;
        }

        IMergingAlgorithm::Status status = algorithm.merge();

        if ((status.chunk && status.chunk.hasRows()) || status.chunk.hasChunkInfo())
        {
            // std::cerr << "Got chunk with " << status.chunk.getNumRows() << " rows" << std::endl;
            state.output_chunk = std::move(status.chunk);
        }

        if (status.required_source >= 0)
        {
            // std::cerr << "Required data for input " << status.required_source << std::endl;
            state.next_input_to_read = status.required_source;
            state.need_data = true;
        }

        if (status.is_finished)
        {
            // std::cerr << "Finished" << std::endl;
            state.is_finished = true;
        }
    }

protected:
    /// Call `consume` with empty chunk when there is no more data.
    bool empty_chunk_on_finish = false;
    // Block in3;
    // Block out3;
    // size_t numinputs = -1;
    //     // SortDescription description;
    // size_t max_blocksize;
    //     // Block & input_header;
    //     // Block & output_header;
    // bool have_allinputs = true;
    // UInt64 limithint = 0;

    Algorithm algorithm;

    /// Profile info.
    Stopwatch total_stopwatch {CLOCK_MONOTONIC_COARSE};

private:
    using IMergingTransformBase::state;
};

using MergingTransformPtr = std::shared_ptr<IMergingTransformBase>;

}
