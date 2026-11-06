

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/keyboard.h>
#include <linux/notifier.h>
#include <linux/atomic.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lab SO - uso educativo");
MODULE_DESCRIPTION("Contador transparente de pulsaciones via keyboard_notifier");
MODULE_VERSION("1.0");

static unsigned int cada = 50;
module_param(cada, uint, 0644);
MODULE_PARM_DESC(cada, "Imprimir un resumen cada N pulsaciones (default 50)");

static atomic_t total = ATOMIC_INIT(0);

static int keymon_cb(struct notifier_block *nb, unsigned long action, void *data)
{
	struct keyboard_notifier_param *param = data;

	if (action == KBD_KEYCODE && param->down) {
		int n = atomic_inc_return(&total);

		/* Nota: NO registramos param->value (la tecla concreta). */
		if (cada && (n % cada == 0))
			pr_info("keymon: %d pulsaciones contadas hasta ahora\n", n);
	}

	return NOTIFY_OK;
}

static struct notifier_block keymon_nb = {
	.notifier_call = keymon_cb,
};

static int __init keymon_init(void)
{
	pr_info("keymon: ===== MONITOR DE TECLAS ACTIVO (modo transparente) =====\n");
	pr_info("keymon: cuenta pulsaciones; NO guarda que tecla se apreto.\n");
	register_keyboard_notifier(&keymon_nb);
	return 0;
}

static void __exit keymon_exit(void)
{
	unregister_keyboard_notifier(&keymon_nb);
	pr_info("keymon: descargado. Total de pulsaciones contadas: %d\n",
		atomic_read(&total));
}

module_init(keymon_init);
module_exit(keymon_exit);
