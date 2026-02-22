# vAccel-Lab
Implemented a linux character device driver, and user applications that test the features, for a virtual AI Accelerator (RaspberryPi) using modern linux kernel standards. 
Eliminated redundant CPU memory copying between user and kernel space using zero-copy pipeline with custom mmap functionality (remap_pfn_range) to reduce latency for high-bandwidth AI model weight streaming.
Implemented an asynchronous execution engine using kernel threads, kthread, and wait queues. 
Optimized resource synchronization by incorporating mutexes and spinlocks to prevent race conditions during concurrent hardware access from multiple user processes.
Used dmesg and ftrace to diagnose kernel-level features and performance.
