#pragma once
#include <Processors/Sinks/SinkToStorage.h>
#include <QueryPipeline/RemoteInserter.h>

namespace DB
{
  // using namespace std;
// template <typename T>
class ParaVal11{
    public:
        // T value;

        // const Block
        Block header;
        String query;

        // ParaVal11();
};

class RemoteSink final : public RemoteInserter, public SinkToStorage
{
public:
    ParaVal11 pv11 = ParaVal11();
    explicit RemoteSink(
        Connection & connection_,
        const ConnectionTimeouts & timeouts,
        const String & query_,
        const Settings & settings_,
        const ClientInfo & client_info_)
      : RemoteInserter(connection_, timeouts, query_, settings_, client_info_)
      , SinkToStorage(RemoteInserter::getHeader())
    {
      pv11.header=RemoteInserter::getHeader();
      pv11.query = query_;
    }
    std::vector<Param> getParaList() override{
        // ParaVal pv11 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv11.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv11.header.columns())));
        vec.push_back(Param("query",pv11.query));
        return vec;
    }

    String getName() const override { return "RemoteSink"; }
    void consume (Chunk chunk) override { write(RemoteInserter::getHeader().cloneWithColumns(chunk.detachColumns())); }
    void onFinish() override { RemoteInserter::onFinish(); }
};

}
