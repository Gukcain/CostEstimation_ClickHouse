#pragma once
#include <Access/EnabledQuota.h>
#include <Processors/ISimpleTransform.h>
#include <QueryPipeline/SizeLimits.h>
#include <Poco/Timespan.h>
#include <Common/Stopwatch.h>

#include <QueryPipeline/StreamLocalLimits.h>

namespace DB
{
// using namespace std;
class ParaVal48{
    public:
        // T value;

        // const Block
        Block header;
        StreamLocalLimits sl;
        // SortDescription description;
        
        // ParaVal48();
};
/// Information for profiling.
struct ProcessorProfileInfo
{
    bool started = false;
    Stopwatch total_stopwatch {CLOCK_MONOTONIC_COARSE};    /// Time with waiting time

    size_t rows = 0;
    size_t blocks = 0;
    size_t bytes = 0;

    void update(const Chunk & block);
};

class LimitsCheckingTransform : public ISimpleTransform
{
public:
    ParaVal48 pv48 = ParaVal48();
    std::vector<Param> getParaList() override{
        // ParaVal pv48 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv48.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv48.header.columns())));
        
        vec.push_back(Param("StreamLocalLimits_size_limits_max_rows",std::to_string(pv48.sl.size_limits.max_rows)));
        vec.push_back(Param("StreamLocalLimits_size_limits_max_bytes",std::to_string(pv48.sl.size_limits.max_bytes)));
        return vec;
    }
    LimitsCheckingTransform(const Block & header_, StreamLocalLimits limits_);

    String getName() const override { return "LimitsCheckingTransform"; }

    void setQuota(const std::shared_ptr<const EnabledQuota> & quota_) { quota = quota_; }

protected:
    void transform(Chunk & chunk) override;

private:
    StreamLocalLimits limits;

    std::shared_ptr<const EnabledQuota> quota;
    UInt64 prev_elapsed = 0;

    ProcessorProfileInfo info;

    bool checkTimeLimit();
    void checkQuota(Chunk & chunk);
};

}
