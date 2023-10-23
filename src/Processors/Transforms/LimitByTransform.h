#pragma once
#include <Processors/ISimpleTransform.h>
#include <Common/HashTable/HashMap.h>


namespace DB
{
// using namespace std;
class ParaVal47{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        
        UInt64 group_length;
        UInt64 group_offset;
        Names columns;
        
        // ParaVal47();
};
/// Executes LIMIT BY for specified columns.
class LimitByTransform : public ISimpleTransform
{
public:
    ParaVal47 pv47 = ParaVal47();
    std::vector<Param> getParaList() override{
        // ParaVal pv47 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv47.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv47.header.columns())));
        String str;
        for(const auto & key : pv47.columns){
            str += key;
        }
        vec.push_back(Param("columns", str));
        vec.push_back(Param("group_length",std::to_string(pv47.group_length)));
        vec.push_back(Param("group_offset",std::to_string(pv47.group_offset)));
        return vec;
    }
    LimitByTransform(const Block & header, UInt64 group_length_, UInt64 group_offset_, const Names & columns);

    String getName() const override { return "LimitByTransform"; }

protected:
    void transform(Chunk & chunk) override;

private:
    using MapHashed = HashMap<UInt128, UInt64, UInt128TrivialHash>;

    MapHashed keys_counts;
    std::vector<size_t> key_positions;
    const UInt64 group_length;
    const UInt64 group_offset;
};

}
