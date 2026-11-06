#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
calavera.py - Intro teatral (y honesta) para el demo del kernel panic.

Secuencia:
  1) Una calavera saluda abriendo y cerrando la boca.
  2) Las calaveras se multiplican hasta llenar la pantalla.
  3) Glitch rojo + cuenta regresiva: "KERNEL PANIC EN T-3... 2... 1...".
  4) Cartel final con el comando real para tumbar la VM.

Todo lo que dice es verdad: anuncia que vas a provocar un panic() de verdad.
No simula ningun hackeo. Es puro terminal: NO toca el sistema.

Uso:
    python3 calavera.py                 # show y cartel con el comando (seguro)
    python3 calavera.py --loop          # repite hasta Ctrl+C
    python3 calavera.py --disparar      # al final carga el modulo y TUMBA la VM
    python3 calavera.py --silencio      # sin beeps del terminal
"""

import os
import sys
import time
import random
import shutil
import subprocess

# Flags (se setean en main()).
DISPARAR = False
SILENCIO = False

# ---- Color (se desactiva solo si la salida no es una terminal) ----
_TTY = sys.stdout.isatty()
ROJO   = "\033[91m" if _TTY else ""
BLANCO = "\033[97m" if _TTY else ""
TENUE  = "\033[90m" if _TTY else ""
RESET  = "\033[0m"  if _TTY else ""

# ---- Calavera grande (la mandibula, ultima parte, se anima) ----
CABEZA = r"""
      .-~~~~~~~-.
    .'           '.
   /   O     O     \
  :                 :
  |       /\        |
  :      (  )       :
   \     '--'      /
    '.           .'
