// SPDX-License-Identifier: GPL-2.0
/*
 * honeypot - Tripwire de syscalls con kprobe sobre la apertura de archivos.
 *
 * Lado DEFENSA puro: es la base de un detector de intrusos (HIDS). Coloca un
 * kprobe en la ruta de apertura de archivos del kernel y AVISA por dmesg
 * cuando cualquier proceso intenta abrir un "archivo trampa". Un archivo que
 * nadie deberia tocar: si se toca, es señal de actividad sospechosa.
 *
 * No bloquea nada, no oculta nada: solo observa y registra (quien, que PID).
 *
 * TECNICA: kprobe sobre do_sys_openat2(dfd, filename, how). En x86-64 el
 * segundo argumento (filename, puntero a espacio de usuario) viaja en
 * regs->si. Lo copiamos con strncpy_from_user (maneja fallos de pagina).
 *
 * Compatibilidad: 'do_sys_openat2' existe desde ~5.6. Si tu kernel no lo
 * tiene, probá con "do_sys_open". Verificá el simbolo con:
 *     grep -w do_sys_openat2 /proc/kallsyms
 */

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

/* Ruta trampa. Crea el archivo con: touch /tmp/trampa (o lo que pongas aca). */
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
	/* 2do argumento de do_sys_openat2 -> RSI */
	uname = (const char __user *)regs->si;
#else
	/* En otras arquitecturas ajustar el registro del 2do argumento. */
	return 0;
#endif

	n = strncpy_from_user(buf, uname, MAXBUF - 1);
	if (n < 0)
		return 0;            /* no se pudo leer (p.ej. EFAULT): ignorar */
	buf[n] = '\0';

	if (strcmp(buf, trampa) == 0) {
		pr_alert("honeypot: ⚠️ ALERTA: '%s' (comm=%s pid=%d) toco la trampa '%s'\n",
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
