#include <linux/module.h>

 static int __init hello_wrld_init(void){
	pr_info("Hello Y'all! How about a GM tweet, but from the kernel instead? Cool, init?!?!\n");
 	return 0;
 }

static void __exit hello_wrld_clean(void){
	pr_info("Getting the hello wrld module the fuck upo outta the kernel\n");
}


module_init(hello_wrld_init);
module_exit(hello_wrld_clean);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR("RONIN");
MODULE_DESCRIPTION("A SIMPLE HELLO-WRLD MODULE TO TEST THE WATERS");
MODULE_INFO(board, "BeagleBone Black Rev C");
