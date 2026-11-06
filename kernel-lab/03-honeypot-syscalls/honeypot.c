

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lab SO - uso educativo");
MODULE_DESCRIPTION("Honeypot/tripwire de apertura de archivos via kprobe");
MODULE_VERSION("1.0");

static char *trampa = "/tmp/trampa";
module_param(trampa, charp, 0644);
MODULE_PARM_DESC(trampa, "Ruta del archivo trampa a vigilar");

#define MAXBUF 256

static int honeypot_pre(struct kprobe *p, struct pt_regs *regs)
{
	const char __user *uname;
	char buf[MAXBUF];
	long n;

#ifdef CONFIG_X86_64
	uname = (const char __user *)regs->si;
#else
	return 0;
#endif

	n = strncpy_from_user(buf, uname, MAXBUF - 1);
	if (n < 0)
		return 0;          
	buf[n] = '\0';

	if (strcmp(buf, trampa) == 0) {
		pr_alert("honeypot:  ALERTA: '%s' (comm=%s pid=%d) toco la trampa '%s'\n",
			 current->comm, current->comm, current->pid, trampa);
	}

	return 0;
}

static struct kprobe kp = {
	.symbol_name = "do_sys_openat2",
	.pre_handler = honeypot_pre,
};

static int __init honeypot_init(void)
{
	int ret;

	ret = register_kprobe(&kp);
	if (ret < 0) {
		pr_err("honeypot: register_kprobe fallo (%d). ¿Existe '%s'? Revisa /proc/kallsyms\n",
		       ret, kp.symbol_name);
		return ret;
	}

	pr_info("honeypot: ===== TRIPWIRE ACTIVO =====\n");
	pr_info("honeypot: vigilando aperturas de '%s'. Crealo con: touch %s\n",
		trampa, trampa);
	return 0;
}

static void __exit honeypot_exit(void)
{
	unregister_kprobe(&kp);
	pr_info("honeypot: descargado.\n");
}

module_init(honeypot_init);
module_exit(honeypot_exit);
