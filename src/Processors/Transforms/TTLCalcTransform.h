#pragma once
#include <Processors/IAccumulatingTransform.h>
#include <Storages/MergeTree/MergeTreeData.h>
#include <Storages/MergeTree/IMergeTreeDataPart.h>
#include <Core/Block.h>
#include <Storages/MergeTree/MergeTreeDataPartTTLInfo.h>
#include <Processors/TTL/ITTLAlgorithm.h>

#include <Common/DateLUT.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal63{
    public:
        // T value;

        // const Block
        Block header;
        // bool enable_auto_progress = true;
        time_t current_time;
        bool force;

        // ParaVal63();
};
class TTLCalcTransform : public IAccumulatingTransform
{
public:
    ParaVal63 pv63 = ParaVal63();
    std::vector<Param> getParaList() override{
        // ParaVal pv63 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv63.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv63.header.columns())));
        vec.push_back(Param("current_time",std::to_string(pv63.current_time)));
        vec.push_back(Param("force",std::to_string(pv63.force)));
        
        return vec;
    }
    TTLCalcTransform(
        const Block & header_,
        const MergeTreeData & storage_,
        const StorageMetadataPtr & metadata_snapshot_,
        const MergeTreeData::MutableDataPartPtr & data_part_,
        time_t current_time,
        bool force_
    );

    String getName() const override { return "TTL_CALC"; }
    Status prepare() override;

protected:
    void consume(Chunk chunk) override;
    Chunk generate() override;

    /// Finalizes ttl infos and updates data part
    void finalize();

private:
    std::vector<TTLAlgorithmPtr> algorithms;

    /// ttl_infos and empty_columns are updating while reading
    const MergeTreeData::MutableDataPartPtr & data_part;
    Poco::Logger * log;
};

}
