#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include "consts.h"
#include "device_handlers.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rhydon");

static int dev_major = 0;
static struct cdev cdev;
static struct class* shadow_ssh_class = NULL;

static const struct file_operations fops = {
    .owner      = THIS_MODULE,
    .open       = device_open,
    .release    = device_close,
    .read       = device_read,
    .write       = device_write,
};

static int shadow_ssh_driver_init(void) {	
	int err;
    dev_t dev;
    printk(KERN_INFO "hello...\n");
    err = alloc_chrdev_region(&dev, 0, MINOR_VERSION, DEVICE_NAME);
    dev_major = MAJOR(dev);
    
    shadow_ssh_class = class_create(THIS_MODULE, DEVICE_NAME);

    cdev_init(&cdev, &fops);
    cdev_add(&cdev, MKDEV(dev_major, MINOR_VERSION), 1);
    device_create(shadow_ssh_class, NULL, MKDEV(dev_major, MINOR_VERSION), NULL, DEVICE_NAME);
    
	return 0;
}

static void shadow_ssh_driver_exit(void) {
	printk(KERN_WARNING "bye ...\n");
}

module_init(shadow_ssh_driver_init);
module_exit(shadow_ssh_driver_exit);