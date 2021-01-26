#include <linux/module.h> 
#include <linux/init.h>   
#include <linux/fs.h>   
#include <linux/device.h>  //device create and class create 
#include <asm/uaccess.h>	//copy to user / copy from user
#include <linux/interrupt.h>   
#include <linux/sched.h>   //wake_up_interruptible
#include <linux/gpio.h>   //gpio
#include <linux/irq.h>   //IRQ_TYPE_EDGE_FALLING

#define	BUTTON_MAJOR	239
#define DEV_NAME		"button"
#define BUF_SIZE		128
#define INT_PIN			25

static struct class *button_class ;
static volatile char ev_press = 0;
static DECLARE_WAIT_QUEUE_HEAD(button_wait_queue);

static irqreturn_t button_handler (int irq, void *dev_id) 
{ 
    printk("the irq number is: %d\n",irq); 
    ev_press = 1; 
    wake_up_interruptible(&button_wait_queue); 
    return IRQ_HANDLED; 
} 


static ssize_t button_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{	
	ssize_t status ;
	unsigned char data[BUF_SIZE] = {"RaspberryPi"};
	printk("Now entering %s() \n", __FUNCTION__ );
	wait_event_interruptible(button_wait_queue, ev_press);
	status = copy_to_user(user_buf,data,count);
	printk("user read data is %s\n",user_buf );
	ev_press = 0 ;
	return status ;
}

static ssize_t button_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	ssize_t	status ;
	unsigned char data[BUF_SIZE];
	printk("Now entering %s() \n", __FUNCTION__ );
	status = copy_from_user( data,user_buf,count);
	printk("user write data is %s\n",data);
	return  status ;
}

static int button_open(struct inode *inode, struct file *file)
{
	int err ;
	printk("Now entering %s() \n", __FUNCTION__ );
	err = request_irq(gpio_to_irq(INT_PIN), button_handler, IRQ_TYPE_EDGE_BOTH, "S0",NULL);
	return err ;
}

static int button_close(struct inode *inode, struct file *file)
{
	printk("Now entering %s() \n", __FUNCTION__ );
	free_irq(gpio_to_irq(INT_PIN), NULL );
	return 0 ;
}

struct file_operations button_ops = {
	.owner = THIS_MODULE ,
	.open = button_open ,
	.release = button_close ,
	.write = button_write ,
	.read = button_read ,
};

static int __init button_init(void){
	int status ;
	printk("Hello driver world\n");
	status = register_chrdev( BUTTON_MAJOR , DEV_NAME , &button_ops);
	if ( status < 0 ){
		printk("Failed to register char device - %d\n" , BUTTON_MAJOR );
		return status ;
	}

	button_class = class_create(THIS_MODULE , "button-class");
	if (IS_ERR(button_class)){
		unregister_chrdev( BUTTON_MAJOR , DEV_NAME );
	}

	device_create(button_class ,NULL , MKDEV(BUTTON_MAJOR , 0), NULL ,  "hubuyu" );
	return 0;
}
	 
static void __exit button_exit(void){
	printk("Goodbye driver world\n");
	device_destroy(button_class , MKDEV(BUTTON_MAJOR ,0));
    class_destroy(button_class);
    unregister_chrdev(BUTTON_MAJOR , DEV_NAME);

}

module_init(button_init);
module_exit(button_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("hubuyu");
MODULE_DESCRIPTION("Lesson 8-1 : gpio-key button interrupt ");

