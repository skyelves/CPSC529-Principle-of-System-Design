#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ke Wang");
MODULE_DESCRIPTION("Hello World Linux module.");
MODULE_VERSION("0.01");

static int __init hello_world_init(void) {
	printk(KERN_INFO "Hello World!\n");
	return 0;
}
static void __exit hello_world_exit(void) {
	printk(KERN_INFO "Goodbye World!\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);