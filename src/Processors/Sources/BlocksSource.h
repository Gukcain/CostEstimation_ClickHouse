#pragma once
/* Copyright (c) 2018 BlackBerry Limited

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include <Processors/ISource.h>
#include <Processors/Transforms/AggregatingTransform.h>


namespace DB
{
// using namespace std;
// template <typename T>
class ParaVal14{
    public:
        // T value;

        // const Block
        Block header;
        bool enable_auto_progress = true;

        // ParaVal14();
};

/** A stream of blocks from a shared vector of blocks
  */
class BlocksSource : public ISource
{
public:
    ParaVal14 pv14 = ParaVal14();
    std::vector<Param> getParaList() override{
        // ParaVal pv14 = ParaVal();
        // vec.push_back(TestC("header", header));
        std::vector<Param> vec;
        vec.push_back(Param("rows",std::to_string(pv14.header.rows())));
        vec.push_back(Param("colomns",std::to_string(pv14.header.columns())));
        vec.push_back(Param("enable_auto_progress",std::to_string(pv14.enable_auto_progress)));
        
        return vec;
    }
    /// Acquires shared ownership of the blocks vector
    BlocksSource(BlocksPtr blocks_ptr_, Block header)
        : ISource(std::move(header))
        , blocks(blocks_ptr_), it(blocks_ptr_->begin()), end(blocks_ptr_->end()) {pv14.header = header;}

    String getName() const override { return "Blocks"; }

protected:
    Chunk generate() override
    {
        if (it == end)
            return {};

        Block res = *it;
        ++it;

        auto info = std::make_shared<AggregatedChunkInfo>();
        info->bucket_num = res.info.bucket_num;
        info->is_overflows = res.info.is_overflows;

        return Chunk(res.getColumns(), res.rows(), std::move(info));
    }

private:
    BlocksPtr blocks;
    Blocks::iterator it;
    const Blocks::iterator end;
};

}
