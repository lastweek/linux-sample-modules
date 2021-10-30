#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/workqueue.h>

struct workqueue_struct *test_wq __read_mostly;
struct work_struct work;

static void work_func(struct work_struct *dummy)
{
	pr_info("I'm work_func, runningon CPU %d, with task name %s\n",
		smp_processor_id(), current->comm);
}

static int test_workqueue_init(void)
{
	pr_info("Module started..\n");

	/*
	 * WQ_UNBOUND is an important flag.
	 * It means don't bound to any CPU.
	 *
	 * This call will not necessarily create new threads.
	 * The queue_work() later might reuse an existing kworker.
	 */
	test_wq = alloc_workqueue("test", WQ_UNBOUND, 0);
	if (!test_wq) {
		pr_info("Fail to create the test workqueue.\n");
		return -ENOMEM;
	}

	INIT_WORK(&work, work_func);
	queue_work(test_wq, &work);

	return 0;
}

/*
 * It is weird that I didn't see any free function.
 * And we actually can load this module multiple times ok.
 */
static void test_workqueue_exit(void)
{
	pr_info("Module exit..\n");
}

module_init(test_workqueue_init);
module_exit(test_workqueue_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yizhou Shan");
