#pragma once
#include <Processors/ISimpleTransform.h>
#include <Core/SortDescription.h>
#include <Columns/IColumn.h>

namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal29{
    public:
        // T value;

        // const Block
        Block header;
        bool skip_empty_chunks = false;

        // ParaVal29();
};
/// Streams checks that flow of blocks is sorted in the sort_description order
/// Othrewise throws exception in readImpl function.
class CheckSortedTransform : public ISimpleTransform
{
public:
    ParaVal29 pv29 = ParaVal29();
    std::vector<Param> getParaList() override{
        // ParaVal pv29 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv29.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv29.header.columns())));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv29.skip_empty_chunks)));
        
        return vec;
    }
    CheckSortedTransform(const Block & header, const SortDescription & sort_description);

    String getName() const override { return "CheckSortedTransform"; }


protected:
    void transform(Chunk & chunk) override;

private:
    SortDescriptionWithPositions sort_description_map;
    Columns last_row;
};
}
