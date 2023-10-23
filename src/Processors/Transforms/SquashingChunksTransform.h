#pragma once

#include <Processors/Sinks/SinkToStorage.h>
#include <Interpreters/SquashingTransform.h>

namespace DB
{
// using namespace std;
class ParaVal61{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        size_t min_block_size_rows;
        size_t min_block_size_bytes;
        bool ignore_on_start_and_finish = false;
        
        // ParaVal61();
};
class SquashingChunksTransform : public ExceptionKeepingTransform
{
public:
    ParaVal61 pv61 = ParaVal61();
    std::vector<Param> getParaList() override{
        // ParaVal pv = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv61.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv61.header.columns())));
        vec.push_back(Param("min_block_size_rows",std::to_string(pv61.min_block_size_rows)));
        vec.push_back(Param("min_block_size_bytes",std::to_string(pv61.min_block_size_bytes)));
        vec.push_back(Param("ignore_on_start_and_finish",std::to_string(pv61.ignore_on_start_and_finish)));
        return vec;
    }
    explicit SquashingChunksTransform(
        const Block & header, size_t min_block_size_rows, size_t min_block_size_bytes);

    String getName() const override { return "SquashingTransform"; }

    void work() override;

protected:
    void onConsume(Chunk chunk) override;
    GenerateResult onGenerate() override;
    void onFinish() override;


private:
    SquashingTransform squashing;
    Chunk cur_chunk;
    Chunk finish_chunk;
};

}
