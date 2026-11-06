# Laboratorio de técnicas de kernel (ataque/defensa) — Linux

Lab educativo para una materia de sistemas operativos / seguridad. Cada módulo
demuestra una técnica **legítima y enseñable** de kernel, con el par
**ataque ↔ defensa** cuando corresponde. Nada acá está pensado para ocultar
actividad ni capturar datos de nadie: todo grita lo que hace por `dmesg`.

> ⚠️ **Dónde correrlo:** SOLO en una VM Ubuntu descartable (snapshot previo).
> Cargar módulos de kernel puede colgar la máquina. No lo uses en una PC real
> ni en equipos de la facultad que no sean tuyos.

## Qué incluye

| Carpeta | Técnica de kernel | Lado |
|---|---|---|
| `01-monitor-teclas/` | Intercepción de eventos de teclado con `keyboard_notifier` | demostración transparente |
| `02-visibilidad-modulos/` | Ocultar un módulo de `lsmod` **y** detectarlo | ataque + defensa |
| `03-honeypot-syscalls/` | `kprobe` sobre `openat` que avisa al tocar un archivo trampa | defensa (tripwire / IDS) |
| `00-analisis/` | Cómo funcionan los rootkits, cómo se detectan y cómo se mitigan | análisis + defensa |
| `04-panic-teatral/` | Kernel panic/oops intencional + intro ASCII (calavera que saluda y baila) | demo didáctico |

## Dos decisiones de diseño (para la defensa oral)

1. **El "keylogger" es un contador, no un logger.** El módulo 01 **cuenta**
   pulsaciones para enseñar la técnica de intercepción, pero **no registra qué
   teclas** se apretaron. La técnica es idéntica a la de un keylogger real; la
   diferencia es que el artefacto no sirve para robar nada. Si un rootkit real
   cambiaría una línea para guardar el keycode, ese es justamente el punto a
   explicar en la defensa.

2. **Ocultar módulos va siempre con su detector.** El módulo 02 incluye la
   técnica de ocultamiento (`list_del`) *y* el script que la caza. El valor
   educativo está en el par: mostrás el truco y mostrás cómo se rompe.

## Requisitos (en la VM)

```bash
sudo apt update
sudo apt install -y build-essential linux-headers-$(uname -r)
```

Kernel probado: series 5.15 / 6.x (Ubuntu 22.04 / 24.04). Algunos símbolos
(`do_sys_openat2`) dependen de la versión; cada módulo lo aclara.

## Cómo compilar y cargar (patrón general)

```bash
cd 01-monitor-teclas        # o la carpeta que toque
make
sudo dmesg -C               # limpia el buffer para leer claro
sudo insmod keymon.ko       # carga
lsmod | grep keymon         # verifica
sudo dmesg                  # mira qué imprimió
sudo rmmod keymon           # descarga
make clean
```

## Orden sugerido para la entrega

1. **03 honeypot** — el más claramente defensivo, buen arranque.
2. **01 monitor** — misma idea de interceptación, transparente.
3. **02 visibilidad** — el plato fuerte: ataque + defensa en vivo.
4. **00 análisis** — el documento que ata todo y te cubre la parte teórica.

Cada carpeta tiene su propio `README` o comentarios con los detalles.
