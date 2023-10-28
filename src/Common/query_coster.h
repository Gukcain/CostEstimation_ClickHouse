#pragma once
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <linux/perf_event.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <chrono>
#include <unordered_set>
#include <regex>
#include <algorithm>
#include <sys/resource.h>
#include <sys/statvfs.h>
#include <algorithm>
#include <numeric>
#include <Processors/Chunk.h>//可能不对劲
#include <Core/Block.h>
#include <Common/MemoryTracker.h>
#include <unistd.h>
#include <cstdint>
// #include "src/Common/ProgressIndication.h"


namespace coster {

	class QueryCoster {
	public:
		struct Metrics
		{
			uint64_t elapsed_us_;
			uint64_t cpu_time_us_ref;
			uint64_t cpu_time_us_;
			uint64_t cpu_cycles_;
			uint64_t instructions_;
			uint64_t cache_references_;
			uint64_t cache_misses_;
			uint16_t disk_block_reads;
			uint16_t disk_block_writes;
			uint64_t memory_b_;
			uint64_t memory_b_max;
			int isused_avx = 0;//
		};
		struct MetaInfo
		{
			//表名，列数，列类型等；
			std::string name ="0";//实现后需要删除该初始化
			int columns_num = 0;
			std::string type = "0";//实现后需要删除该初始化
		};
		struct Input
		{
			size_t tuple_num_chunk = 0;
			size_t tuple_num_block_rows = 0;
			size_t tuple_num_block_columns = 0;
			MetaInfo metainfo;
			//算子调用参数
			std::string Operator = "0";//实现后需要删除该初始化
			double CPU_usage = 0;
			double memory_usage = 0;
			double IO_usage = 0;
		};
		
