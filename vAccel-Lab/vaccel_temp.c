#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "vaccel"
#define CLASS_NAME "vaccel_class"
#define SHARED_MEMORY_SIZE (1024 * 1024) //1MB shared memory

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jaesung Yoo");
MODULE_DESCRIPTION("virtual AI accelerator driver");

static int major_num;
static struct class* vaccel_class = NULL;
static struct cdev* vaccel_cdev = NULL;
static char *kernel_buffer = NULL;

static int vaccel_open(struct inode *inode, struct file *file) {
	if (!try_module_get(THIS_MODULE)) {
		return -ENODEV;
	}

	printk(KERN_INFO "vAccel-Lab: device opened\n");
	return 0;
}

static int vaccel_release(struct inode *inode, struct file *file) {
	module_put(THIS_MODULE);

	printk(KERN_INFO "vAccel-Lab: device closed\n");
	return 0;
}

static long vaccel_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	return 0;
}

static int vaccel_mmap(struct inode *inode, struct file *file) {
	return 0;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = vaccel_open,
	.unlocked_ioctl = vaccel_ioctl,
	.release = vaccel_release,
	.unlocked_ioctl = vaccel_ioctl,
};

static int __init vaccel_init(void) {
	printk(KERN_INFO "vAccel-Lab: initializing vaccel driver\n");

	int err;
	dev_t dev;

	// allocate chardev region then assign major num
	err = alloc_chrdev_region(&dev, 0, 1, "vaccel");

	major_num = MAJOR(dev);
	printk(KERN_INFO "vAccel-Lab: major number assigned to %d\n", major_num);

	vaccel_class = class_create(CLASS_NAME);
	if (IS_ERR(vaccel_class)) {
		err = PTR_ERR(vaccel_class);
		goto err_unregister;
	}

	vaccel_cdev = cdev_alloc();
	if (!vaccel_cdev) {
		err = -ENOMEM;
		goto err_destroy_class;
	}

	cdev_init(vaccel_cdev, &fops);
	err = cdev_add(vaccel_cdev, MKDEV(major_num, 0), 1);
	if (err)
		goto err_del_cdev;

	device_create(vaccel_class, NULL, MKDEV(major_num, 0), NULL, DEVICE_NAME);

	kernel_buffer = kmalloc(SHARED_MEMORY_SIZE, GFP_KERNEL);
	if (!kernel_buffer) {
		err = -ENOMEM;
		goto err_destroy_device;
	}
	for (int i = 0; i < SHARED_MEMORY_SIZE; i += PAGE_SIZE)
		SetPageReserved(virt_to_page(kernel_buffer+i));

	return 0;

err_destroy_device:
	device_destroy(vaccel_class, MKDEV(major_num, 0));
err_del_cdev:
	cdev_del(vaccel_cdev);
err_destroy_class:
	class_destroy(vaccel_class);
err_unregister:
	unregister_chrdev_region(MKDEV(major_num, 0), 1);
	return err;
}

static void __exit vaccel_exit(void) {
	for (int i = 0; i < SHARED_MEMORY_SIZE; i += PAGE_SIZE)
		ClearPageReserved(virt_to_page(kernel_buffer + i));

	kfree(kernel_buffer);
	device_destroy(vaccel_class, MKDEV(major_num, 0));
	if (vaccel_cdev)
		cdev_del(vaccel_cdev);
	class_unregister(vaccel_class);
	class_destroy(vaccel_class);
	unregister_chrdev_region(MKDEV(major_num, 0), MINORMASK);
}

module_init(vaccel_init);
module_exit(vaccel_exit);
