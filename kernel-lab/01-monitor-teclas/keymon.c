// SPDX-License-Identifier: GPL-2.0
/*
 * keymon - Monitor de pulsaciones de teclado (CONTADOR, no keylogger)
 *
 * Objetivo didactico: mostrar como el kernel permite interceptar eventos de
 * teclado usando la API de notificadores (keyboard_notifier). Es la MISMA
 * tecnica de interceptacion que usaria un keylogger, con una diferencia
 * deliberada y central:
 *
 *      -> SOLO CONTAMOS eventos. NO guardamos que tecla se apreto.
 *
 * Por eso esto NO sirve para robar informacion: no reconstruye lo tipeado.
 * Ademas anuncia por dmesg que esta activo. La transparencia es parte del
 * ejercicio (un rootkit haria exactamente lo contrario: esconderse y guardar
 * los keycodes). Ese contraste es lo que se explica en la defensa.
 *
 * Probado en kernels 5.15 / 6.x.
 */

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

/* Cada cuantas pulsaciones imprimimos un resumen. Parametro de modulo. */
static unsigned int cada = 50;
module_param(cada, uint, 0644);
MODULE_PARM_DESC(cada, "Imprimir un resumen cada N pulsaciones (default 50)");

static atomic_t total = ATOMIC_INIT(0);

/*
 * Callback del notificador. Se invoca por cada evento de teclado.
 * param->down == 1  -> tecla presionada (key down)
 * param->value      -> keycode (NO lo usamos ni lo guardamos a proposito)
 *
 * Contamos solo los "key down" en etapa KBD_KEYCODE para no contar dos veces.
 */
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
