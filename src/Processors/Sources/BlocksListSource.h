#pragma once

#include <Processors/ISource.h>


namespace DB
{
    // using namespace std;
// template <typename T>
class ParaVal13{
    public:
        // T value;

        // const Block
        Block header;
        bool enable_auto_progress = true;

        // ParaVal13();
};

/** A stream of blocks from which you can read the next block from an explicitly provided list.
  * Also see SourceFromSingleChunk.
  */
class BlocksListSource : public ISource
{
public:
ParaVal13 pv13 = ParaVal13();
    std::vector<Param> getParaList() override{
        // ParaVal pv13 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv13.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv13.header.columns())));
        vec.push_back(Param("enable_auto_progress",std::to_string(pv13.enable_auto_progress)));
        
        return vec;
    }
    /// Acquires the ownership of the block list.
    explicit BlocksListSource(BlocksList && list_)
        : ISource(list_.empty() ? Block() : list_.front().cloneEmpty())
        , list(std::move(list_)), it(list.begin()), end(list.end()) {
            pv13.header = list_.empty() ? Block() : list_.front().cloneEmpty();
        }

    /// Uses a list of blocks lying somewhere else.
    BlocksListSource(BlocksList::iterator & begin_, BlocksList::iterator & end_)
        : ISource(begin_ == end_ ? Block() : begin_->cloneEmpty())
        , it(begin_), end(end_) {begin_ == end_ ? Block() : begin_->cloneEmpty();}

    String getName() const override { return "BlocksListSource"; }

protected:

    Chunk generate() override
    {
        if (it == end)
            return {};

        Block res = *it;
        ++it;

        size_t num_rows = res.rows();
        return Chunk(res.getColumns(), num_rows);
    }

private:
    BlocksList list;
    BlocksList::iterator it;
    const BlocksList::iterator end;
};

}
