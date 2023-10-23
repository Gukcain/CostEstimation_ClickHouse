#pragma once

#include <IO/Progress.h>
#include <Processors/Transforms/ExceptionKeepingTransform.h>
#include <Access/EnabledQuota.h>


namespace DB
{

class QueryStatus;
using QueryStatusPtr = std::shared_ptr<QueryStatus>;
class ThreadStatus;
// using namespace std;
class ParaVal32{
    public:
        // T value;

        // const Block
        Block  header;
        // SortDescription description;
        // ThreadStatus
        

        
        // ParaVal32();
};
/// Proxy class which counts number of written block, rows, bytes
class CountingTransform final : public ExceptionKeepingTransform
{
public:
    ParaVal32 pv32 = ParaVal32();
    std::vector<Param> getParaList() override{
        // ParaVal pv32 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv32.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv32.header.columns())));

        
        return vec;
    }
    explicit CountingTransform(
        const Block & header,
        ThreadStatus * thread_status_ = nullptr,
        std::shared_ptr<const EnabledQuota> quota_ = nullptr)
        : ExceptionKeepingTransform(header, header)
        , thread_status(thread_status_), quota(std::move(quota_)) {
            pv32.header = header;
            
        }

    String getName() const override { return "CountingTransform"; }

    void setProgressCallback(const ProgressCallback & callback)
    {
        progress_callback = callback;
    }

    void setProcessListElement(QueryStatusPtr elem)
    {
        process_elem = elem;
    }

    const Progress & getProgress() const
    {
        return progress;
    }

    void onConsume(Chunk chunk) override;
    GenerateResult onGenerate() override
    {
        GenerateResult res;
        res.chunk = std::move(cur_chunk);
        return res;
    }

protected:
    Progress progress;
    ProgressCallback progress_callback;
    QueryStatusPtr process_elem;
    ThreadStatus * thread_status = nullptr;

    /// Quota is used to limit amount of written bytes.
    std::shared_ptr<const EnabledQuota> quota;
    Chunk cur_chunk;
};

}
