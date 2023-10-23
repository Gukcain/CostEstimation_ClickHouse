#pragma once

#include <string>
#include <Core/Block.h>
#include <Processors/ISource.h>
#include <mysqlxx/PoolWithFailover.h>
#include <mysqlxx/Query.h>
#include <Core/ExternalResultDescription.h>
#include <Core/Settings.h>

namespace DB
{
// using namespace std;

struct StreamSettings
{
    /// Check if setting is enabled, otherwise use common `max_block_size` setting.
    size_t max_read_mysql_row_nums;
    size_t max_read_mysql_bytes_size;
    bool auto_close;
    bool fetch_by_name;
    size_t default_num_tries_on_connection_loss;
    StreamSettings()=default;
    StreamSettings(const Settings & settings, bool auto_close_ = false, bool fetch_by_name_ = false, size_t max_retry_ = 5);

};
class ParaVal16{
    public:
        // T value;

        // const Block
        Block header;
        bool enable_auto_progress = true;
        String query_str;
        StreamSettings ss;

        // ParaVal16();
};

/// Allows processing results of a MySQL query as a sequence of Blocks, simplifies chaining
class MySQLSource : public ISource
{
public:
ParaVal16 pv16 = ParaVal16();
    std::vector<Param> getParaList() override{
        // ParaVal pv16 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv16.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv16.header.columns())));
        vec.push_back(Param("enable_auto_progress",std::to_string(pv16.enable_auto_progress)));
        vec.push_back(Param("query_str",pv16.query_str));
        vec.push_back(Param("max_read_mysql_row_nums",std::to_string(pv16.ss.max_read_mysql_row_nums)));
        vec.push_back(Param("max_read_mysql_bytes_size",std::to_string(pv16.ss.max_read_mysql_bytes_size)));
        vec.push_back(Param("auto_close",std::to_string(pv16.ss.auto_close)));
        vec.push_back(Param("default_num_tries_on_connection_loss",std::to_string(pv16.ss.default_num_tries_on_connection_loss)));
        vec.push_back(Param("fetch_by_name",std::to_string(pv16.ss.fetch_by_name)));
        return vec;
    }
    MySQLSource(
        const mysqlxx::PoolWithFailover::Entry & entry,
        const std::string & query_str,
        const Block & sample_block,
        const StreamSettings & settings_);

    String getName() const override { return "MySQL"; }

protected:
    MySQLSource(const Block & sample_block_, const StreamSettings & settings);
    Chunk generate() override;
    void initPositionMappingFromQueryResultStructure();

    struct Connection
    {
        Connection(const mysqlxx::PoolWithFailover::Entry & entry_, const std::string & query_str);

        mysqlxx::PoolWithFailover::Entry entry;
        mysqlxx::Query query;
        mysqlxx::UseQueryResult result;
    };

    Poco::Logger * log;
    std::unique_ptr<Connection> connection;

    const std::unique_ptr<StreamSettings> settings;
    std::vector<size_t> position_mapping;
    ExternalResultDescription description;
};

/// Like MySQLSource, but allocates connection only when reading is starting.
/// It allows to create a lot of stream objects without occupation of all connection pool.
/// Also makes attempts to reconnect in case of connection failures.
class MySQLWithFailoverSource final : public MySQLSource
{
public:

    MySQLWithFailoverSource(
        mysqlxx::PoolWithFailoverPtr pool_,
        const std::string & query_str_,
        const Block & sample_block_,
        const StreamSettings & settings_);

    Chunk generate() override;

private:
    void onStart();

    mysqlxx::PoolWithFailoverPtr pool;
    std::string query_str;
    bool is_initialized = false;
};

}