		QueryCoster()//
		{
			// perf_event_attr 
			cycles_us=0;
			attrs[0] = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES);
			attrs[1] = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS);
    		attrs[2] = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES);
    		attrs[3] = create_perf_event_attr(PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES);


		}
		~QueryCoster()//
		{
			
			// close(fd_i);
			// close(fd_instructions_i);
			// close(fd_cache_references_i);
			// close(fd_cache_misses_i);
			// writeCSV("data.csv",0,std::to_string(input.tuple_num_chunk));
			// writeCSV("data.csv",1,std::to_string(input.tuple_num_chunk));
			// writeCSV("data.csv",2,std::to_string(input.tuple_num_chunk));
			// writeCSV("data.csv",3,input.metainfo.name);
			// writeCSV("data.csv",4,std::to_string(input.metainfo.columns_num));
			// writeCSV("data.csv",5,input.metainfo.type);
			// writeCSV("data.csv",6,input.Operator);
			// writeCSV("data.csv",7,std::to_string(input.CPU_usage));
			// writeCSV("data.csv",8,std::to_string(input.memory_usage));
			// writeCSV("data.csv",9,std::to_string(input.IO_usage));
			// writeCSV("data.csv",10,std::to_string(metrics.elapsed_us_));
			// writeCSV("data.csv",11,std::to_string(metrics.cpu_time_us_));
			// writeCSV("data.csv",12,std::to_string(metrics.cpu_time_us_ref));
			// writeCSV("data.csv",13,std::to_string(metrics.cpu_cycles_));
			// writeCSV("data.csv",14,std::to_string(metrics.instructions_));
			// writeCSV("data.csv",15,std::to_string(metrics.cache_references_));
			// writeCSV("data.csv",16,std::to_string(metrics.cache_misses_));
			// writeCSV("data.csv",17,std::to_string(metrics.disk_block_reads));
			// writeCSV("data.csv",18,std::to_string(metrics.disk_block_writes));
			// writeCSV("data.csv",19,std::to_string(metrics.memory_b_));
			// writeCSV("data.csv",20,std::to_string(metrics.memory_b_max));
			// writeCSV("data.csv",21,std::to_string(metrics.isused_avx));


			//
		}
		void CPUTime();
		void CPUCycles();
		void CPUInstructions();
		void CPUCacheReferences();
		void CPUCacheMisses();
		void getIOUsage();
		void getCPUUsage();
		void getMemoryUsage();
		void getTuple(DB::Chunk chunk);
		void start(size_t rows,size_t columns);
		std::vector<std::string> stop();
		void writeCSV(const std::string& filename,const int targetColumn,const std::string newData);
		Metrics metrics;
		Input input;
		struct perf_event_attr attrs[5];
		long fd;
		long fd_instructions;
		long fd_cache_references;
		long fd_cache_misses;
		int fd_i;
		int fd_instructions_i;
		int fd_cache_references_i;
		int fd_cache_misses_i;
		uint64_t start_time_microseconds;
		uint64_t event_time_microseconds;
		uint64_t query_time_microseconds;
		std::string model_name_;
		uint64_t ref_cycles_us_;
		uint64_t cycles_us;
		uint64_t memory_usage_start;
		uint64_t max_host_usage_start;
		uint64_t memory_usage_event;
		uint64_t max_host_usage_event;
		// rusage start_r;
  		// rusage end_r;
		// rusage rus;
		long long rb_end;
    	long long wb_end;
		long long rb_start;
    	long long wb_start;
		pid_t pid;
		MemoryTracker memory_tracker;

		inline uint64_t timeInMicroseconds(std::chrono::time_point<std::chrono::system_clock> timepoint)
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(timepoint.time_since_epoch()).count();
		}
		struct perf_event_attr create_perf_event_attr(uint32_t type, uint64_t config)
		{
    		struct perf_event_attr attr;
    		memset(&attr, 0, sizeof(attr));
    		attr.size = sizeof(attr);
    		attr.type = type;
    		attr.config = config;
    		attr.disabled = 1;
    		attr.exclude_kernel = 1;
    		attr.exclude_hv = 1;
    		// attr.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
    		return attr;
		}
		// void Now(rusage *const usage) const {

    	// 	getrusage(RUSAGE_THREAD, usage);
  		// }
		static timeval SubtractTimeval(const timeval &lhs, const timeval &rhs) {
    		return {lhs.tv_sec - rhs.tv_sec, lhs.tv_usec - rhs.tv_usec};
 	 	}

  		// static rusage SubtractRusage(const rusage &lhs, const rusage &rhs) {
    	// 	return {SubtractTimeval(lhs.ru_utime, rhs.ru_utime),
        //     	SubtractTimeval(lhs.ru_stime, rhs.ru_stime),
        //     	{lhs.ru_maxrss - rhs.ru_maxrss},
        //     	{lhs.ru_ixrss - rhs.ru_ixrss},
        //    		{lhs.ru_idrss - rhs.ru_idrss},
        //     	{lhs.ru_isrss - rhs.ru_isrss},
        //     	{lhs.ru_minflt - rhs.ru_minflt},
        //     	{lhs.ru_majflt - rhs.ru_majflt},
        //     	{lhs.ru_nswap - rhs.ru_nswap},
        //     	{lhs.ru_inblock - rhs.ru_inblock},
        //     	{lhs.ru_oublock - rhs.ru_oublock},
        //     	{lhs.ru_msgsnd - rhs.ru_msgsnd},
        //     	{lhs.ru_msgrcv - rhs.ru_msgrcv},
        //     	{lhs.ru_nsignals - rhs.ru_nsignals},
        //     	{lhs.ru_nvcsw - rhs.ru_nvcsw},
        //     	{lhs.ru_nivcsw - rhs.ru_nivcsw}};
  		// }
		std::string merge(const std::vector<std::string>& strVector){
			std::string combinedString = std::accumulate(
        		strVector.begin(),
        		strVector.end(),
        		std::string(""),  // 初始字符串为空
        		[](const std::string &a, const std::string &b) { return a + b; }
   			 );

    		return combinedString;
		}

	};
	
}
extern MemoryTracker total_memory_tracker;
