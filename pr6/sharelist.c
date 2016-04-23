#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/rculist.h>
#include <linux/delay.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/kthread.h>

static struct task_struct *tskwriter;
static struct task_struct *tskreader1;
static struct task_struct *tskreader2;

struct ShareData{
  int data;
  struct list_head list;
};


LIST_HEAD(head);



static int thread_writer(void *d)
{
  int count = 0;
  while (!kthread_should_stop())
  {
    if (count<=50){
      struct ShareData *p = kmalloc(sizeof(*p), GFP_KERNEL);
      p->data = count;
      list_add_rcu(&p->list,&head);
      printk(KERN_EMERG "thread_writer %d times\n",count++);
    }
    msleep(1000);
  }
  return 0;
}

static void thread_reader(void *d)
{
  int count = 0;
  struct ShareData *p = NULL;
  while (!kthread_should_stop())
  {
    if (count<50){
      count++;
      rcu_read_lock();
      list_for_each_entry(p,&head,list){
        if (p)
        {
          printk(KERN_EMERG"reader = %d ; data = %d \n",current->pid,p->data);
        }
      };
      rcu_read_unlock();
    }
    msleep(1000);
  }

}



static int __init start_module(void)
{
  tskwriter = kthread_run(thread_writer,NULL,"%s","writer");
  if( IS_ERR(tskwriter) )
  {
    printk(KERN_EMERG "Creating Writer Failed!");
  }
  else
  {
    printk(KERN_EMERG "Creating Writer OK!");
  }

  tskreader1 = kthread_run(thread_reader,NULL,"%s","reader1");
  if( IS_ERR(tskreader1) )
  {
    printk(KERN_EMERG "Creating tskreader1 Failed!");
  }
  else
  {
    printk(KERN_EMERG "Creating tskreader1 OK!");
  }

  tskreader2 = kthread_run(thread_reader,NULL,"%s","reader2");
  if( IS_ERR(tskreader2) )
  {
    printk(KERN_EMERG "Creating tskreader2 Failed!");
  }
  else
  {
    printk(KERN_EMERG "Creating tskreader2 OK!");
  }
  return 0;
}

static int __exit end_module(void)
{
  if (!IS_ERR(tskwriter))
  {
    kthread_stop(tskwriter);
    printk(KERN_EMERG "Stopping tskwriter OK!");
  }
  if (!IS_ERR(tskreader1))
  {
    kthread_stop(tskreader1);
    printk(KERN_EMERG "Stopping tskreader1 OK!");
  }
  if (!IS_ERR(tskreader2))
  {
    kthread_stop(tskreader2);
    printk(KERN_EMERG "Stopping tskreader2 OK!");
  }

}

module_init(start_module);
module_exit(end_module);
