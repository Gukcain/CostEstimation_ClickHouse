#pragma once
#include <Processors/QueryPlan/IQueryPlanStep.h>

namespace DB
{

/// Unite several logical streams of data into single logical stream with specified structure.
class UnionStep : public IQueryPlanStep
{
public:
    /// max_threads is used to limit the number of threads for result pipeline.
    // 改 05-01
    // 回 2023-11-14
    explicit UnionStep(DataStreams input_streams_, size_t max_threads_ = 0);
    // explicit UnionStep(DataStreams input_streams_, size_t max_threads_ = 1);

    String getName() const override { return "Union"; }

    QueryPipelineBuilderPtr updatePipeline(QueryPipelineBuilders pipelines, const BuildQueryPipelineSettings &) override;

    void describePipeline(FormatSettings & settings) const override;

    size_t getMaxThreads() const { return max_threads; }

    void updateOutputSortDescription();

private:
    Block header;
    // 改 05-01
    // 回 2023-11-14
    // size_t max_threads = 1;
    size_t max_threads;
    Processors processors;
};

}
