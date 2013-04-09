//New way of assigning to the file structure (with GCC version =>2.95)
struct file_operations fops = {
    read: device_read,
    write: device_write,
    open: device_open,
    release: device_release
};

//Old way, used for compatibility, is this:
struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

/* Each device is represented by a file structure, which is defined in linux/fs.h
     represents an abstract open 'file', not a file on disk.
 *An instance of struct file is commonly named filp. You'll also see it refered to
     as struct file file. Resist the temptation.
 *Device files don't use the entirety of the file header, only what's declared. */

//Register a driver with the kernel using register_chrdev from linux/fs.h:
//int register_chrdev(unsigned int major, const char *name,
//   struct file_operations *fops);
/*On the variables relevant to registering a device:
 *Major number tells which driver handles which device file,
 *minor number differentiates which device the driver is operating on, if the
     driver handles more than one device.
 *major is the major number you are requesting,
 *name is the name as it will appear in /proc/devices
 *file_operations *fops is a pointer to the file_operations table for your driver
 *Negative return value means the registration failed.
 *We don't pass minor number as the kernel doesn't care about it.
 *Passing "0" to major dynamically allocates the major number.
*/

/* After passing 0 to the major, we don't know the major # and therefore can't
     make the device file in advanced, there are 3 ways to circumvent this: 
 1. The driver can print the newly assigned number, and we hand make the
     device file
 2. Newly registered dev will have an entry in /proc/devices. Either make dev
     file by hand or make a bash script to read the file in and make the dev file
 3. Have our driver make the dev file using mknod after successful registration
     and rm during the call to cleanup_module.
*/

/*Can't allow the module to be rmmod'ed whenever root feels like it, b.c. using
 *the dev file could then result in running code for another module that was
 *placed in the same location as the original code.
 *There is a usage counter in the 3d field of /proc/modules. If this # isn't zero,
     rmmod will fail. The check is automatically performed by the system call
     sys_delete_module defined in linux/module.c. Can access counter with:
         try_module_get(THIS_MODULE) //increments use count
         module_put(THIS_MODULE) //decrements use count
 *Failing to keep an accurate usage count will make the module unloadable.
*/