#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/rculist.h>
#include <linux/delay.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/spinlock.h>
#include <linux/list.h>

struct ShareData{
  int data;
  struct list_head list;
};


extern struct list_head head;
extern int g_count;
extern volatile int running;
extern struct work_struct my_work[100];
extern struct workqueue_struct* myqueue[100];

extern struct timer_list mytimer;
extern spinlock_t list_lock;



static int list_num(struct list_head* head)
{
  struct ShareData* p;
  int count=0;
  list_for_each_entry(p,head,list){
    count++;
  }
  return count;
}



static void my_func(void)
{
    while (running)
    {  
      
      spin_lock(&list_lock);
      int count=list_num(&head);
      spin_unlock(&list_lock);
      if  (count<800)
      {
        spin_lock(&list_lock);
        printk(KERN_EMERG "KernelThread: adding data(%d) to head\n",g_count);
        struct ShareData* p = kmalloc(sizeof(*p),GFP_KERNEL);
        p->data = g_count;
        g_count++;
        list_add(&p->list,&head);
        spin_unlock(&list_lock);
      }
      msleep(100);
    }
    printk(KERN_EMERG "KernelThread: Quitting!\n");
}

void mytimer_ok(unsigned long expires)
{
 
  printk("Timer: timer wake up! \n");
  mod_timer(&mytimer,jiffies+200);
  spin_lock(&list_lock);
  struct list_head *tail = (head.prev);
  if (tail!=&head){ 
    struct ShareData* p = list_entry(tail,typeof(*p),list);
    printk("Timer: deleting tail of list (%d)\n", p->data);
    list_del(tail);
  }
  spin_unlock(&list_lock);

}


static int __init start_module(void)
{
  
  init_timer(&mytimer);
  mytimer.expires=jiffies+20;
  mytimer.function=mytimer_ok;
  add_timer(&mytimer);

  int i;
  for (i=0;i<100;i++)
  {
    myqueue[i]=create_workqueue("my_queue");
    INIT_WORK(&my_work[i],my_func);
  }
  for (i=0;i<100;i++)
    queue_work(myqueue[i],&my_work[i]);
  return 0;
}

static int __exit end_module(void)
{
  int i;
  running=0;
  msleep(1000);
  del_timer_sync(&mytimer);
  for (i=0;i<100;i++)
    destroy_workqueue(myqueue[i]);
}

module_init(start_module);
module_exit(end_module);
MODULE_LICENSE("GPL");
