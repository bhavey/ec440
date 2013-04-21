//Morse code LED module for Raspberry Pi
static char *morse_table[] = {".-","-...","-.-.","-..", //A-D
    ".","..-.","--.","....","..",".---","-.-",".-..", //E-L
    "--","-.","---",".--.","--.-",".-.","...","-","..-", //M-U
    "...-",".--","-..-","-.--","--..", //V-Z
    ".----","..---","...--","....-",".....", //1-5
    "-....","--...","---..","----.","-----", //6-0
};

#include <linux/module.h>
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
struct tty_driver *my_driver;
char morse_in = 'a';
char morse_status = 0;
#define DIT HZ/5 //dot
#define DAH HZ*4/5 //dash
#define SPACE HZ*2/5 //space

//Note. Each time you set up a new timer, you need to consider how long
//you want *that* timer to last. As in, you want to always set the LED
//status for dots/dashes in the space preceeding them
static void Dit(char morse_in) {
	//Check for end of morse code for the character.
	if (morse_table[97-morse_in][morse_status/2]==NULL) {
		morse_status = 11; //No morse code requires more then 11.
		//turn off the light here!
		printk(KERN_ERR "We're here!\n");
	} else {
	        if (morse_status%2==0) { //Run a "space" between dashes/dots
			morse_status++;
			//Check what to print now!
			if (morse_table[97-morse_in][morse_status/2]=='.') {
				printk(KERN_ERR "Dot\n");
				my_timer.expires = jiffies + DIT;
			} else {
				printk(KERN_ERR "Dash\n");
				my_timer.expires = jiffies + DAH
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

static int __init kbleds_init(void) {
        printk(KERN_ERR "morse: loading\n");
        init_timer(&my_timer);
        my_timer.function = Dit;
        my_timer.data = morse_in;
        my_timer.expires = jiffies + DAH;
        add_timer(&my_timer);
        return 0;
}
static void __exit kbleds_cleanup(void) {
        printk(KERN_ERR "morse: unloading...\n");
        del_timer(&my_timer);
}
module_init(kbleds_init);
module_exit(kbleds_cleanup);
