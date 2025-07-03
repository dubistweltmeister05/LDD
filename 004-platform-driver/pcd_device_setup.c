#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt, __func__

//Create 2 platform data
struct pcdev_platform_data pcdev_pdata[2]= {
    [0]={.size = 512, .perm = RDWR, .sr_no = "PCDEV_ABC"},
    [1] = {.size = 2048, .perm  = RDWR, .sr_no = "PCDEV_XYZ"},
}; 
void pcdev_release(struct device *dev){
    pr_info("Device Released\n");
}

// Create 2 platform devices

struct platform_device platform_pcdev_1 = {
    .name = "pseudo-char-device",
    .id = 0,
    .dev = {
        .platform_data = &pcdev_pdata[0],
        .release = pcdev_release
    } 
};

struct platform_device platform_pcdev_2 = {
    .name = "pseudo-char-device",
    .id = 1,
    .dev = {
        .platform_data = &pcdev_pdata[1],
        .release = pcdev_release
    }
};

static int __init pcdev_platform_init(void)
{
    // register the device
    platform_device_register(&platform_pcdev_1);
    platform_device_register(&platform_pcdev_2);

    pr_info("Module Inserted \n");
    return 0;
}

void __exit pcdev_platform_exit(void)
{
    platform_device_unregister(&platform_pcdev_1);
    platform_device_unregister(&platform_pcdev_2);

    pr_info("DEvice Setup module removed\n");
};

module_init(pcdev_platform_init);
module_exit(pcdev_platform_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kshitij Neeraj Vaze");
MODULE_DESCRIPTION("A module to register platform devices");