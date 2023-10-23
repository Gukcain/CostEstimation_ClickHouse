#pragma once
#include <Storages/MergeTree/MergeTreeSelectProcessor.h>
#include <Processors/Param.h>

namespace DB
{


/// Used to read data from single part with select query in order of primary key.
/// Cares about PREWHERE, virtual columns, indexes etc.
/// To read data from multiple parts, Storage (MergeTree) creates multiple such objects.
class MergeTreeInOrderSelectAlgorithm final : public MergeTreeSelectAlgorithm
{
public:
    template <typename... Args>
    explicit MergeTreeInOrderSelectAlgorithm(Args &&... args)
        : MergeTreeSelectAlgorithm{std::forward<Args>(args)...}
    {
        LOG_TRACE(log, "Reading {} ranges in order from part {}, approx. {} rows starting from {}",
            all_mark_ranges.size(), data_part->name, total_rows,
            data_part->index_granularity.getMarkStartingRow(all_mark_ranges.front().begin));
    }

    // std::vector<Param> getParaList() override{
    //     // ParaVal pv73 = ParaVal();
    //     // vec.push_back(TestC("header", header));
    //     std::vector<Param> vec;
    //     vec.push_back(Param("rows",std::to_string(pv73.header.rows())));
    //     vec.push_back(Param("colomns",std::to_string(pv73.header.columns())));
    //     vec.push_back(Param("num_inputs",std::to_string(pv73.num_inputs)));
    //     vec.push_back(Param("num_outputs",std::to_string(pv73.num_outputs)));
    //     return vec;
    // }
    String getName() const override { return "MergeTreeInOrder"; }

private:
    bool getNewTaskImpl() override;
    void finalizeNewTask() override {}

    Poco::Logger * log = &Poco::Logger::get("MergeTreeInOrderSelectProcessor");
};

}
