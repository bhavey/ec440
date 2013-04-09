  After compiling module and user-level program, first 

       mknod /dev/mytimer c 61 0

  to make the timer device under /dev


  Then under km 

       insmod mytimer.ko

  to load the module


  Last, under ul, from the user-level

       ./ktimer -s [num] [message] 

  to specify the time offset ( number of seconds from the current time) and the message to be printed on the console upon expiration. 

       ./ktimer -l 

  to list to stdout the expiration time of the registered timers and the messages that are set to be printed upon the corresponding timer expiration. 

  