#pragma once

#include <base/types.h>

// #include <fmt/format.h>

namespace DB
{
    // using String = std::string;
    class Param{
        public:
            // String type;     // 参数类型默认是以String保存
            String name;
            // std::any val;
            String val;

            // Param(String paramName, std::any paramVal):name(paramName),val(paramVal){}
            Param(String paramName, String paramVal):name(paramName),val(paramVal){}
    };
}
