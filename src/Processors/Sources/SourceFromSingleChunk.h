#pragma once
#include <Processors/ISource.h>


namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal20{
    public:
        // T value;

        // const Block
        Block header;
        bool enable_auto_progress = true;
        Chunk chunk;

        // ParaVal20();
};
class SourceFromSingleChunk : public ISource
{
public:
    ParaVal20 pv20 = ParaVal20();
    std::vector<Param> getParaList() override{
        // ParaVal pv20 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv20.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv20.header.columns())));
        vec.push_back(Param("enable_auto_progress",std::to_string(pv20.enable_auto_progress)));
        vec.push_back(Param("chunk_rows",std::to_string(pv20.chunk.getNumRows())));
        vec.push_back(Param("chunk_colomns",std::to_string(pv20.chunk.getNumColumns())));
        return vec;
    }
    explicit SourceFromSingleChunk(Block header, Chunk chunk_);
    explicit SourceFromSingleChunk(Block data);
    String getName() const override { return "SourceFromSingleChunk"; }

protected:
    Chunk generate() override { return std::move(chunk); }

private:
    Chunk chunk;
};

}
