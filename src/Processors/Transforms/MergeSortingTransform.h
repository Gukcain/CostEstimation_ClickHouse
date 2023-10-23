#pragma once

#include <Processors/Transforms/SortingTransform.h>
#include <Core/SortDescription.h>
#include <Common/filesystemHelpers.h>
#include <Disks/TemporaryFileOnDisk.h>
#include <Interpreters/TemporaryDataOnDisk.h>
#include <Common/logger_useful.h>


namespace DB
{
// using namespace std;
class ParaVal52{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        size_t max_merged_block_size;
        UInt64 limit;
        bool increase_sort_description_compile_attempts;
        size_t max_bytes_before_remerge;
        double remerge_lowered_memory_bytes_ratio;
        size_t max_bytes_before_external_sort;
        size_t min_free_disk_space;
        
        // ParaVal52();
};
class IVolume;
using VolumePtr = std::shared_ptr<IVolume>;

/// Takes sorted separate chunks of data. Sorts them.
/// Returns stream with globally sorted data.
class MergeSortingTransform : public SortingTransform
{
public:
    ParaVal52 pv52 = ParaVal52();
    std::vector<Param> getParaList() override{
        // ParaVal pv52 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv52.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv52.header.columns())));
        vec.push_back(Param("increase_sort_description_compile_attempts",std::to_string(pv52.increase_sort_description_compile_attempts)));
        vec.push_back(Param("max_merged_block_size",std::to_string(pv52.max_merged_block_size)));
        vec.push_back(Param("limit",std::to_string(pv52.limit)));
        vec.push_back(Param("max_bytes_before_remerge",std::to_string(pv52.max_bytes_before_remerge)));
        vec.push_back(Param("remerge_lowered_memory_bytes_ratio",std::to_string(pv52.remerge_lowered_memory_bytes_ratio)));
        vec.push_back(Param("max_bytes_before_external_sort",std::to_string(pv52.max_bytes_before_external_sort)));
        vec.push_back(Param("min_free_disk_space",std::to_string(pv52.min_free_disk_space)));
        return vec;
    }
    /// limit - if not 0, allowed to return just first 'limit' rows in sorted order.
    MergeSortingTransform(
        const Block & header,
        const SortDescription & description_,
        size_t max_merged_block_size_,
        UInt64 limit_,
        bool increase_sort_description_compile_attempts,
        size_t max_bytes_before_remerge_,
        double remerge_lowered_memory_bytes_ratio_,
        size_t max_bytes_before_external_sort_,
        TemporaryDataOnDiskPtr tmp_data_,
        size_t min_free_disk_space_);

    String getName() const override { return "MergeSortingTransform"; }

protected:
    void consume(Chunk chunk) override;
    void serialize() override;
    void generate() override;

    Processors expandPipeline() override;

private:
    size_t max_bytes_before_remerge;
    double remerge_lowered_memory_bytes_ratio;
    size_t max_bytes_before_external_sort;
    TemporaryDataOnDiskPtr tmp_data;
    size_t min_free_disk_space;

    size_t sum_rows_in_blocks = 0;
    size_t sum_bytes_in_blocks = 0;

    Poco::Logger * log = &Poco::Logger::get("MergeSortingTransform");

    /// If remerge doesn't save memory at least several times, mark it as useless and don't do it anymore.
    bool remerge_is_useful = true;

    /// Merge all accumulated blocks to keep no more than limit rows.
    void remerge();

    ProcessorPtr external_merging_sorted;
};

}
