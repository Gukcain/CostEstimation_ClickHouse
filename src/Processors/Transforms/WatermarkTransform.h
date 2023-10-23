#pragma once

#include <Processors/ISimpleTransform.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal65{
    public:
        // T value;

        // const Block
        Block header;
        // bool enable_auto_progress = true;
        String window_column_name;
        UInt32 lateness_upper_bound;
        bool skip_empty_chunks = false;

        // ParaVal65();
};
class StorageWindowView;

class WatermarkTransform : public ISimpleTransform
{
public:
    ParaVal65 pv65 = ParaVal65();
    std::vector<Param> getParaList() override{
        // ParaVal pv65 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv65.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv65.header.columns())));
        vec.push_back(Param("lateness_upper_bound",std::to_string(pv65.lateness_upper_bound)));
        vec.push_back(Param("window_column_name",pv65.window_column_name));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv65.skip_empty_chunks)));
        return vec;
    }
    WatermarkTransform(
        const Block & header_,
        StorageWindowView & storage_,
        const String & window_column_name_,
        UInt32 lateness_upper_bound_);

    String getName() const override { return "WatermarkTransform"; }

    ~WatermarkTransform() override;

protected:
    void transform(Chunk & chunk) override;

    Block block_header;

    StorageWindowView & storage;
    String window_column_name;

    UInt32 lateness_upper_bound = 0;
    UInt32 max_watermark = 0;

    std::set<UInt32> late_signals;
};

}
