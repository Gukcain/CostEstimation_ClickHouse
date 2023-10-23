#pragma once
#include <Processors/ISimpleTransform.h>

namespace DB
{
// using namespace std;
class ParaVal49{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        
        bool skip_empty_chunks = false;
        
        // ParaVal49();
};
/// Converts columns-constants to full columns ("materializes" them).
class MaterializingTransform : public ISimpleTransform
{
public:
    ParaVal49 pv49 = ParaVal49();
    std::vector<Param> getParaList() override{
        // ParaVal pv49 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv49.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv49.header.columns())));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv49.skip_empty_chunks)));
        return vec;
    }
    explicit MaterializingTransform(const Block & header);

    String getName() const override { return "MaterializingTransform"; }

protected:
    void transform(Chunk & chunk) override;
};

}
