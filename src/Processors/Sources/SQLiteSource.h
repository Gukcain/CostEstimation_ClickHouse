#pragma once

#include "config.h"

#if USE_SQLITE
#include <Core/ExternalResultDescription.h>
#include <Processors/ISource.h>

#include <sqlite3.h>


namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal21{
    public:
        // T value;

        // const Block
        Block header;
        bool enable_auto_progress = true;
        String query;
        UInt64 max_block_size;

        // ParaVal21();
};
class SQLiteSource : public ISource
{

using SQLitePtr = std::shared_ptr<sqlite3>;

public:
ParaVal21 pv21 = ParaVal21();
    std::vector<Param> getParaList() override{
        // ParaVal pv21 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv21.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv21.header.columns())));
        vec.push_back(Param("enable_auto_progress",std::to_string(pv21.enable_auto_progress)));
        vec.push_back(Param("max_block_size",std::to_string(pv21.max_block_size)));
        vec.push_back(Param("query_string",pv21.query));
        return vec;
    }
    SQLiteSource(SQLitePtr sqlite_db_, const String & query_str_, const Block & sample_block, UInt64 max_block_size_);

    String getName() const override { return "SQLite"; }

private:

    using ValueType = ExternalResultDescription::ValueType;

    struct StatementDeleter
    {
        void operator()(sqlite3_stmt * stmt) { sqlite3_finalize(stmt); }
    };

    Chunk generate() override;

    void insertValue(IColumn & column, ExternalResultDescription::ValueType type, int idx);

    String query_str;
    UInt64 max_block_size;

    ExternalResultDescription description;
    SQLitePtr sqlite_db;
    std::unique_ptr<sqlite3_stmt, StatementDeleter> compiled_statement;
};

}

#endif
