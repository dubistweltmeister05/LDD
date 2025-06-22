#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
// this is a pseudo char device
#define DEV_MEM_SIZE 512
char device_buffer[DEV_MEM_SIZE];

#define pr_fmt(fmt) "%s :" fmt, __func__

// give it something to hold the device number in
dev_t device_number;

// Cdev variable
struct cdev pcd_cdev;

loff_t pcd_lseek(struct file *filp, loff_t offset, int whence)
{
    loff_t temp;

    pr_info("lseek requested \n");
    pr_info("Current value of the file position = %lld\n", filp->f_pos);

    switch (whence)
    {
    case SEEK_SET:
        if ((offset > DEV_MEM_SIZE) || (offset < 0))
            return -EINVAL;
        filp->f_pos = offset;
        break;
    case SEEK_CUR:
        temp = filp->f_pos + offset;
        if ((temp > DEV_MEM_SIZE) || (temp < 0))
            return -EINVAL;
        filp->f_pos = temp;
        break;
    case SEEK_END:
        temp = DEV_MEM_SIZE + offset;
        if ((temp > DEV_MEM_SIZE) || (temp < 0))
            return -EINVAL;
        filp->f_pos = temp;
        break;
    default:
        return -EINVAL;
    }

    pr_info("New value of the file position = %lld\n", filp->f_pos);

    return filp->f_pos;
}

ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("read requested for %zu bytes \n", count);
    pr_info("current file position is = %lld", *f_pos);

    /*1. Adjust the count*/
    if ((*f_pos + count) > DEV_MEM_SIZE)
        count = DEV_MEM_SIZE - *f_pos;

    /*2. Copy to the user*/
    if (copy_to_user(buff, &device_buffer[*f_pos], count))
    {
        return -EFAULT;
    }

    /*3. Update the current file position*/
    *f_pos += count;

    pr_info("Number of bytes read are = %zu", count);
    pr_info("updated file position is = %lld", *f_pos);

    /*return the number of bytes that have been read*/
    return count;
}

ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("Write requested for %zu bytes \n", count);
    pr_info("current file position is = %lld", *f_pos);

    /*1. Adjust the count*/
    if ((*f_pos + count) > DEV_MEM_SIZE)
        count = DEV_MEM_SIZE - *f_pos;

    if (!count)
        return -ENOMEM;

    /*2. Copy from the user*/
    if (copy_from_user(&device_buffer[*f_pos], buff, count))
    {
        return -EFAULT;
    }

    /*3. Update the current file position*/
    *f_pos += count;

    pr_info("Number of bytes written are = %zu", count);
    pr_info("updated file position is = %lld", *f_pos);

    /*return the number of bytes that have been written*/
    return count;
}

int pcd_open(struct inode *p_inode, struct file *filp)
{
    pr_info("Open was successful\n");
    return 0;
}

int pcd_release(struct inode *p_inode, struct file *filp)
{
    pr_info("Close was successful\n");
    return 0;
}

/*Let's define the file operations*/
struct file_operations pcd_fops = {
    .open = pcd_open,
    .write = pcd_write,
    .read = pcd_read,
    .release = pcd_release,
    .llseek = pcd_lseek,
    .owner = THIS_MODULE,
};

struct class *class_pcd;
struct device *device_pcd;
static int __init
pcd_module_init(void)
{
    /*1. Dynamically allocate the device number*/
    alloc_chrdev_region(&device_number, 0, 1, "PCD_devices");
    pr_info(" Device Number <Major>:<Minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

    /*2. Let's initialize the cdev structure */
    cdev_init(&pcd_cdev, &pcd_fops);

    /*3. Register the device (a cdev structure) with VFS*/
    pcd_cdev.owner = THIS_MODULE; // necessary to prevent accidental unloading
    cdev_add(&pcd_cdev, device_number, 1);

    /*4. Create device class under /sys/class */
    class_pcd = class_create("pcd_class");
    /*5. Populete the sysfs with device info*/
    device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd_devices");
    pr_info("Module Init is successful!\n");
    return 0;
}

static void __exit pcd_driver_cleanup(void)
{
    device_destroy(class_pcd, device_number);
    class_destroy(class_pcd);
    cdev_del(&pcd_cdev);
    unregister_chrdev_region(device_number, 1);
    pr_info("module unloaded\n");
}

module_init(pcd_module_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RONIN");
MODULE_DESCRIPTION("A SIMPLE HELLO-WRLD MODULE TO TEST THE WATERS");
MODULE_INFO(board, "BeagleBone Black Rev C");
