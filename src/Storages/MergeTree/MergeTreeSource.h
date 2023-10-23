#pragma once
#include <Processors/ISource.h>
#include "Storages/MergeTree/MergeTreeBaseSelectProcessor.h"

namespace DB
{

class IMergeTreeSelectAlgorithm;
using MergeTreeSelectAlgorithmPtr = std::unique_ptr<IMergeTreeSelectAlgorithm>;

struct ChunkAndProgress;

class MergeTreeSource final : public ISource
{
public:
    explicit MergeTreeSource(MergeTreeSelectAlgorithmPtr algorithm_);
    ~MergeTreeSource() override;

    std::string getName() const override;

    std::vector<Param> getParaList() override{
        // ParaVal pv73 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(i_source_header.rows())));
        vec.push_back(Param("colomns",std::to_string(i_source_header.columns())));
        vec.push_back(Param("MergeTreeSelectAlgorithm",algorithm.get()->getName()));
        vec.push_back(Param("enable_auto_progress",std::to_string(enable)));
        return vec;
    }

    Status prepare() override;

#if defined(OS_LINUX)
    int schedule() override;
#endif

protected:
    std::optional<Chunk> tryGenerate() override;

    void onCancel() override;

private:
    MergeTreeSelectAlgorithmPtr algorithm;

#if defined(OS_LINUX)
    struct AsyncReadingState;
    std::unique_ptr<AsyncReadingState> async_reading_state;
#endif

    std::optional<Chunk> reportProgress(ChunkAndProgress chunk);
};

}
