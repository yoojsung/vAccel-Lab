#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "vnpu.h"

#define NP_SIZE (1024 * 1024) // 1MB

int main() {
    int fd = open("/dev/vnpu", O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    char *map = mmap(NULL, NP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        return 1; 
    }

    map[0] = 100;
    printf("[User] Initial value in shared memory: %d\n", map[0]);

    struct vnpu_job job = { .job_id = 1 };
    ioctl(fd, VNPU_IOC_SUBMIT_JOB, &job);

    printf("[User] Waiting for vNPU to process...\n");
    sleep(1);

    printf("[User] Value after vNPU processing: %d\n", (unsigned char)map[0]);

    munmap(map, NP_SIZE);
    close(fd);
    return 0;
}