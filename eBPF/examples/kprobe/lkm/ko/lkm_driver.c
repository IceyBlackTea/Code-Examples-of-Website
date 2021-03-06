#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define LICENSE "GPL"
#define AUTHOR "IceyBlackTea <One_Random@outlook.com>"
#define DESCRIPTION "A simple module example"
#define VERSION "1.00"

#define DEVICE_NAME "lkm_example"
#define EXAMPLE_MSG "Hello, World!\n"
#define MSG_BUFFER_LEN 128

/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static int major_num;
static int device_open_count = 0;
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;

/* This structure points to all of the device functions */
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/* When a process reads from our device, this gets called. */
static ssize_t device_read(struct file *flip, char __user *user_buffer, size_t size, loff_t *offset) {
    ssize_t len = size;

    if (len > MSG_BUFFER_LEN - *offset)
        len = MSG_BUFFER_LEN - *offset;

    if (len <= 0) return 0;

    if (copy_to_user(user_buffer, msg_ptr + *offset, len))
        return -EFAULT;

    *offset += len;
    return len;
}

/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *flip, const char __user *user_buffer, size_t size, loff_t *offset) {
    ssize_t len = size;

    if (len > MSG_BUFFER_LEN - *offset)
        len = MSG_BUFFER_LEN - *offset;

    if (len <= 0) return 0;

    if (copy_from_user(msg_ptr + *offset, user_buffer, len))
        return -EFAULT;

    *offset += len;
    return len;
}

/* Called when a process opens our device */
static int device_open(struct inode *inode, struct file *file) {
    // If device is open, return busy
    if (device_open_count) return -EBUSY;

    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

/* Called when a process closes our device */
static int device_release(struct inode *inode, struct file *file) {
    // Decrement the open counter and usage count.
    // Without this, the module would not unload.
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int __init lkm_example_init(void) {
    // Fill buffer with our message
    strncpy(msg_buffer, EXAMPLE_MSG, MSG_BUFFER_LEN);
    msg_ptr = msg_buffer;

    // Try to register character device
    major_num = register_chrdev(0, "lkm_example", &file_ops);
    if (major_num < 0) {
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    } else {
        printk(KERN_INFO "lkm_example module loaded with device major number %d\n", major_num);
        return 0;
    }
}

static void __exit lkm_example_exit(void) {
    // Remember ??? we have to clean up after ourselves.
    // Unregister the character device.
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Goodbye, World!\n");
}

/* Register module functions */
module_init(lkm_example_init);
module_exit(lkm_example_exit);

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
MODULE_VERSION(VERSION);