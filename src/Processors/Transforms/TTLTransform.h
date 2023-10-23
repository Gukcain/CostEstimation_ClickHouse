#pragma once
#include <Processors/IAccumulatingTransform.h>
#include <Storages/MergeTree/MergeTreeData.h>
#include <Storages/MergeTree/IMergeTreeDataPart.h>
#include <Core/Block.h>
#include <Storages/MergeTree/MergeTreeDataPartTTLInfo.h>
#include <Processors/TTL/ITTLAlgorithm.h>
#include <Processors/TTL/TTLDeleteAlgorithm.h>

#include <Common/DateLUT.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal64{
    public:
        // T value;

        // const Block
        Block header;
        // bool enable_auto_progress = true;
        time_t current_time;
        bool force;

        // ParaVal64();
};
class TTLTransform : public IAccumulatingTransform
{
public:
    ParaVal64 pv64 = ParaVal64();
    std::vector<Param> getParaList() override{
        // ParaVal pv64 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv64.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv64.header.columns())));
        vec.push_back(Param("current_time",std::to_string(pv64.current_time)));
        vec.push_back(Param("force",std::to_string(pv64.force)));
        
        return vec;
    }
    TTLTransform(
        const Block & header_,
        const MergeTreeData & storage_,
        const StorageMetadataPtr & metadata_snapshot_,
        const MergeTreeData::MutableDataPartPtr & data_part_,
        time_t current_time,
        bool force_
    );

    String getName() const override { return "TTL"; }

    Status prepare() override;

protected:
    void consume(Chunk chunk) override;
    Chunk generate() override;

    /// Finalizes ttl infos and updates data part
    void finalize();

private:
    std::vector<TTLAlgorithmPtr> algorithms;
    const TTLDeleteAlgorithm * delete_algorithm = nullptr;
    bool all_data_dropped = false;

    /// ttl_infos and empty_columns are updating while reading
    const MergeTreeData::MutableDataPartPtr & data_part;
    Poco::Logger * log;
};

}
