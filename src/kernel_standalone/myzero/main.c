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

#include<linux/module.h> // for MODULE_*, module_*
#include<linux/fs.h> // for fops
#include<linux/device.h> // for class_create
#include<linux/slab.h> // for kmalloc
#include<linux/uaccess.h> // for copy_to_user, access_ok
#include<linux/gfp.h> // for get_zeroed_page
#include<linux/cdev.h> // for cdev_*
#include<linux/sched.h> // for cond_resched

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("A simple implementation for something like /dev/zero");

/*
* This is an implementation of a kernel module which exposes a /dev/zero like
* device to user space. Whenever someone reads from the device the buffer passed
* to kernel gets filled with zeros.
* If you want to see the real implementation in the kernel see:
* $KERNEL_SOURCES/drivers/char/mem.c
*
* TODO:
*/

// how many minors do we need ?
const int MINOR_COUNT=1;

// these are the actual operations

static int open_zero(struct inode * inode, struct file * file) {
	//char* p=(char*)kmalloc(PAGE_SIZE, GFP_KERNEL);
	//memset(p,0,PAGE_SIZE);
	//file->private_data=(void*)p;
	file->private_data=(void*)get_zeroed_page(GFP_KERNEL);
	pr_info("all is ok and buffer is %p\n",file->private_data);
	return 0;
}

static ssize_t read_zero(struct file * file, char __user * buf, size_t count, loff_t *ppos) {
	ssize_t remaining;
	// do the access checking right at the start so that we would not start zeroing
	// the users pages and only then find out that the buffer is off the edge...
	if (!access_ok(VERIFY_WRITE, buf, count))
		return -EFAULT;

	remaining=count;
	while(remaining) {
		ssize_t curr=min(remaining,(ssize_t)PAGE_SIZE);
		/*
		if(copy_to_user(buf,file->private_data,curr)) {
			return -EFAULT;
		} else {
			buf+=curr;
			remaining-=curr;
		}
		*/
		// very efficient way of clearing user memory once it has been verified...
		__clear_user(buf,curr);
		buf+=curr;
		remaining-=curr;
		// allow for voluntary pre-emption
		cond_resched();
	}
	*ppos+=count;
	return count;
}

static int release_zero(struct inode* inode,struct file* file) {
	//kfree(file->private_data);
	free_page((unsigned long)file->private_data);
	return 0;
}

// this is the operations table
static const struct file_operations zero_fops={
	.owner=THIS_MODULE,
	.open=open_zero,
	.read=read_zero,
	.release=release_zero,
};

// this variable will store the class
static struct class *my_class;
// this variable will store the device
static struct device *my_device;
// this variable will hold our cdev struct
static struct cdev cdev;
// this is the first dev_t allocated to us...
static dev_t first_dev;
// this is our first minor
static int first_minor=0;

static int zero_init(void) {
	int err=0;
	// allocate our own range of devices
	if((err=alloc_chrdev_region(&first_dev, first_minor, MINOR_COUNT, THIS_MODULE->name))) {
		pr_err("cannot alloc_chrdev_region");
		goto err_final;
	}
	pr_info("allocated the region\n");
	// add the cdev structure
	cdev_init(&cdev, &zero_fops);
	if((err=cdev_add(&cdev, first_dev, MINOR_COUNT))) {
		pr_err("cannot cdev_add\n");
		goto err_dealloc;
	}
	pr_info("added the cdev\n");
	// this is creating a new class (/proc/devices)
	my_class=class_create(THIS_MODULE,THIS_MODULE->name);
	if(IS_ERR(my_class)) {
		pr_err("failed in class_create\n");
		err=PTR_ERR(my_class);
		goto err_cdev_del;
	}
	pr_info("created the class\n");
	// and now lets auto-create a /dev/ node
	my_device=device_create(my_class,NULL,first_dev,NULL,"my%s",THIS_MODULE->name);
	if(IS_ERR(my_device)) {
		pr_err("failed in device_create\n");
		err=PTR_ERR(my_device);
		goto err_class;
	}
	pr_info("created the device\n");
	pr_info("loaded ok\n");
	return 0;
//err_device:
	device_destroy(my_class, first_dev);
err_class:
	class_destroy(my_class);
err_cdev_del:
	cdev_del(&cdev);
err_dealloc:
	unregister_chrdev_region(first_dev, MINOR_COUNT);
err_final:
	return err;
}

static void zero_exit(void) {
	device_destroy(my_class, first_dev);
	class_destroy(my_class);
	cdev_del(&cdev);
	unregister_chrdev_region(first_dev, MINOR_COUNT);
	pr_info("unloaded ok\n");
}

module_init(zero_init);
module_exit(zero_exit);