"""
MAND_CERRADA = r"     '-|=|=|=|=|-'"
MAND_ABIERTA = "     .-|     |-.\n      | | | | |\n     '-'-'-'-'-'"

# ---- Mini calavera para el enjambre ----
MINI = [" .-. ", "(o o)", " ')' "]

GLITCH = "#@%&$/\\|=+<>░▒▓"


def limpiar():
    sys.stdout.write("\033[2J\033[H")
    sys.stdout.flush()


def beep(n=1):
    """Suena el bell del terminal (si hay terminal y no esta en silencio)."""
    if _TTY and not SILENCIO:
        sys.stdout.write("\a" * n)
        sys.stdout.flush()


def dims():
    try:
        s = shutil.get_terminal_size()
        return max(s.columns, 20), max(s.lines, 10)
    except Exception:
        return 80, 24


def centrar_h(linea, w):
    return linea.center(w)


def pantalla_centro(lineas):
    """Centra un bloque de lineas vertical y horizontalmente."""
    w, h = dims()
    limpiar()
    arriba = max((h - len(lineas)) // 2, 0)
    sys.stdout.write("\n" * arriba)
    for ln in lineas:
        sys.stdout.write(centrar_h(ln, w) + "\n")
    sys.stdout.flush()


def dibujar_calavera(mandibula, mensaje=""):
    w, _ = dims()
    limpiar()
    cuerpo = CABEZA.rstrip("\n") + "\n" + mandibula
    bloque = [centrar_h(l, w) for l in cuerpo.split("\n")]
    print(ROJO + "\n".join(bloque) + RESET)
    if mensaje:
        print()
        print(BLANCO + centrar_h(mensaje, w) + RESET)
    sys.stdout.flush()


def hablar(texto, veces=2):
    """Abre y cierra la boca mientras 'dice' el texto."""
    for _ in range(veces):
        dibujar_calavera(MAND_ABIERTA, texto)
        beep()
        time.sleep(0.17)
        dibujar_calavera(MAND_CERRADA, texto)
        time.sleep(0.17)


def enjambre():
    """Las calaveras se multiplican progresivamente hasta llenar la pantalla."""
    w, h = dims()
    cw, ch = 7, 4
    ncol = max(w // cw, 1)
    nrow = max((h - 1) // ch, 1)
    celdas = [(r, c) for r in range(nrow) for c in range(ncol)]
    random.shuffle(celdas)
    total = len(celdas)

    pasos = 10
    for k in range(1, pasos + 1):
        n = int(total * k / pasos)
        activos = celdas[:n]
        canvas = [[" "] * w for _ in range(nrow * ch)]
        for (r, c) in activos:
            y, x = r * ch, c * cw
            for i, ln in enumerate(MINI):
                for j, ch_ in enumerate(ln):
                    if y + i < len(canvas) and x + j < w:
                        canvas[y + i][x + j] = ch_
        limpiar()
        sys.stdout.write(ROJO + "\n".join("".join(row) for row in canvas) + RESET)
        sys.stdout.flush()
        time.sleep(0.12)


def glitchear(texto, pasos=7):
    w, h = dims()
    beep()
    for _ in range(pasos):
        s = "".join(c if (c == " " or random.random() > 0.35)
                    else random.choice(GLITCH) for c in texto)
        pantalla_centro(["", ROJO + s + RESET, ""])
        time.sleep(0.08)
    pantalla_centro(["", ROJO + texto + RESET, ""])
    time.sleep(0.3)


def cuenta_regresiva():
    for t in (3, 2, 1):
        pantalla_centro([
            ROJO + "***  KERNEL PANIC EN  ***" + RESET,
            "",
            ROJO + ("T  -  %d" % t) + RESET,
            "",
            TENUE + "(panic() real, no es simulacro)" + RESET,
        ])
        beep(2)
        time.sleep(0.8)


def final():
    pantalla_centro([
        ROJO + "MODULO LISTO PARA CARGAR" + RESET,
        "",
        BLANCO + "Ejecuta en otra terminal:" + RESET,
        "",
        BLANCO + "    sudo insmod panic_demo_whatsapp2.ko modo=panic" + RESET,
        "",
        TENUE + "...y mira caer la VM en vivo." + RESET,
    ])
    time.sleep(1.5)


def disparar_panic():
    """Carga el modulo real (solo con --disparar). Esto tumba la VM."""
    aqui = os.path.dirname(os.path.abspath(__file__))
    ko = os.path.join(aqui, "panic_demo_whatsapp2.ko")
    if not os.path.exists(ko):
        pantalla_centro([
            ROJO + "No encontre panic_demo_whatsapp2.ko" + RESET,
            "",
            BLANCO + "Compilalo primero con:  make" + RESET,
        ])
        return
    pantalla_centro([ROJO + ">>> DISPARANDO panic() <<<" + RESET])
    beep(3)
    time.sleep(0.5)
    try:
        # No retorna: el kernel entra en panico y detiene la VM.
        subprocess.call(["sudo", "insmod", ko, "modo=panic"])
    except Exception as e:
        print("No se pudo cargar el modulo:", e)


def show():
    dibujar_calavera(MAND_CERRADA, "...")
    time.sleep(0.6)
    hablar("¡Hola! Soy la calavera del kernel")
    hablar("Vengo a presentar una demo de SO:")
    hablar("un KERNEL PANIC provocado a proposito")
    enjambre()
    glitchear(">>> F A L L A   D E   K E R N E L   I N M I N E N T E <<<")
    cuenta_regresiva()
    if DISPARAR:
        disparar_panic()
    else:
        final()


def main():
    global DISPARAR, SILENCIO
    args = sys.argv[1:]
    loop = "--loop" in args
    DISPARAR = "--disparar" in args
    SILENCIO = "--silencio" in args
    try:
        sys.stdout.write("\033[?25l")  # ocultar cursor
        if loop:
            while True:
                show()
        else:
            show()
    except KeyboardInterrupt:
        pass
    finally:
        sys.stdout.write("\033[?25h")  # mostrar cursor
        sys.stdout.write(RESET)
        print()


if __name__ == "__main__":
    main()
