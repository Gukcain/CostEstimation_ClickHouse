#pragma once

#include <Processors/ISimpleTransform.h>
#include <Storages/ColumnsDescription.h>


namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal24{
    public:
        // T value;

        // const Block
        Block header;
        bool skip_empty_chunks = true;

        // ParaVal24();
};
class IInputFormat;

/// Adds defaults to columns using BlockDelayedDefaults bitmask attached to Block by child InputStream.
class AddingDefaultsTransform : public ISimpleTransform
{
public:
ParaVal24 pv24 = ParaVal24();
    std::vector<Param> getParaList() override{
        // ParaVal pv24 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv24.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv24.header.columns())));
        vec.push_back(Param("skip_empty_chunks",std::to_string(pv24.skip_empty_chunks)));
        return vec;
    }
    AddingDefaultsTransform(
        const Block & header,
        const ColumnsDescription & columns_,
        IInputFormat & input_format_,
        ContextPtr context_);

    String getName() const override { return "AddingDefaultsTransform"; }

protected:
    void transform(Chunk & chunk) override;

private:
    const ColumnsDescription columns;
    const ColumnDefaults column_defaults;
    IInputFormat & input_format;
    ContextPtr context;
};

}
