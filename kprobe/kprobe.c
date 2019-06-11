#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

/* For each probe you need to allocate a kprobe structure */
static struct kprobe kp = {
	.symbol_name	= "__handle_mm_fault",
};

/* kprobe pre_handler: called just before the probed instruction is executed */
static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	/*
	 * To fully track a function, you need to know the calling convention,
	 * i.e., knowing where are the arguments..
	 */
	pr_info("<%s> pre_handler: p->addr = 0x%p, ip = %#lx args(vma=%#lx addr=%#lx)\n",
		p->symbol_name, p->addr, regs->ip, regs->di, regs->si);

	/* A dump_stack() here will give a stack backtrace */
	//dump_stack();
	return 0;
}

/* kprobe post_handler: called after the probed instruction is executed */
static void handler_post(struct kprobe *p, struct pt_regs *regs,
				unsigned long flags)
{
	pr_info("<%s> post_handler: p->addr = 0x%p, flags = 0x%lx\n",
		p->symbol_name, p->addr, regs->flags);
}

/*
 * fault_handler: this is called if an exception is generated for any
 * instruction within the pre- or post-handler, or when Kprobes
 * single-steps the probed instruction.
 */
static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
	pr_info("fault_handler: p->addr = 0x%p, trap #%dn", p->addr, trapnr);
	/* Return 0 because we don't handle the fault. */
	return 0;
}

static int u_kprobe_init(void)
{
	int ret;
	kp.pre_handler = handler_pre;
	kp.post_handler = handler_post;
	kp.fault_handler = handler_fault;

	ret = register_kprobe(&kp);
	if (ret < 0) {
		pr_err("register_kprobe failed, returned %d\n", ret);
		return ret;
	}
	pr_info("Planted kprobe at %p\n", kp.addr);

	return 0;
}

static void u_kprobe_exit(void)
{
	unregister_kprobe(&kp);
	pr_info("kprobe at %p unregistered\n", kp.addr);
}

module_init(u_kprobe_init);
module_exit(u_kprobe_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yizhou Shan");
