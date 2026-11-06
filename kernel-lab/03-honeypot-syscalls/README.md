# 03 — Honeypot / tripwire de syscalls (kprobe)

Hookea la apertura de archivos y avisa cuando alguien toca un archivo trampa.
Es el esqueleto de un HIDS (detector de intrusos basado en host).

## Correr (en la VM descartable)

```bash
# Verificá primero que el símbolo exista en tu kernel:
grep -w do_sys_openat2 /proc/kallsyms   # debería imprimir una línea

make
touch /tmp/trampa                        # creá el archivo trampa
sudo dmesg -C
sudo insmod honeypot.ko                  # o: sudo insmod honeypot.ko trampa=/tmp/otro

# Dispará la trampa desde otra terminal:
cat /tmp/trampa

sudo dmesg                               # verás la ALERTA con comm/pid
sudo rmmod honeypot
make clean
```

## Notas

- `do_sys_openat2` existe en kernels ~5.6+. Si `register_kprobe` falla,
  cambiá `.symbol_name` por el que tengas (`do_sys_open`, etc.).
- Solo **observa y registra** — no bloquea ni esconde nada.
- Para la defensa: explicá por qué un kprobe en el kernel ve *todas* las
  aperturas sin importar el proceso, y cómo esto se usa para auditoría y
  detección (y por qué un atacante querría, al revés, desactivar estos hooks).
