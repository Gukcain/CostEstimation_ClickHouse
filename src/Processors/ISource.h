#pragma once

#include <Processors/IProcessor.h>


namespace DB
{

class ISource : public IProcessor
{
private:
    ReadProgressCounters read_progress;
    bool read_progress_was_set = false;
    bool auto_progress;

protected:
    OutputPort & output;
    bool has_input = false;
    bool finished = false;
    bool got_exception = false;
    Port::Data current_chunk;

    std::shared_ptr<const StorageLimitsList> storage_limits;

    virtual Chunk generate();
    virtual std::optional<Chunk> tryGenerate();

    virtual void progress(size_t read_rows, size_t read_bytes);

public:
    Block i_source_header;
    bool enable = true;
    explicit ISource(Block header, bool enable_auto_progress = true);
    ~ISource() override;

    Status prepare() override;
    void work() override;
    std::vector<Param> getParaList() override{
        // ParaVal pv73 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(i_source_header.rows())));
        vec.push_back(Param("colomns",std::to_string(i_source_header.columns())));
        vec.push_back(Param("enable_auto_progress",std::to_string(enable)));
        // vec.push_back(Param("num_outputs",std::to_string(pv73.num_outputs)));
        return vec;
    }

    OutputPort & getPort() { return output; }
    const OutputPort & getPort() const { return output; }

    void setStorageLimits(const std::shared_ptr<const StorageLimitsList> & storage_limits_) override;

    /// Default implementation for all the sources.
    std::optional<ReadProgress> getReadProgress() final;

    void addTotalRowsApprox(size_t value) { read_progress.total_rows_approx += value; }
};

using SourcePtr = std::shared_ptr<ISource>;

}
