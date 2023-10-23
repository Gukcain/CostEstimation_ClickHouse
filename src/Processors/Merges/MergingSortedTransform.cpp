#include <Processors/Merges/MergingSortedTransform.h>
#include <Processors/Transforms/ColumnGathererTransform.h>
#include <IO/WriteBuffer.h>

#include <Common/logger_useful.h>

namespace DB
{

MergingSortedTransform::MergingSortedTransform(
    const Block & header,
    size_t num_inputs,
    const SortDescription & description_,
    size_t max_block_size,
    SortingQueueStrategy sorting_queue_strategy,
    UInt64 limit_,
    WriteBuffer * out_row_sources_buf_,
    bool quiet_,
    bool use_average_block_sizes,
    bool have_all_inputs_)
    : IMergingTransform(
        num_inputs,
        header,
        header,
        have_all_inputs_,
        limit_,
        header,
        num_inputs,
        description_,
        max_block_size,
        sorting_queue_strategy,
        limit_,
        out_row_sources_buf_,
        use_average_block_sizes)
    , quiet(quiet_)
{
    pv5.header = header;
    pv5.num_inputs = num_inputs;
    // pv5.description = description_;
    pv5.max_block_size = max_block_size;
    // pv5.input_header = header;
    // pv5.output_header = header;
    pv5.have_all_inputs = true;
    pv5.limit = 0;
    pv5.quiet = quiet_;
    pv5.use_average_block_sizes = use_average_block_sizes;
}

void MergingSortedTransform::onNewInput()
{
    algorithm.addInput();
}

void MergingSortedTransform::onFinish()
{
    if (quiet)
        return;

    const auto & merged_data = algorithm.getMergedData();

    auto * log = &Poco::Logger::get("MergingSortedTransform");

    double seconds = total_stopwatch.elapsedSeconds();

    if (seconds == 0.0)
        LOG_DEBUG(log, "Merge sorted {} blocks, {} rows in 0 sec.", merged_data.totalChunks(), merged_data.totalMergedRows());
    else
        LOG_DEBUG(log, "Merge sorted {} blocks, {} rows in {} sec., {} rows/sec., {}/sec",
            merged_data.totalChunks(), merged_data.totalMergedRows(), seconds,
            merged_data.totalMergedRows() / seconds,
            ReadableSize(merged_data.totalAllocatedBytes() / seconds));
}

}
