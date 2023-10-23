#pragma once

#include <QueryPipeline/SizeLimits.h>
#include <Interpreters/Context_fwd.h>
#include <Processors/IAccumulatingTransform.h>
#include <QueryPipeline/Chain.h>
#include <QueryPipeline/QueryPipeline.h>
#include <Interpreters/PreparedSets.h>
#include <Common/Stopwatch.h>

#include <Poco/Logger.h>

namespace DB
{
// using namespace std;
class ParaVal34{
    public:
        // T value;

        // const Block
        Block header;
        Block out_header;
        // SortDescription description;
        SizeLimits network_transfer_limits;
        
        // ParaVal34();
};
class QueryStatus;
struct Progress;
using ProgressCallback = std::function<void(const Progress & progress)>;

class PushingPipelineExecutor;

/// This processor creates set during execution.
/// Don't return any data. Sets are created when Finish status is returned.
/// In general, several work() methods need to be called to finish.
/// Independent processors is created for each subquery.
class CreatingSetsTransform : public IAccumulatingTransform, WithContext
{
public:
    ParaVal34 pv34 = ParaVal34();
    std::vector<Param> getParaList() override{
        // ParaVal pv34 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv34.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv34.header.columns())));
        vec.push_back(Param("size_limits-max_rows",std::to_string(pv34.network_transfer_limits.max_rows)));
        vec.push_back(Param("size_limits-max_bytes",std::to_string(pv34.network_transfer_limits.max_bytes)));
        return vec;
    }
    CreatingSetsTransform(
        Block in_header_,
        Block out_header_,
        SubqueryForSet subquery_for_set_,
        SizeLimits network_transfer_limits_,
        ContextPtr context_);

    ~CreatingSetsTransform() override;

    String getName() const override { return "CreatingSetsTransform"; }

    void work() override;
    void consume(Chunk chunk) override;
    Chunk generate() override;

private:
    SubqueryForSet subquery;

    QueryPipeline table_out;
    std::unique_ptr<PushingPipelineExecutor> executor;
    UInt64 read_rows = 0;
    Stopwatch watch;

    bool done_with_set = true;
    bool done_with_table = true;

    SizeLimits network_transfer_limits;

    size_t rows_to_transfer = 0;
    size_t bytes_to_transfer = 0;

    using Logger = Poco::Logger;
    Poco::Logger * log = &Poco::Logger::get("CreatingSetsTransform");

    bool is_initialized = false;

    void init();
    void startSubquery();
    void finishSubquery();
};

}
