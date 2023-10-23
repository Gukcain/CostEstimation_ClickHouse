#include <random>
#include <Interpreters/RandomGenerate.h>


int formData(){
	std::random_device sd;//生成random_device对象sd做种子
    std::minstd_rand linear_ran(sd());//使用种子初始化linear_congruential_engine对象，为的是使用它来做我们下面随机分布的种子以输出多个随机分布.注意这里要使用()操作符，因为minst_rand()接受的是一个值（你用srand也是给出这样的一个值）
    std::uniform_int_distribution<int>dis1(0,10000000);
    return dis1(linear_ran);
}
