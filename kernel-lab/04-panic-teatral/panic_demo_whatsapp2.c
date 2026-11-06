// SPDX-License-Identifier: GPL-2.0
/*
 * panic_demo_whatsapp2 - Provoca una falla de kernel A PROPOSITO (uso educativo).
 *
 * Material clasico de sistemas operativos: ver COMO muere el kernel y qué
 * deja registrado. Solo afecta a TU VM descartable. No captura nada, no
 * persiste, no esconde nada: anuncia lo que va a hacer y lo hace.
 *
 * Elegis el modo de muerte con el parametro 'modo':
 *   panic -> llama panic(): detiene el kernel de inmediato (el mas limpio).
 *   null  -> desreferencia un puntero NULL: genera un "oops".
 *   bug   -> BUG(): marca un bug de kernel (oops/stop).
 *
 * Diferencia importante para la defensa:
 *   - panic() SIEMPRE frena la maquina.
 *   - un "oops" (null/bug) mata el contexto actual pero el kernel puede
 *     seguir vivo, SALVO que /proc/sys/kernel/panic_on_oops == 1.
 *   Ver el README para forzar reboot automatico y leer el crash.
 */

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

	/* Pequeño suspenso para el show. */
	msleep(1500);

	if (strcmp(modo, "null") == 0) {
		int *p = NULL;

		pr_alert("panic_demo_whatsapp2: desreferenciando NULL -> oops\n");
		/* Escritura a 0x0: provoca el oops. */
		*p = 0xDEAD;
	} else if (strcmp(modo, "bug") == 0) {
		pr_alert("panic_demo_whatsapp2: invocando BUG()\n");
		BUG();
	} else {
		pr_alert("panic_demo_whatsapp2: invocando panic()\n");
		panic("lab SO: panico intencional desde panic_demo_whatsapp2 (modo=%s)", modo);
	}

	/* Con modo=panic no se llega aca. Con oops, puede que sí. */
	return 0;
}

static void __exit pd_exit(void)
{
	pr_alert("panic_demo_whatsapp2: descargado.\n");
}

module_init(pd_init);
module_exit(pd_exit);
