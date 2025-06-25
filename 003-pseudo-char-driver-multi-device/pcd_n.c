#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/kernel.h>

#define pr_fmt(fmt) "%s :" fmt, __func__

#define NO_OF_DEVICES 4

// These are the memory sizes for each PCD device
#define MEM_SIZE_MAX_PCDEV1 1024
#define MEM_SIZE_MAX_PCDEV2 1024
#define MEM_SIZE_MAX_PCDEV3 1024
#define MEM_SIZE_MAX_PCDEV4 1024

// These are the banks for all PCD devices
char device_buffer_pcdev1[MEM_SIZE_MAX_PCDEV1];
char device_buffer_pcdev2[MEM_SIZE_MAX_PCDEV2];
char device_buffer_pcdev3[MEM_SIZE_MAX_PCDEV3];
char device_buffer_pcdev4[MEM_SIZE_MAX_PCDEV4];

/*Defining a device's private data structure*/
struct pcdev_priv_data
{
    char *buffer;
    unsigned size;
    const char *sr_no;
    int perm;
    struct cdev cdev;
};

/*Defining a driver's private data structure*/
struct pcdrv_priv_data
{
    int total_devices;
    dev_t device_number;
    struct class *class_pcd;
    struct device *device_pcd;
    struct pcdev_priv_data pcdev_data[NO_OF_DEVICES];
};

struct pcdrv_priv_data pcdrv_data = {
    .total_devices = NO_OF_DEVICES,
    .pcdev_data = {
        [0] = {
            .buffer = device_buffer_pcdev1,
            .size = MEM_SIZE_MAX_PCDEV1,
            .sr_no = "PCDEV1XYZ",
            .perm = 0x01, /*RDONLY*/
        },
        [1] = {
            .buffer = device_buffer_pcdev2, .size = MEM_SIZE_MAX_PCDEV2, .sr_no = "PCDEV2XYZ", .perm = 0x10, /*WRONLY*/
        },
        [2] = {
            .buffer = device_buffer_pcdev3, .size = MEM_SIZE_MAX_PCDEV3, .sr_no = "PCDEV3XYZ", .perm = 0x11, /*RDWR*/
        },
        [3] = {
            .buffer = device_buffer_pcdev4, .size = MEM_SIZE_MAX_PCDEV4, .sr_no = "PCDEV4XYZ", .perm = 0x11, /*RDWR*/
        },
    }};

loff_t
pcd_lseek(struct file *filp, loff_t offset, int whence)
{

    struct pcdev_priv_data *pcdev_data = (struct pcdev_priv_data *)filp->private_data;
    int MAX_SIZE = pcdev_data->size;
    loff_t temp;

    pr_info("lseek requested \n");
    pr_info("Current value of the file position = %lld\n", filp->f_pos);

    switch (whence)
    {
    case SEEK_SET:
        if ((offset > MAX_SIZE) || (offset < 0))
            return -EINVAL;
        filp->f_pos = offset;
        break;
    case SEEK_CUR:
        temp = filp->f_pos + offset;
        if ((temp > MAX_SIZE) || (temp < 0))
            return -EINVAL;
        filp->f_pos = temp;
        break;
    case SEEK_END:
        temp = MAX_SIZE + offset;
        if ((temp > MAX_SIZE) || (temp < 0))
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
    struct pcdev_priv_data *pcdev_data = (struct pcdev_priv_data *)filp->private_data;
    int MAX_SIZE = pcdev_data->size;
    pr_info("read requested for %zu bytes \n", count);
    pr_info("current file position is = %lld", *f_pos);

    /*1. Adjust the count*/
    if ((*f_pos + count) > MAX_SIZE)
        count = MAX_SIZE - *f_pos;

    /*2. Copy to the user*/
    if (copy_to_user(buff, pcdev_data->buffer + *(f_pos), count))
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

    struct pcdev_priv_data *pcdev_data = (struct pcdev_priv_data *)filp->private_data;
    int MAX_SIZE = pcdev_data->size;
    pr_info("Write requested for %zu bytes \n", count);
    pr_info("current file position is = %lld", *f_pos);

    /*1. Adjust the count*/
    if ((*f_pos + count) > MAX_SIZE)
        count = MAX_SIZE - *f_pos;

    if (!count)
        return -ENOMEM;

    /*2. Copy from the user*/
    if (copy_from_user(&pcdev_data->buffer + (*f_pos), buff, count))
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

int pcd_open(struct inode *inode, struct file *filp)
{
    int ret;

    int minor_n;

    struct pcdev_priv_data *pcdev_data;

    /*find out on which device file open was attempted by the user space */

    minor_n = MINOR(inode->i_rdev);
    pr_info("minor access = %d\n", minor_n);

    /*get device's private data structure */
    pcdev_data = container_of(inode->i_cdev, struct pcdev_priv_data, cdev);

    /*to supply device private data to other methods of the driver */
    filp->private_data = pcdev_data;
    /*3. check permissions*/

    // ret = check_permission();
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

static int __init
pcd_module_init(void)
{

    int ret;
    int i;
    /*1. Dynamically allocate the device number*/
    ret = alloc_chrdev_region(&pcdrv_data.device_number, 0, NO_OF_DEVICES, "PCD_devices");
    if (ret < 0)
        goto out;
    for (int i = 0; i < NO_OF_DEVICES; i++)
    {
        pr_info(" Device Number <Major>:<Minor> = %d:%d\n", MAJOR(pcdrv_data.device_number + i), MINOR(pcdrv_data.device_number + 1));
    }

    pcdrv_data.class_pcd = class_create("pcd_class");
    if (IS_ERR(pcdrv_data.class_pcd))
    {
        pr_info("pr error\n");
        ret = PTR_ERR(pcdrv_data.class_pcd);
        goto cdev_del;
    }

    for (i = 0; i < NO_OF_DEVICES; i++)
    {
        /*2. Let's initialize the cdev structure */
        cdev_init(&pcdrv_data.pcdev_data[i].cdev, &pcd_fops);

        /*3. Register the device (a cdev structure) with VFS*/
        pcdrv_data.pcdev_data[i].cdev.owner = THIS_MODULE; // necessary to prevent accidental unloading

        ret = cdev_add(&pcdrv_data.pcdev_data[i].cdev, pcdrv_data.device_number + i, 1);
        if (ret < 0)
            goto unreg_chrdev;
        /*4. Create device class under /sys/class */

        /*5. Populete the sysfs with device info*/
        pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, pcdrv_data.device_number + i, NULL, "pcdev-%d", i);
        if (IS_ERR(pcdrv_data.device_pcd))
        {
            pr_err("Device create failed\n");
            ret = PTR_ERR(pcdrv_data.device_pcd);
            goto class_del;
        }
    }

    pr_info("Module init was successful\n");

    return 0;

cdev_del:
class_del:
    for (; i >= 0; i--)
    {
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number + i);
        cdev_del(&pcdrv_data.pcdev_data[i].cdev);
    }
    class_destroy(pcdrv_data.class_pcd);

unreg_chrdev:
    unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);
out:
    pr_info("Module insertion failed\n");
    return ret;
}

static void __exit pcd_driver_cleanup(void)
{
#if 0
    device_destroy(class_pcd, device_number);
    class_destroy(class_pcd);
    cdev_del(&pcd_cdev);
    unregister_chrdev_region(device_number, 1);
    pr_info("module unloaded\n");
#endif
}

module_init(pcd_module_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RONIN");
MODULE_DESCRIPTION("A PSEUDO-CHAR DRIVER TO HANDLE MULTIPLE CHAR DEVICES");
MODULE_INFO(board, "BeagleBone Black Rev C");
