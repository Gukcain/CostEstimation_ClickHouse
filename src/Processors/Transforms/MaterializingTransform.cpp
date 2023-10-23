#include <Processors/Transforms/MaterializingTransform.h>
#include <Columns/ColumnSparse.h>

namespace DB
{

MaterializingTransform::MaterializingTransform(const Block & header)
    : ISimpleTransform(header, materializeBlock(header), false) {
        pv49.header = header;
        pv49.skip_empty_chunks = false;
    }

void MaterializingTransform::transform(Chunk & chunk)
{
    auto num_rows = chunk.getNumRows();
    auto columns = chunk.detachColumns();

    for (auto & col : columns)
        col = recursiveRemoveSparse(col->convertToFullColumnIfConst());

    chunk.setColumns(std::move(columns), num_rows);
}

}
