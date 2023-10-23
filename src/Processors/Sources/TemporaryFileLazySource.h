#pragma once

#include <Processors/ISource.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal22{
    public:
        // T value;

        // const Block
        Block header;
        bool enable_auto_progress = true;
        String path;

        // ParaVal22();
};
struct TemporaryFileStreamLegacy;

class TemporaryFileLazySource : public ISource
{
public:
ParaVal22 pv22 = ParaVal22();
    std::vector<Param> getParaList() override{
        // ParaVal pv22 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv22.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv22.header.columns())));
        vec.push_back(Param("enable_auto_progress",std::to_string(pv22.enable_auto_progress)));
        vec.push_back(Param("path",(pv22.path)));
        vec.push_back(Param("done",std::to_string(done)));
        return vec;
    }
    TemporaryFileLazySource(const std::string & path_, const Block & header_);
    ~TemporaryFileLazySource() override;
    String getName() const override { return "TemporaryFileLazySource"; }

protected:
    Chunk generate() override;

private:
    const std::string path;
    Block header;
    bool done;

    std::unique_ptr<TemporaryFileStreamLegacy> stream;
};

}
