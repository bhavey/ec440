//Morse code LED module for Raspberry Pi
static char *morse_table[] = {".-","-...","-.-.","-..", //A-D
    ".","..-.","--.","....","..",".---","-.-",".-..", //E-L
    "--","-.","---",".--.","--.-",".-.","...","-","..-", //M-U
    "...-",".--","-..-","-.--","--..", //V-Z
    ".----","..---","...--","....-",".....", //1-5
    "-....","--...","---..","----.","-----", //6-0
};

#include <linux/kernel.h>	//We're doing kernel work
#include <linux/module.h>	//on a module
#include <asm/uaccess.h>	//for get_user and put_user
#include <linux/fs.h>
#include <linux/configfs.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>
MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");
MODULE_LICENSE("GPL");
struct timer_list my_timer;
char morse_in = 'a';
char morse_status = 11;
#define DIT HZ/5 //dot
#define DAH HZ*6/5 //dash
#define SPACE HZ*2/5 //space

#include "bendev.h"
#define SUCCESS 0
#define DEVICE_NAME "ben_dev"
#define BUF_LEN 1 //Buffer size is 31 characters!

//Is the device open right now? Used to prevent
//concurent access into the same device
static int Device_Open = 0;

//The message the device will give when asked
static char Message[BUF_LEN];

/* How far did the process reading the message get?
 * Useful if the message is larger than the size of the
 * buffer we get to fill in device_read. */
static char *Message_Ptr;

//Note. Each time you set up a new timer, you need to consider how long
//you want *that* timer to last. As in, you want to always set the LED
//status for dots/dashes in the space preceeding them
static void Dit(char morse_in) {
	if (morse_status == 11) {
		printk(KERN_ERR "morse_in: %c\n",morse_in);
		morse_status=0; //Reset morse_status
	//Check for end of morse code for the character.
	} if (morse_table[(int)morse_in][morse_status/2]==NULL) {
		morse_status = 11; //No morse code requires more then 11.
		//turn off the light here!
		printk(KERN_ERR "Done!\n");
	} else {
	        if (morse_status%2==0) { //Run a "space" between dashes/dots
			morse_status++;
			//Check what to print now!
			if (morse_table[(int)morse_in][morse_status/2]=='.') {
				printk(KERN_ERR "Dot\n");
				my_timer.expires = jiffies + DIT;
			} else {
				printk(KERN_ERR "Dash\n");
				my_timer.expires = jiffies + DAH;
			}
			add_timer(&my_timer);
		} else { //The dot/dash is done, call a space.
			morse_status++;
			printk(KERN_ERR "Space\n");
			my_timer.expires = jiffies + SPACE;
			add_timer(&my_timer);
		}
	}
}

//This is called whenever a process attempts to open the device file
static int device_open(struct inode *inode, struct file *file) {
#ifdef DEBUG
    //Note: Need to use KERN_ERR b.c. KERN_INFO doesn't have high enough
    //priority to work on my computer. (Even in getty)
    printk(KERN_ERR "device_open(%p)\n", file);
#endif

    //We don't want to talk to two processes at the same time
    if (Device_Open)
        return -EBUSY;

        Device_Open++;
        //Initialize the message
        Message_Ptr = Message;
        try_module_get(THIS_MODULE);
        return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file) {
#ifdef DEBUG
        printk(KERN_ERR "device_release(%p,%p)\n", inode, file);
#endif

        //We're now ready for our next caller
        Device_Open--;

        module_put(THIS_MODULE);
        return SUCCESS;
}

//This function is called whenever a process which has already opened the
//device file attempts to read from it.
static ssize_t device_read(struct file *file, //see include/linux/fs.h
    char __user * buffer, //buffer to be filled with data
    size_t length, //length of the buffer
    loff_t * offset) {
    //Number of bytes actually written to the buffer
    int bytes_read = 0;

#ifdef DEBUG
    printk(KERN_ERR "device_read(%p,%p,%d)\n", file, buffer, length);
#endif
    //If we're at the end of the message, return 0
    //(which signifies end of file)
    if (*Message_Ptr == 0)
        return 0;

    //Actually put the data into the buffer
    while (length && *Message_Ptr) {
        /* Because the buffer is in the user data segment,
        * not the kernel data segment, assignment wouldn't
        * work. Instead, we have to use put_user which
        * copies data from the kernel data segment to the
        * user data segment. */
        put_user(*(Message_Ptr++), buffer++);
        length--;
        bytes_read++;
    }

#ifdef DEBUG
    printk(KERN_ERR "Read %d bytes, %d left\n", bytes_read, length);
#endif
    //Read functions are supposed to return the number
    //of bytes actually inserted into the buffer
    return bytes_read;
}

