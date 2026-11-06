#!/usr/bin/env bash
#
# detector.sh - Caza modulos ocultos de lsmod/proc comparando con sysfs.
#
# Lado DEFENSA del par. Idea: un modulo cargado dinamicamente aparece en
# /proc/modules (lo que lee `lsmod`) Y en /sys/module/<nombre> (con un archivo
# 'initstate'). Un rootkit que solo hace list_del() desaparece de
# /proc/modules pero se OLVIDA de sysfs. Buscamos esa inconsistencia.
#
# Uso:
#   chmod +x detector.sh
#   sudo ./detector.sh
#
set -euo pipefail

echo "== Detector de modulos ocultos (sysfs vs /proc/modules) =="
encontrados=0

for dir in /sys/module/*; do
	[ -d "$dir" ] || continue
	nombre="$(basename "$dir")"

	# 'initstate' existe para modulos cargados dinamicamente (LKM), no para
	# los compilados dentro del kernel. Asi filtramos ruido de built-ins.
	[ -e "$dir/initstate" ] || continue

	# ¿Esta tambien en /proc/modules? Si no, es sospechoso.
	if ! grep -q "^${nombre} " /proc/modules; then
		echo "[SOSPECHA] '${nombre}' esta en /sys/module pero NO en lsmod/proc"
		encontrados=$((encontrados + 1))
	fi
done

if [ "$encontrados" -eq 0 ]; then
	echo "OK: no se detectaron modulos ocultos con esta tecnica."
else
	echo ">> ${encontrados} modulo(s) sospechoso(s). Revisar manualmente."
fi
