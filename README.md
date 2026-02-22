# vAccel-Lab
Built a Linux character device driver and test apps for a virtual AI accelerator. Reduced latency with zero-copy mmap (remap_pfn_range) to avoid redundant user-kernel copies. Added async execution using kthreads and wait queues, ensured safe concurrency with mutexes/spinlocks, and debugged performance via dmesg and ftrace.
