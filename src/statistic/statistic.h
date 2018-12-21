namespace statistic {

	enum statistic_type_scalar_t {
		STATISTIC_LIST_SCALAR_END
	};
	
	enum statistic_type_medium_t {
		STATISTIC_CPU_FETCHED_BYTES_PER_CYCLE,
		STATISTIC_CPU_FETCH_BUFFER_OCCUPANCY,
		STATISTIC_CPU_DISPATCHED_INSTRUCTION_PER_CYCLE,
		STATISTIC_CPU_COMMITED_INSTRUCTION_PER_CYCLE,
		STATISTIC_CPU_REORDER_BUFFER,
		STATISTIC_LIST_MEDIUM_END
	};
	
	struct statistic_type_medium_t {
		uint64_t medium[STATISTIC_LIST_MEDIUM_END][2]; /* 0: n, 1: data */
		uint32_t cpuid;
	};

};
