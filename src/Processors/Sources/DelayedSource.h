#pragma once

#include <Processors/IProcessor.h>
#include <QueryPipeline/Pipe.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal15{
    public:
        // T value;

        // const Block
        Block header;
        bool enable_auto_progress = true;
        bool add_totals_port;
        bool add_extremes_port;

        // ParaVal15();
};
/// DelayedSource delays pipeline calculation until it starts execution.
/// It accepts callback which creates a new pipe.
///
/// First time when DelayedSource's main output port needs data, callback is called.
/// Then, DelayedSource expands pipeline: adds new inputs and connects pipe with it.
/// Then, DelayedSource just move data from inputs to outputs until finished.
///
/// It main output port of DelayedSource is never needed, callback won't be called.
class DelayedSource : public IProcessor
{
public:
ParaVal15 pv15 = ParaVal15();
    std::vector<Param> getParaList() override{
        // ParaVal pv15 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv15.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv15.header.columns())));
        vec.push_back(Param("enable_auto_progress",std::to_string(pv15.enable_auto_progress)));
        vec.push_back(Param("add_totals_port",std::to_string(pv15.add_totals_port)));
        vec.push_back(Param("add_extremes_port",std::to_string(pv15.add_extremes_port)));
        return vec;
    }
    using Creator = std::function<QueryPipelineBuilder()>;

    DelayedSource(const Block & header, Creator processors_creator, bool add_totals_port, bool add_extremes_port);
    String getName() const override { return "Delayed"; }

    Status prepare() override;
    void work() override;
    Processors expandPipeline() override;

    OutputPort & getPort() { return *main; }
    OutputPort * getTotalsPort() { return totals; }
    OutputPort * getExtremesPort() { return extremes; }

private:
    QueryPlanResourceHolder resources;
    Creator creator;
    Processors processors;

    /// Outputs for DelayedSource.
    OutputPort * main = nullptr;
    OutputPort * totals = nullptr;
    OutputPort * extremes = nullptr;

    /// Outputs from returned pipe.
    OutputPort * main_output = nullptr;
    OutputPort * totals_output = nullptr;
    OutputPort * extremes_output = nullptr;
};

/// Creates pipe from DelayedSource.
Pipe createDelayedPipe(const Block & header, DelayedSource::Creator processors_creator, bool add_totals_port, bool add_extremes_port);

}
