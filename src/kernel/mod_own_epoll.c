/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2013 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

/* #define DEBUG */
#include <linux/module.h> /* for MODULE_*, module_* */
#include <linux/fs.h> /* for fops */
#include <linux/device.h> /* for device support */
/* #define DO_DEBUG */
#include "kernel_helper.h" /* our own helper */
#include "shared.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Veltzer");
MODULE_DESCRIPTION("device that does something like WaitForMultipleObjects");

/* static data */
static struct device *my_device;

/* fops */

/*
* This is the ioctl implementation.
*/
static long kern_unlocked_ioctl(struct file *filp, unsigned int cmd,
		unsigned long arg) {
	PR_INFO("start");
	switch (cmd) {
	case IOCTL_EPOLL_WAIT:
		PR_INFO("in WAIT");
		break;
	case IOCTL_EPOLL_WAKE:
		PR_INFO("in WAKE");
		break;
	}
	return 0;
}

/*
* The file operations structure.
*/
static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = kern_unlocked_ioctl,
};

#include "device.inc"
