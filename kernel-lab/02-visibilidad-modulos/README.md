# 02 — Visibilidad de módulos (ataque + defensa)

Par completo: un módulo que se oculta de `lsmod` y un script que lo caza.

## Correr la demo (en la VM descartable)

```bash
make
sudo dmesg -C
sudo insmod ocultar_demo.ko
lsmod | grep ocultar_demo        # NO aparece (se escondió)
ls /sys/module/ | grep ocultar   # SÍ aparece (se olvidó de sysfs)
sudo dmesg                       # mira cómo anuncia todo
```

Ahora la defensa:

```bash
chmod +x detector.sh
sudo ./detector.sh               # [SOSPECHA] ocultar_demo ...
```

## Limpiar

Estando oculto, `rmmod ocultar_demo` por nombre **falla** (ya no está en la
lista de módulos). Es parte de la lección: ocultarse rompe tu propia salida.
**Reiniciá la VM** para dejar todo limpio.

## Para la defensa oral

- **La técnica de ataque** (`list_del`) es de una línea y es la base de casi
  todos los rootkits LKM "clásicos".
- **Por qué es débil:** toca una sola estructura (`/proc/modules`) y deja
  rastros en otra (`/sys/module`). La detección vive de esas inconsistencias
  entre vistas del mismo estado.
- **Escalada ataque/defensa:** si el rootkit *también* hace `kobject_del` para
  limpiar sysfs, el detector de arriba ya no sirve. Ahí la defensa sube un
  escalón: escanear la memoria del kernel / usar `ftrace` / comparar contra
  una lista de módulos confiable firmada. Eso está explicado en
  `../00-analisis/rootkit-analisis.md`.
