/*
	This file is part of the linuxapi project.
	Copyright (C) 2011, 2012 Mark Veltzer <mark.veltzer@gmail.com>

	The linuxapi package is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The linuxapi package is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with the GNU C Library; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307 USA.
*/

//#define DEBUG
#include <linux/module.h> // for MODULE_*. module_*
#include <linux/printk.h> // for pr_*
//#define DO_DEBUG
#include "kernel_helper.h" // our own helper

/*
* This is a minimal module showing various logging functions
* I show only kernel logging functions.
* - Always prefer the pr_* functions, or better yet, the dev_* functions.
* - Notice that debug messages will not reach the log unless you define DEBUG
* at compile time.
*
* TODO:
* - show rate limited functions here (see printk.h for details).
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("module for showing logging functions");

void print_error_once_wrapper(void) {
	pr_err_once("this is an error that will not be repeated\n");
}

// our own functions
static int __init mod_init(void) {
	printk(KERN_EMERG "this is an emergency message");
	printk(KERN_ALERT "this is an alert message");
	printk(KERN_CRIT "this is a critical message");
	printk(KERN_ERR "this is a err message");
	printk(KERN_WARNING "this is a warning message");
	printk(KERN_NOTICE "this is a notice message");
	printk(KERN_INFO "this is an info message");
	printk(KERN_DEBUG "this is a debug message");
	printk(FW_BUG "this is a firmware bug message");
	printk(FW_WARN "this is a firmware warning message");
	printk(FW_INFO "this is a firmware info message");
	printk(HW_ERR "this is a hardware error message");
	pr_emerg("this is an emergency message\n");
	pr_alert("this is an alert message\n");
	pr_crit("this is a critical message\n");
	pr_err("this is an error message\n");
	pr_warning("this is a warning message\n");
	pr_warn("this is also a warning message\n");
	pr_notice("this is a notice message\n");
	pr_info("this is an info message\n");
	pr_devel("this is a development message\n");
	pr_debug("this is a debug message\n");
	print_error_once_wrapper();
	// lets dump the stack...
	dump_stack();
	return(0);
}


static void __exit mod_exit(void) {
	// this will not show any output...
	print_error_once_wrapper();
	dump_stack();
}

// declaration of init/cleanup functions of this module

module_init(mod_init);
module_exit(mod_exit);
