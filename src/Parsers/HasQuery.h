#pragma once

#include <mutex>
namespace DB{
class HasQuery
{
    public:
        static bool hasquery;
        HasQuery()= default;
        // explicit HasQuery(bool has);
        // static bool getHasQuery(){
        //     return hasquery;
        // }
        static std::mutex mutexofall;
};

}

