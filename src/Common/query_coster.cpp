#include <Common/query_coster.h>



namespace coster {

	void QueryCoster::CPUTime(){
		std::string line;
		std::ifstream infile("/proc/cpuinfo");
		while (std::getline(infile, line)) {
			// printf("%s",line.c_str());
			// llvm::StringRef str(line);

			// auto [name, value] = str.split(":");
			// value = value.trim(" ");

			// if (name.startswith("model name")) {
			// 	model_name_ = value.str();
			// 	std::regex cpu_freq_regex("\\s[\\d.]+GHz");
			// 	std::cmatch m;
			// 	std::regex_search(model_name_.c_str(), m, cpu_freq_regex);
			// 	if (!m.empty()) {
			// 		double base_cpu_ghz = std::stod(m[0].str());
			// 		ref_cycles_us_ = static_cast<uint64_t>(base_cpu_ghz * 1000);
			// 	}
			// }
			// else if (name.startswith("cpu MHz")) {
			// 	uint64_t cpu_mhz;
			// 	value.getAsDouble(cpu_mhz);
			// 	cycles_us = std::max(cycles_us, cpu_mhz);
			// }
			std::istringstream iss(line);
    		std::string name, value;
    		std::getline(iss, name, ':'); // 分割字符串，将结果存入 name
    		std::getline(iss, value);     // 获取冒号后的内容，将结果存入 value
			// 去除 value 字符串的首尾空格（可选，根据需要进行去除空格）
			name.erase(name.begin(), std::find_if(name.begin(), name.end(), [](unsigned char ch) {
        	return !std::isspace(ch);
   			}));
    		name.erase(std::find_if(name.rbegin(), name.rend(), [](unsigned char ch) {
        	return !std::isspace(ch);
    		}).base(), name.end());
    		value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](unsigned char ch) {
        	return !std::isspace(ch);
   			}));
    		value.erase(std::find_if(value.rbegin(), value.rend(), [](unsigned char ch) {
        	return !std::isspace(ch);
    		}).base(), value.end());
			if (name=="cpu MHz") {
				uint64_t cpu_mhz;
				cpu_mhz=std::stoull(value);
				cycles_us = std::max(cycles_us, cpu_mhz);
			}
			
		}
		if(ref_cycles_us_!=0){
			metrics.cpu_time_us_ref = metrics.cpu_cycles_ / ref_cycles_us_;
		}
		if(cycles_us!=0){
			metrics.cpu_time_us_ = metrics.cpu_cycles_ / cycles_us;
		}

		
		return;
	}

	void QueryCoster::CPUCycles() {
		fd = syscall(__NR_perf_event_open, &attrs[0], 0, -1, -1, 0);
		if (fd == -1) {
			printf("Failed to create perf event - CPU cycles\n");
			return;
		}
		fd_i = static_cast<int>(fd);
		ioctl(fd_i, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd_i, PERF_EVENT_IOC_ENABLE, 0);
		return;
	}
	void QueryCoster::CPUInstructions() {
		fd_instructions = syscall(__NR_perf_event_open, &attrs[1], 0, -1, -1, 0);
		if (fd_instructions == -1) {
			printf("Failed to create perf event - CPU instructions\n");
			return;
		}
		fd_instructions_i = static_cast<int>(fd_instructions);
		ioctl(fd_instructions_i, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd_instructions_i, PERF_EVENT_IOC_ENABLE, 0);
	}
	void QueryCoster::CPUCacheReferences() {
		fd_cache_references = syscall(__NR_perf_event_open, &attrs[2], 0, -1, -1, 0);
		if (fd_cache_references == -1) {
			printf("Failed to create perf event - CPU cache references\n");
			return;
		}
		fd_cache_references_i = static_cast<int>(fd_cache_references);
		ioctl(fd_cache_references_i, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd_cache_references_i, PERF_EVENT_IOC_ENABLE, 0);
	}
	void QueryCoster::CPUCacheMisses() {
		fd_cache_misses = syscall(__NR_perf_event_open, &attrs[3], 0, -1, -1, 0);
		if (fd_cache_misses == -1) {
			printf("Failed to create perf event - CPU cache misses\n");
			return;
		}
		fd_cache_misses_i = static_cast<int>(fd_cache_misses);
		ioctl(fd_cache_misses_i, PERF_EVENT_IOC_RESET, 0);
		ioctl(fd_cache_misses_i, PERF_EVENT_IOC_ENABLE, 0);
	}
	void QueryCoster::getIOUsage() {
		 struct statvfs stat;
    	if (statvfs("/", &stat) == 0) {
        	double totalSpace = stat.f_blocks * stat.f_frsize;
        	double freeSpace = stat.f_bfree * stat.f_frsize;
        	double usedSpace = totalSpace - freeSpace;
        	double usagePercentage = (usedSpace * 100.0) / totalSpace;
			input.IO_usage=usagePercentage;
		}
	}
	void QueryCoster::getCPUUsage() {
    	std::ifstream statFile("/proc/stat");
    	std::string line;
    	std::getline(statFile, line);

    	std::istringstream iss(line);
    	std::string cpuLabel;
    	iss >> cpuLabel;

    	double user, nice, system, idle, iowait, irq, softirq, steal;
    	iss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    	double totalIdle = idle + iowait;
    	double totalTime = user + nice + system + idle + iowait + irq + softirq + steal;

    	double utilization = (totalTime - totalIdle) / totalTime * 100.0;
		input.CPU_usage=utilization;
	}
	void QueryCoster::getMemoryUsage() {
    	std::ifstream meminfoFile("/proc/meminfo");
    	std::string line;
    	double totalMemory = 0;
		double freeMemory = 0; 
		double buffers = 0;
		double cached = 0;
    	while (std::getline(meminfoFile, line)) {
        	std::istringstream iss(line);
        	std::string label;
        	iss >> label;
        	if (label == "MemTotal:") {
            	iss >> totalMemory;
        	} else if (label == "MemFree:") {
            	iss >> freeMemory;
        	} else if (label == "Buffers:") {
            	iss >> buffers;
        	} else if (label == "Cached:") {
            	iss >> cached;
            	break;
        	}
    	}
    	double usedMemory = totalMemory - freeMemory - buffers - cached;
    	double memoryUsage = (usedMemory / totalMemory) * 100.0;
    	input.memory_usage=memoryUsage;
	}
	void QueryCoster::getTuple(DB::Chunk chunk){
		input.tuple_num_chunk = chunk.getNumRows();
	}
	void QueryCoster::start(size_t rows,size_t columns) {
		getCPUUsage();
		getIOUsage();
		getMemoryUsage();
		CPUCycles();
		CPUInstructions();
		CPUCacheReferences();
		CPUCacheMisses();
		input.tuple_num_block_rows=rows;
		input.tuple_num_block_columns=columns;
		// input.metainfo.name = merge(block.getNames());
		auto query_start_time = std::chrono::system_clock::now();
		start_time_microseconds = timeInMicroseconds(query_start_time);
		// memory_tracker.reset();
		total_memory_tracker.reset();
		// Now(&start_r);
		pid = getpid();

    	// 构建/proc/[PID]/io文件的路径
    	std::string ioFilePath = "/proc/" + std::to_string(pid) + "/io";

    	// 打开/proc/[PID]/io文件以读取I/O统计信息
    	std::ifstream ioFile(ioFilePath);

    	if (!ioFile) {
        	std::cerr << "Error opening " << ioFilePath << std::endl;
        	return ;
    	}

    	// 读取并解析I/O统计信息
    	std::string line;

	    while (std::getline(ioFile, line)) {
    	    std::istringstream iss(line);
        	std::string key, value;
        	iss >> key >> value;
        
        	if (key == "read_bytes:") {
            	rb_start=std::stoll(value);
        	} 
        	else if (key == "write_bytes:") {
            	wb_start=std::stoll(value);
        	}
    	}

    	// 关闭文件
    	ioFile.close();
		return;
	}

	std::vector<std::string> QueryCoster::stop() {
		auto query_finish_time = std::chrono::system_clock::now();
		event_time_microseconds = timeInMicroseconds(query_finish_time);
		query_time_microseconds = event_time_microseconds - start_time_microseconds;
		metrics.elapsed_us_=query_time_microseconds;
		ioctl(fd_i, PERF_EVENT_IOC_DISABLE, 0);
		ioctl(fd_instructions_i, PERF_EVENT_IOC_DISABLE, 0);
		ioctl(fd_cache_references_i, PERF_EVENT_IOC_DISABLE, 0);
		ioctl(fd_cache_misses_i, PERF_EVENT_IOC_DISABLE, 0);
		read(fd_i, &metrics.cpu_cycles_, sizeof(metrics.cpu_cycles_));
		read(fd_instructions_i, &metrics.instructions_, sizeof(metrics.instructions_));
		read(fd_cache_references_i, &metrics.cache_references_, sizeof(metrics.cache_references_));
		read(fd_cache_misses_i, &metrics.cache_misses_, sizeof(metrics.cache_misses_));
		close(fd_i);
		close(fd_instructions_i);
		close(fd_cache_references_i);
		close(fd_cache_misses_i);
		CPUTime();
		metrics.memory_b_ = total_memory_tracker.get();
		metrics.memory_b_max = total_memory_tracker.getPeak();
		// metrics.memory_b_ = memory_tracker.get();
		// metrics.memory_b_max = memory_tracker.getPeak();
		std::string ioFilePath = "/proc/" + std::to_string(pid) + "/io";
		std::ifstream iofile(ioFilePath);

    	if (!iofile) {
        	std::cerr << "Error opening " << ioFilePath << std::endl;
    	}

    	// 读取并解析I/O统计信息
    	std::string line;
    	while (std::getline(iofile, line)) {
        	std::istringstream iss(line);
        	std::string key, value;
        	iss >> key >> value;
        
        	if (key == "read_bytes:") {
            	rb_end=std::stoll(value);
        	} 
        	else if (key == "write_bytes:") {
            	wb_end=std::stoll(value);
        	}
    	}
    	iofile.close();

    	uint64_t rb=static_cast<uint64_t>(rb_end-rb_start);
    	uint64_t wb=static_cast<uint64_t>(wb_end-wb_start);
		metrics.disk_block_reads=rb;
		metrics.disk_block_writes=wb;
		// Now(&end_r);
		// rus=SubtractRusage(end_r, start_r);
		// metrics.disk_block_reads=rus.ru_inblock;
		// metrics.disk_block_writes=rus.ru_oublock;
		std::vector<std::string> res;
		// res.push_back(std::to_string(input.tuple_num_chunk));
		// res.push_back(std::to_string(input.tuple_num_block_rows));
		// res.push_back(std::to_string(input.tuple_num_block_columns));
		// res.push_back(input.metainfo.name);
		// res.push_back(std::to_string(input.metainfo.columns_num));
		// res.push_back(input.metainfo.type);
		// res.push_back(input.Operator);
		res.push_back(std::to_string(input.CPU_usage));
		res.push_back(std::to_string(input.memory_usage));
		res.push_back(std::to_string(input.IO_usage));
		res.push_back(std::to_string(metrics.elapsed_us_));
		res.push_back(std::to_string(metrics.cpu_time_us_));
		// res.push_back(std::to_string(metrics.cpu_time_us_ref));
		res.push_back(std::to_string(metrics.cpu_cycles_));
		res.push_back(std::to_string(metrics.instructions_));
		res.push_back(std::to_string(metrics.cache_references_));
		res.push_back(std::to_string(metrics.cache_misses_));
		res.push_back(std::to_string(metrics.disk_block_reads));
		res.push_back(std::to_string(metrics.disk_block_writes));
		res.push_back(std::to_string(metrics.memory_b_));
		res.push_back(std::to_string(metrics.memory_b_max));
		res.push_back(std::to_string(metrics.isused_avx));
		return res;
	}

	// void QueryCoster::writeCSV(const std::string& filename,const int targetColumn,const std::string newData) {
    // 	std::ifstream inputFile(filename);
    // 	if (!inputFile.is_open()) {
    //     	std::cerr << "Failed to open file: " << filename << std::endl;
    //     	return ;
    // 	}
    // 	std::vector<std::vector<std::string>> data;
    // 	std::string line;
    // 	while (std::getline(inputFile, line)) {
    //     	std::vector<std::string> row;
    //     	std::istringstream iss(line);
    //     	std::string value;

    //     	while (std::getline(iss, value, ',')) { 
    //         	row.push_back(value);
    //     	}

    //     	data.push_back(row);
    // 	}
    // 	inputFile.close();
    // 	size_t lastRowIndex = data.size() - 1;
    // 	bool isLastRowEmpty = targetColumn < static_cast<int>(data[lastRowIndex].size()) && data[lastRowIndex][targetColumn].empty();
    // 	if (isLastRowEmpty) {
    //     	data[lastRowIndex][targetColumn] = newData;
    // 	} else {
    //     	std::vector<std::string> newRow(data[0].size(), "");
    //     	newRow[targetColumn] = newData;
    //     	data.push_back(newRow); 
    // 	}
    // 	std::ofstream outputFile(filename);
    // 	if (!outputFile.is_open()) {
    //     	std::cerr << "Failed to open file for writing: " << filename << std::endl;
    //     	return ;
    // 	}
    // 	for (const auto &row : data) {
    //     	for (size_t i = 0; i < row.size(); ++i) {
    //        		outputFile << row[i];
    //         	if (i < row.size() - 1) {
    //         	    outputFile << ",";
    //         	}
    //     	}
    //     	outputFile << std::endl;
    // 	}
    // 	outputFile.close();
    // 	return ;
	// }

}
