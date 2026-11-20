// 在CLion中报错的原因是header路径的问题，默认用的是/usr/include

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define BUFFER_MAX    (10)
#define OK            (0)
#define ERROR         (-1)

struct cdev *gDev;
struct file_operations *gFile;
dev_t  devNum;
unsigned int count = 1;
int reg_major  =  232;
int reg_minor =   0;
char *buffer;
int flag = 0;
int hello_open(struct inode *p, struct file *f)
{
    printk(KERN_EMERG"hello_open\r\n");
    return 0;
}

ssize_t hello_write(struct file *f, const char __user *u, size_t s, loff_t *l)
{
printk(KERN_EMERG"hello_write\r\n");
return 0;
}
ssize_t hello_read(struct file *f, char __user *u, size_t s, loff_t *l)
{
printk(KERN_EMERG"hello_read\r\n");
return 0;
}
int hello_init(void)
{
    // 创建设备号，传入主次设备号
    devNum = MKDEV(reg_major, reg_minor);
    // 注册设备号，count表示需要从devNum开始注册几个设备号
    if(OK == register_chrdev_region(devNum, count, "helloworld")){
        printk(KERN_EMERG"register_chrdev_region ok \n");
    }else {
        printk(KERN_EMERG"register_chrdev_region error n");
        return ERROR;
    }
    printk(KERN_EMERG" hello driver init \n");
    // 创建字符设备对象
    gDev = kzalloc(sizeof(struct cdev), GFP_KERNEL);
    // 创建文件操作函数集对象
    gFile = kzalloc(sizeof(struct file_operations), GFP_KERNEL);
    // 设置函数集中的函数
    gFile->open = hello_open;
    gFile->read = hello_read;
    gFile->write = hello_write;
    gFile->owner = THIS_MODULE;
    // 字符设备初始化，会绑定设备和文件操作函数集的关系
    cdev_init(gDev, gFile);
    // 注册设备，会绑定设备号和设备对象
    cdev_add(gDev, devNum, 3);
    return 0;
}

void __exit hello_exit(void)
{
    cdev_del(gDev);
    unregister_chrdev_region(devNum, count);
    return;
}

// 模块初始化函数
module_init(hello_init);
// 模块退出函数
module_exit(hello_exit);
MODULE_LICENSE("GPL");