#pragma once

#include <Processors/ISource.h>
#include <Interpreters/AsynchronousInsertQueue.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal23{
    public:
        // T value;

        // const Block
        Block header;
        bool enable_auto_progress = true;
        size_t timeout_ms;

        // ParaVal23();
};
namespace ErrorCodes
{
    extern const int TIMEOUT_EXCEEDED;
    extern const int LOGICAL_ERROR;
}

/// Source, that allow to wait until processing of
/// asynchronous insert for specified query_id will be finished.
class WaitForAsyncInsertSource : public ISource, WithContext
{
public:
    ParaVal23 pv23 = ParaVal23();
    std::vector<Param> getParaList() override{
        // ParaVal pv23 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv23.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv23.header.columns())));
        vec.push_back(Param("enable_auto_progress",std::to_string(pv23.enable_auto_progress)));
        vec.push_back(Param("timeout_ms",std::to_string(pv23.timeout_ms)));
        return vec;
    }
    WaitForAsyncInsertSource(
        std::future<void> insert_future_, size_t timeout_ms_)
        : ISource(Block())
        , insert_future(std::move(insert_future_))
        , timeout_ms(timeout_ms_)
    {
        pv23.header = Block();
        pv23.timeout_ms = timeout_ms_;
        assert(insert_future.valid());
    }

    String getName() const override { return "WaitForAsyncInsert"; }

protected:
    Chunk generate() override
    {
        auto status = insert_future.wait_for(std::chrono::milliseconds(timeout_ms));
        if (status == std::future_status::deferred)
            throw Exception(ErrorCodes::LOGICAL_ERROR, "Logical error: got future in deferred state");

        if (status == std::future_status::timeout)
            throw Exception(ErrorCodes::TIMEOUT_EXCEEDED, "Wait for async insert timeout ({} ms) exceeded)", timeout_ms);

        insert_future.get();
        return Chunk();
    }

private:
    std::future<void> insert_future;
    size_t timeout_ms;
};

}
