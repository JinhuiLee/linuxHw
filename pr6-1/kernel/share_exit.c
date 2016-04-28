#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/rculist.h>
#include <linux/delay.h>
#include <linux/rcupdate.h>
#include <linux/kthread.h>
#include <linux/spinlock.h>

#include <linux/myhead.h>



asmlinkage long sys_share_exit(void)
{
  int i=0;
  running=0;
  msleep(1000);
  del_timer_sync(&mytimer);
  for (i=0;i<100;i++)
    destroy_workqueue(myqueue[i]);  
  return 0;
}
