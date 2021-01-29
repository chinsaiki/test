#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define MAX_LEN       4096
static struct proc_dir_entry *proc_entry;
static char *info;
static int len, temp;

ssize_t read_proc(struct file *f, char *user_buf, size_t count, loff_t *off )
{
	//copy data from kernel space to user space
    if(count > temp)
    {
        count = temp;
    }
    
    temp = temp - count;
    copy_to_user( user_buf, info, count);
    
    if(count == 0)
        temp = len;
    
	printk(KERN_INFO "procfs_read: read %lu bytes\n", count);
	return count;
}


ssize_t write_proc(struct file *f, const char *user_buf, size_t count, loff_t *off)
{
	//copy data from user space and save it in the my proc entry
  
    copy_from_user( info, user_buf, count );
    len = count;
    temp = len;
    
    
	printk(KERN_INFO "procfs_write: write %lu bytes\n", count);
	return count;
}


struct file_operations proc_fops = {
	read: read_proc,
	write: write_proc
};


int init_module1( void )
{
	int ret = 0;
    
	// allocated memory space for the proc entry
	info = (char *)vmalloc( MAX_LEN );
	memset( info, 0, MAX_LEN );
    //create the entry
    proc_entry = proc_create("myproc", 0, NULL, &proc_fops);
    printk(KERN_INFO "myproc created.\n");
	

	return ret;
}


void cleanup_module1( void )
{
	remove_proc_entry("myproc",NULL);

	printk(KERN_INFO "test_proc deleted.\n");
	kfree(info);
}



MODULE_LICENSE("GPL");
module_init(init_module1);
module_exit(cleanup_module1);