//This function is called when somebody tries to
//write into our device file.
static ssize_t device_write(struct file *file, const char __user * buffer,
    size_t length, loff_t * offset) {

#ifdef DEBUG
    printk(KERN_ERR "device_write(%p,%s,%d)", file, buffer, length);
#endif

//    for (i = 0; i < length && i < BUF_LEN; i++)
//        get_user(Message[i], buffer + i);
    printk(KERN_ERR "Message before get user: %c\n",Message[0]);
    get_user(Message[0], buffer);
    printk(KERN_ERR "Buffer reads as: %c\n",buffer);
    printk(KERN_ERR "Message after get user: %c\n",Message[0]);
    Message[0]=buffer;

    Message_Ptr = Message;

    if (morse_status!=11)
	printk(KERN_ERR "Device busy.\n");
    else {
	//Check if accurate input!
	morse_in=Message[0];
	printk(KERN_ERR "Writing message in: %c\n", Message[0]);
	if ((morse_in>=48)&&(morse_in<=57)) {
	    //Number. Move to the proper index!
	    morse_in-=22;
	} else if ((morse_in>=65)&&(morse_in<=90)) {
	    morse_in-=65; //Adjust the indices for capital letters.
	} else if ((morse_in>=97)&&(morse_in<=122)) {
	    morse_in-=97; //Adjust the indices for lower letters.
	} else {
	    printk(KERN_ERR "Improper input. Please give a letter or #\n");
	}
	my_timer.expires = jiffies + DAH;
	add_timer(&my_timer);
    }
    return 0;
}

 /* This function is called whenever a process tries to do an ioctl on our
 * device file. We get two extra parameters (additional to the inode and file
 * structures, which all device functions get): the number of the ioctl called
 * and the parameter given to the ioctl function.
 *
 * If the ioctl is write or read/write (meaning output is returned to the
 * calling process), the ioctl call returns the output of this function. */

//This function call needed to be changed to conform to new ioctl header
static long device_ioctl(struct file *file, //see /user/include/linux/fs.h
    unsigned int ioctl_num, //number and param for ioctl
    unsigned long ioctl_param) {

    int i;
    char *temp;
    char ch;

    //Switch according to the ioctl called
    switch (ioctl_num) {
        case IOCTL_SET_MSG:
            //Receive a pointer to a message (in user space) and set that to be the
            //device's message.  Get the parameter given to ioctl by the process.
            temp = (char *)ioctl_param;

            //Find the length of the message
            get_user(ch, temp);
            for (i = 0; ch && i < BUF_LEN; i++, temp++)
                get_user(ch, temp);

            device_write(file, (char *)ioctl_param, i, 0);
                break;

        case IOCTL_GET_MSG:
            //Give the current message to the calling process,
            //the parameter we got is a pointer, fill it.
            i = device_read(file, (char *)ioctl_param, 99, 0);
            //Put a 0 at the end of the buffer so it will be properly terminated
            put_user('\0', (char *)ioctl_param + i);
            break;

        case IOCTL_GET_NTH_BYTE:
            //This ioctl is both input (ioctl_param) and
            //output (the return value of this function)
            return Message[ioctl_param];
            break;
        }

        return SUCCESS;
}

//Module Declarations


/* This structure will hold the functions to be called
 * when a process does something to the device we
 * created. Since a pointer to this structure is kept in
 * the devices table, it can't be local to
 * init_module. NULL is for unimplemented functions. */
struct file_operations Fops = {
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl, //The newer version of ioctl
    .open = device_open,
    .release = device_release,	//a.k.a. close
};

//Initialize the module - Register the character device
int init_module() {
    int ret_val;
    //Register the character device (atleast try)
    ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);

    //Negative values signify an error
    if (ret_val < 0) {
        printk(KERN_ALERT "%s failed with %d\n",
            "Sorry, registering the character device ", ret_val);
        return ret_val;
    }

    //Note: Using KERN_ERR here due to personal restrictions, not b.c.
    //that's the best posibile print option. Everything below KERN_ERR
    //priority fails to print due to some idiotic problems with my build.
    printk(KERN_ERR "%s The major device number is %d.\n",
        "Registeration is a success", MAJOR_NUM);
    printk(KERN_ERR "Create a device file with: \n");
    printk(KERN_ERR "mknod %s c %d MINOR_NUM\n", DEVICE_FILE_NAME, MAJOR_NUM);
    printk(KERN_ERR "Write to device with 'echo WORDS > %s.\n",DEVICE_FILE_NAME);
    printk(KERN_ERR "Read with 'cat %s\n",DEVICE_FILE_NAME);
    printk(KERN_ERR "morse: loading\n");
    init_timer(&my_timer);
    my_timer.function = Dit;
//    add_timer(&my_timer);
    return 0;
}

//Cleanup - unregister the appropriate file from /proc 
void cleanup_module() {
    //Unregister the device
    //Had to get rid of the error messages due to changes in the code.
    printk(KERN_ERR "morse: unloading...\n");
    del_timer(&my_timer);
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

//MODULE_LICENSE("GPL");
//MODULE_AUTHOR("Benjamin Havey, Sourced from Anil Kumar Pugalia");
//MODULE_DESCRIPTION("Banner message Driver");
