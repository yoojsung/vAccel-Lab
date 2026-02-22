#ifndef _VNPU_H_
#define _VNPU_H_

#include <linux/types.h>
#include <linux/ioctl.h>

#define VNPU_IOC_MAGIC 'V'

struct vnpu_job {
    __u32 job_id;
    __u32 status;
};

#define VNPU_IOC_SUBMIT_JOB _IOWR(VNPU_IOC_MAGIC, 1, struct vnpu_job)

#endif