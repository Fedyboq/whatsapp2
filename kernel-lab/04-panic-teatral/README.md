# 04 — Kernel panic teatral 💀

Un módulo que **tumba el kernel a propósito** (material clásico de SO) + una
intro ASCII dramática pero **honesta**: una calavera saluda, después las
calaveras se multiplican hasta llenar la pantalla, glitch rojo y cuenta
regresiva `KERNEL PANIC EN T-3...2...1`, y un cartel con el comando real para
largar el panic. No simula ningún hackeo: anuncia lo que de verdad va a pasar.
Pensado para el demo/defensa.

> ⚠️ **Solo en tu VM descartable.** Esto detiene la máquina de verdad. Sacá un
> snapshot antes. La calavera (`calavera.py`) es inofensiva; el que tumba todo
> es `panic_demo_whatsapp2.ko`.

## El show (orden sugerido)

```bash
# 1) Intro teatral en la terminal (no toca el sistema):
#    calavera -> enjambre de calaveras -> glitch -> cuenta regresiva -> cartel
python3 calavera.py          # o: python3 calavera.py --loop  (Ctrl+C para cortar)

# 2) Compilá el módulo:
make

# 3) Limpiá el log y provocá el panic:
sudo dmesg -C
sudo insmod panic_demo_whatsapp2.ko             # modo por defecto: panic()
```

Con `modo=panic` la VM se detiene al instante. Fin del show.

### Todo-en-uno (la intro dispara el panic sola)

```bash
make                                   # compilá primero, para que exista el .ko
python3 calavera.py --disparar         # al terminar la cuenta regresiva hace
                                       # el insmod solo y tumba la VM (pide sudo)
```

Flags de `calavera.py`:

| Flag | Qué hace |
|---|---|
| (ninguno) | Show + cartel con el comando. **No** tumba nada (seguro para ensayar). |
| `--disparar` | Al final carga `panic_demo_whatsapp2.ko modo=panic` y **detiene la VM**. |
| `--loop` | Repite el show hasta Ctrl+C. |
| `--silencio` | Sin beeps del terminal. |

> El terminal suena (bell `\a`) en los saludos y en la cuenta regresiva. Si no
> escuchás nada, puede estar silenciado el bell del terminal/VM; probá subir el
> volumen o activar el "audible bell". Con `--silencio` lo apagás.

## Modos de falla (para explicar en la defensa)

```bash
sudo insmod panic_demo_whatsapp2.ko modo=panic   # panic(): frena el kernel YA
sudo insmod panic_demo_whatsapp2.ko modo=null    # desref NULL -> "oops"
sudo insmod panic_demo_whatsapp2.ko modo=bug     # BUG(): marca un bug de kernel
```

Diferencia clave:
- **`panic()`** siempre detiene la máquina.
- **`oops`** (null/bug) mata solo el proceso/contexto; el kernel **puede seguir
  vivo** salvo que lo configures para entrar en pánico ante un oops.

## Ver y controlar el crash

```bash
# Que un oops provoque panic (para que null/bug también tumben la VM):
sudo sysctl kernel.panic_on_oops=1

# Reboot automático N segundos después del panic (0 = quedarse colgado):
sudo sysctl kernel.panic=5

# Leer el último crash tras reiniciar (si está habilitado el log persistente):
journalctl -k -b -1        # kernel log del arranque anterior
```

El oops/panic imprime el registro de instrucción (RIP), el stack y los
registros: justo lo que sirve para explicar **cómo** y **dónde** murió el
kernel en la defensa.

## Limpiar

Con `modo=panic` ya reiniciaste (o reiniciá vos). Nada queda instalado de forma
persistente: es un módulo que cargás a mano.
