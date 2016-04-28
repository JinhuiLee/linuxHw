
struct ShareData{
  int data;
  struct list_head list;
};


LIST_HEAD(head);
int g_count=0;
volatile int running=1;
struct work_struct my_work[100];
struct workqueue_struct* myqueue[100];

struct timer_list mytimer;
DEFINE_SPINLOCK(list_lock);

EXPORT_SYMBOL(head);
EXPORT_SYMBOL(g_count);
EXPORT_SYMBOL(running);
EXPORT_SYMBOL(my_work);
EXPORT_SYMBOL(myqueue);
EXPORT_SYMBOL(mytimer);
EXPORT_SYMBOL(list_lock);
