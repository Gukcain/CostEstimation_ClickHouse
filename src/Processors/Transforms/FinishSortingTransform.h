#pragma once
#include <Processors/Transforms/SortingTransform.h>

namespace DB
{
// using namespace std;
class ParaVal44{
    public:
        // T value;

        // const Block
        Block header;
        // SortDescription description;
        
        size_t max_merged_block_size;
        UInt64 limit;
        bool increase_sort_description_compile_attempts;
        
        // ParaVal44();
};
/** Takes stream already sorted by `x` and finishes sorting it by (`x`, `y`).
 *  During sorting only chunks with rows that equal by `x` saved in RAM.
 * */
class FinishSortingTransform : public SortingTransform
{
public:
    ParaVal44 pv44 = ParaVal44();
    std::vector<Param> getParaList() override{
        // ParaVal pv44 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv44.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv44.header.columns())));
        vec.push_back(Param("max_merged_block_size",std::to_string(pv44.max_merged_block_size)));
        vec.push_back(Param("limit", std::to_string(pv44.limit)));
        vec.push_back(Param("increase_sort_description_compile_attempts",std::to_string(pv44.increase_sort_description_compile_attempts)));

        return vec;
    }
    /// limit - if not 0, allowed to return just first 'limit' rows in sorted order.
    FinishSortingTransform(
        const Block & header,
        const SortDescription & description_sorted_,
        const SortDescription & description_to_sort_,
        size_t max_merged_block_size_,
        UInt64 limit_,
        bool increase_sort_description_compile_attempts);

    String getName() const override { return "FinishSortingTransform"; }

protected:
    void consume(Chunk chunk) override;
    void generate() override;

private:
    SortDescriptionWithPositions description_with_positions;

    Chunk tail_chunk;
};

}
