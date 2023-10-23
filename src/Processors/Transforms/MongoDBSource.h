#pragma once

#include <Poco/MongoDB/Element.h>

#include <Core/Block.h>
#include <Processors/ISource.h>
#include <Core/ExternalResultDescription.h>


namespace Poco
{
namespace MongoDB
{
    class Connection;
    class Cursor;
}
}

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal55{
    public:
        // T value;

        // const Block
        Block header;
        UInt64 max_block_size;

        // ParaVal55();
};
void authenticate(Poco::MongoDB::Connection & connection, const std::string & database, const std::string & user, const std::string & password);

std::unique_ptr<Poco::MongoDB::Cursor> createCursor(const std::string & database, const std::string & collection, const Block & sample_block_to_select);

/// Converts MongoDB Cursor to a stream of Blocks
class MongoDBSource final : public ISource
{
public:
    ParaVal55 pv55 = ParaVal55();
    std::vector<Param> getParaList() override{
        // ParaVal pv55 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv55.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv55.header.columns())));
        vec.push_back(Param("max_block_size",std::to_string(pv55.max_block_size)));
        
        return vec;
    }
    MongoDBSource(
        std::shared_ptr<Poco::MongoDB::Connection> & connection_,
        std::unique_ptr<Poco::MongoDB::Cursor> cursor_,
        const Block & sample_block,
        UInt64 max_block_size_);

    ~MongoDBSource() override;

    String getName() const override { return "MongoDB"; }

private:
    Chunk generate() override;

    std::shared_ptr<Poco::MongoDB::Connection> connection;
    std::unique_ptr<Poco::MongoDB::Cursor> cursor;
    const UInt64 max_block_size;
    ExternalResultDescription description;
    bool all_read = false;
};

}
