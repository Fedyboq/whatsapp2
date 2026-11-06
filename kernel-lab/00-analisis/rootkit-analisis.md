# Análisis de técnicas de rootkit en kernel: cómo funcionan, cómo se detectan, cómo se mitigan

Documento teórico que acompaña al lab. Cubre el "por qué" sin escribir payload
ofensivo: es el material que respalda la defensa oral y la parte conceptual de
la entrega.

> Enfoque: entender el mecanismo para poder **detectarlo y defenderse**. Todo
> lo demostrable en código está en los módulos del lab, de forma transparente.

---

## 1. Qué es un rootkit de kernel y por qué el kernel

Un rootkit es software cuyo objetivo principal es **mantener acceso y
permanecer oculto**. En kernel (anillo 0) es especialmente poderoso porque el
kernel es la fuente de verdad del sistema: decide qué procesos existen, qué
archivos hay, qué devuelve cada syscall. Si comprometés esa capa, podés mentirle
a todas las herramientas de usuario (`ps`, `ls`, `lsmod`, `netstat`) porque
todas preguntan, en última instancia, al kernel.

Tres familias de técnicas, que son las que "enganchan" y que el lab cubre en su
versión legítima:

1. **Ocultar objetos** (procesos, archivos, el propio módulo).
2. **Interceptar eventos** (syscalls, teclado, red).
3. **Comunicación usuario↔kernel** (canales de control).

---

## 2. Ocultar el módulo de `lsmod`

### Mecanismo
`lsmod` lee `/proc/modules`, que recorre una lista doblemente enlazada de
`struct module`. Sacarse de esa lista (`list_del(&THIS_MODULE->list)`) hace que
el módulo desaparezca de `lsmod`. **Demostrado en** `02-visibilidad-modulos/`.

### Por qué es débil / cómo se detecta
El estado del kernel está representado en **varias vistas a la vez**:
- `/proc/modules` (la lista).
- `/sys/module/<nombre>` (kobjects / sysfs).
- estructuras internas de asignación de memoria del módulo.

Un `list_del` toca solo la primera. La detección vive de **cruzar vistas**:
- Comparar `/sys/module` contra `/proc/modules` → `detector.sh` del lab.
- Recorrer la lista de kobjects del kernel.
- Escanear regiones de memoria de módulos (`module_alloc`) buscando módulos
  cargados que no figuren en la lista.
- `ftrace`/notificadores de carga de módulos registrados **antes** del ataque.

### Mitigación
- **Secure Boot + firma de módulos** (`CONFIG_MODULE_SIG_FORCE`): el kernel
  rechaza módulos no firmados; sube mucho la barrera de entrada.
- **Lockdown mode** del kernel.
- **`kable`/IMA** y medición de integridad.
- Baselining: lista firmada de módulos esperados y alerta ante cualquier desvío.

---

## 3. Ocultar procesos y archivos

### Mecanismo (conceptual)
`ps` y `ls` leen de `/proc` y de `getdents64`. Un rootkit intercepta esas
lecturas y **filtra** las entradas que quiere esconder (por PID, por prefijo de
nombre). Históricamente se hacía parcheando la tabla de syscalls; hoy se hace
con hooks de funciones del VFS o con `ftrace`.

### Detección
- **Cross-view**: comparar lo que reporta la API de alto nivel contra fuentes
  más crudas. Ej.: recorrer PIDs "a fuerza bruta" (`kill -0` a cada PID, o leer
  `/proc/<pid>` directo) y comparar contra lo que lista `/proc`.
- Herramientas tipo `rkhunter`, `chkrootkit`, `unhide`.
- Verificar integridad de la tabla de syscalls / de los punteros hookeados.

### Mitigación
- Igual que arriba: firma de módulos, lockdown, IMA, y monitoreo con eBPF
  desde una base de confianza.

---

## 4. Interceptar eventos

### Teclado
Mecanismo legítimo: `register_keyboard_notifier`. **Demostrado como contador**
en `01-monitor-teclas/`. Un keylogger real guardaría el keycode; el lab a
propósito **solo cuenta**. La defensa consiste en auditar qué notificadores y
qué hooks de input están registrados.

### Syscalls
Mecanismo legítimo y muy usado para auditoría/seguridad: `kprobes`,
`tracepoints`, `ftrace`, `eBPF`. **Demostrado** en `03-honeypot-syscalls/` como
tripwire defensivo sobre la apertura de archivos.

Lo interesante para la defensa: **la misma primitiva** (`kprobe`) sirve para
auditar (defensa) o para interceptar y mentir (ataque). Lo que cambia es la
intención y la transparencia. Por eso en defensa se monitorea *quién* registra
kprobes/hooks y se usan mecanismos de integridad.

---

## 5. Comunicación usuario↔kernel

Canales legítimos: entradas en `/proc`, `/sys`, `netlink`, `ioctl` sobre un
char device. Un rootkit los usa para recibir comandos ("ocultá este PID") de
forma encubierta. Defensa: inventariar devices e interfaces `/proc` y `/sys`
inesperadas y correlacionar con módulos no confiables.

---

## 6. Detección en la práctica — checklist

- [ ] `rkhunter` / `chkrootkit` / `unhide` (detectores conocidos).
- [ ] Cruce `/sys/module` ↔ `/proc/modules` (ver `detector.sh`).
- [ ] Verificar integridad de la tabla de syscalls y hooks de VFS/input.
- [ ] Enumerar kprobes/ftrace activos (`/sys/kernel/debug/kprobes/list`,
      `/sys/kernel/tracing`).
- [ ] Barrido de PIDs a fuerza bruta vs `/proc`.
- [ ] eBPF desde una base confiable para observar sin depender del estado
      posiblemente comprometido.

## 7. Mitigación — resumen

| Control | Qué frena |
|---|---|
| Firma obligatoria de módulos + Secure Boot | Carga de LKM no confiables |
| Kernel lockdown | Accesos que permitirían parchear el kernel en caliente |
| IMA / medición de integridad | Cambios no autorizados a binarios/kernel |
| Baselining + monitoreo (eBPF/auditd) | Desvíos respecto del estado esperado |
| Mínimo privilegio / no root rutinario | Capacidad de cargar módulos |

---

## 8. Marco ético y de alcance

- Todo el código del lab es **transparente e inofensivo**: anuncia lo que hace
  y no incluye captura de datos, persistencia ni evasión robusta.
- El objetivo declarado es **detección y defensa**; las técnicas de ataque se
  describen solo en la medida necesaria para entenderlas y cazarlas.
- Ejecutar **solo en una VM propia y descartable**, nunca sobre equipos de
  terceros o de la institución sin autorización explícita.

### Fuentes para ampliar (para la bibliografía de la entrega)
- Documentación del kernel: `Documentation/trace/kprobes.rst`,
  `Documentation/admin-guide/module-signing.rst`.
- Proyectos de detección: `rkhunter`, `chkrootkit`, `unhide`.
- LKM programming guides (TLDP) para la mecánica de módulos.
