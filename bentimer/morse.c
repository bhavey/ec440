//Morse code LED module for Raspberry Pi

#include <linux/module.h>
#include <linux/configfs.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/vt.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>
MODULE_DESCRIPTION("Example module illustrating the use of Keyboard LEDs.");
MODULE_LICENSE("GPL");
struct timer_list my_timer;
int timer_on = 0;
struct tty_driver *my_driver;
char morse_status = 0;
#define DIT HZ/5 //dot
#define DAH HZ*4/5 //dash
#define SPACE HZ*2/5 //space

//The dit function.
static void Dit(unsigned long ptr) {
	timer_on=1; //Lock the function.
        int *pstatus = (int *)ptr;
        if (*pstatus == 0) {
		*pstatus = 1;
		printk(KERN_ERR "Dot\n");
		my_timer.expires = jiffies + DAH; //Was blink delay
		add_timer(&my_timer);
	} else if (*pstatus == 1) {
		printk(KERN_ERR "Off\n");
		*pstatus = 2;
		my_timer.expires = jiffies + SPACE; //Was blink delay
		add_timer(&my_timer);
	} else  if (*pstatus == 2) {
		printk(KERN_ERR "Done\n");
		*pstatus = 0;
		timer_on=0;
	}
}
static void Dot(unsigned long ptr) {

}

static void Off(unsigned long ptr) {

}

static int __init kbleds_init(void) {
        int i;
        printk(KERN_ERR "kbleds: loading\n");
        printk(KERN_ERR "kbleds: fgconsole is %x\n", fg_console);
        for (i = 0; i < MAX_NR_CONSOLES; i++) {
                if (!vc_cons[i].d)
                        break;
                printk(KERN_ERR "poet_atkm: console[%i/%i] #%i, tty %lx\n", i,
                       MAX_NR_CONSOLES, vc_cons[i].d->vc_num,
                       (unsigned long)vc_cons[i].d->port.tty);
        }
        printk(KERN_ERR "kbleds: finished scanning consoles\n");
        my_driver = vc_cons[fg_console].d->port.tty->driver;
        printk(KERN_ERR "kbleds: tty driver magic %x\n", my_driver->magic);
        init_timer(&my_timer);
        my_timer.function = dit;
        my_timer.data = (unsigned long)&morse_status;
        my_timer.expires = jiffies + DIT;
        add_timer(&my_timer);
        return 0;
}
static void __exit kbleds_cleanup(void) {
        printk(KERN_ERR "kbleds: unloading...\n");
        del_timer(&my_timer);
}
module_init(kbleds_init);
module_exit(kbleds_cleanup);
