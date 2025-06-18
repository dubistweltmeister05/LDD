#include <linux/module.h>
#include <linux/fs.h>

// this is a pseudo char device
#define DEV_MEM_SIZE 512
char device_buffer[DEV_MEM_SIZE];

// give it something to hold the device number in
dev_t device_number;
static int __init pcd_module_init(void)
{
    /*1. Dynamically allocate the device number*/
    alloc_chrdev_region(&device_number, 0, 1, "PCD");
    return 0;
}

static void __exit pcd_driver_cleanup(void)
{
}

module_init(pcd_module_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RONIN");
MODULE_DESCRIPTION("A SIMPLE HELLO-WRLD MODULE TO TEST THE WATERS");
MODULE_INFO(board, "BeagleBone Black Rev C");
