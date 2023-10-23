#pragma once

#include "config.h"

#if USE_LIBPQXX
#include <Core/Block.h>
#include <Processors/ISource.h>
#include <Core/ExternalResultDescription.h>
#include <Core/Field.h>
#include <Core/PostgreSQL/insertPostgreSQLValue.h>
#include <Core/PostgreSQL/ConnectionHolder.h>
#include <Core/PostgreSQL/Utils.h>


namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal57{
    public:
        // T value;

        // const Block
        Block header;
        UInt64 max_block_size;
        String query_str;
        bool auto_commit = true;

        // ParaVal57();
};
template <typename T = pqxx::ReadTransaction>
class PostgreSQLSource : public ISource
{

public:
    ParaVal57 pv57 = ParaVal57();
    std::vector<Param> getParaList() override{
        // ParaVal pv57 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv57.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv57.header.columns())));
        vec.push_back(Param("max_block_size",std::to_string(pv57.max_block_size)));
        vec.push_back(Param("query_str",pv57.query_str));
        vec.push_back(Param("auto_commit",std::to_string(pv57.auto_commit)));
        
        return vec;
    }
    PostgreSQLSource(
        postgres::ConnectionHolderPtr connection_holder_,
        const String & query_str_,
        const Block & sample_block,
        UInt64 max_block_size_);

    String getName() const override { return "PostgreSQL"; }

protected:
    PostgreSQLSource(
        std::shared_ptr<T> tx_,
        const std::string & query_str_,
        const Block & sample_block,
        UInt64 max_block_size_,
        bool auto_commit_);

    String query_str;
    std::shared_ptr<T> tx;
    std::unique_ptr<pqxx::stream_from> stream;

    Status prepare() override;

    void onStart();
    Chunk generate() override;
    void onFinish();

private:
    void init(const Block & sample_block);

    const UInt64 max_block_size;
    bool auto_commit = true;
    ExternalResultDescription description;

    bool started = false;

    postgres::ConnectionHolderPtr connection_holder;

    std::unordered_map<size_t, PostgreSQLArrayInfo> array_info;
};


/// Passes transaction object into PostgreSQLSource and does not close transaction after read is finished.
template <typename T>
class PostgreSQLTransactionSource : public PostgreSQLSource<T>
{
public:
    using Base = PostgreSQLSource<T>;

    PostgreSQLTransactionSource(
        std::shared_ptr<T> tx_,
        const std::string & query_str_,
        const Block & sample_block_,
        const UInt64 max_block_size_)
        : PostgreSQLSource<T>(tx_, query_str_, sample_block_, max_block_size_, false) {}
};

}

#endif
