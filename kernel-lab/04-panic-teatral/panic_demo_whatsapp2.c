// SPDX-License-Identifier: GPL-2.0


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lab SO - uso educativo");
MODULE_DESCRIPTION("Provoca panic/oops intencional del kernel (demo)");
MODULE_VERSION("1.0");

static char *modo = "panic";
module_param(modo, charp, 0644);
MODULE_PARM_DESC(modo, "panic | null | bug");

static int __init pd_init(void)
{
	pr_alert("panic_demo_whatsapp2: ============================================\n");
	pr_alert("panic_demo_whatsapp2:   FALLA INTENCIONAL DE KERNEL (modo=%s)\n", modo);
	pr_alert("panic_demo_whatsapp2:   Esto va a tumbar ESTA VM a proposito.\n");
	pr_alert("panic_demo_whatsapp2: ============================================\n");

	msleep(1500);

	if (strcmp(modo, "null") == 0) {
		int *p = NULL;

		pr_alert("panic_demo_whatsapp2: desreferenciando NULL -> oops\n");
		*p = 0xDEAD;
	} else if (strcmp(modo, "bug") == 0) {
		pr_alert("panic_demo_whatsapp2: invocando BUG()\n");
		BUG();
	} else {
		pr_alert("panic_demo_whatsapp2: invocando panic()\n");
		panic("lab SO: panico intencional desde panic_demo_whatsapp2 (modo=%s)", modo);
	}

	return 0;
}

static void __exit pd_exit(void)
{
	pr_alert("panic_demo_whatsapp2: descargado.\n");
}

module_init(pd_init);
module_exit(pd_exit);
