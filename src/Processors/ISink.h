#pragma once

#include <Processors/IProcessor.h>


namespace DB
{

class ISink : public IProcessor
{
protected:
    InputPort & input;
    Chunk current_chunk;
    bool has_input = false;
    bool was_on_start_called = false;
    bool was_on_finish_called = false;

    virtual void consume(Chunk block) = 0;
    virtual void onStart() {}
    virtual void onFinish() {}

public:
    Block i_sink_header;
    explicit ISink(Block header);

    Status prepare() override;
    void work() override;
    std::vector<Param> getParaList() override{
        // ParaVal pv73 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(i_sink_header.rows())));
        vec.push_back(Param("colomns",std::to_string(i_sink_header.columns())));
        // vec.push_back(Param("enable_auto_progress",std::to_string(enable)));
        // vec.push_back(Param("num_outputs",std::to_string(pv73.num_outputs)));
        return vec;
    }

    InputPort & getPort() { return input; }
};

}
