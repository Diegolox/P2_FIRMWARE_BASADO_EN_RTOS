"""Calcula periodos y separacion entre dos mensajes del monitor de PlatformIO."""

import re
from statistics import mean

PERIODO_ESPERADO_MS = 1000  # Cambiar si las tareas deben ejecutarse cada otro tiempo
PATRON = re.compile(r"^(\d{1,2}):(\d{2}):(\d{2})\.(\d{3})\s*>\s*(.*)$")


def instante_ms(linea):
    match = PATRON.match(linea.strip())
    if not match:
        return None
    hora, minuto, segundo, milisegundo = map(int, match.groups()[:4])
    mensaje = match.group(5)
    tiempo = ((hora * 60 + minuto) * 60 + segundo) * 1000 + milisegundo
    if mensaje == "Hola mundo":
        return "HOLA", tiempo
    if len(mensaje.split(";")) == 6:
        return "IMU", tiempo
    return None


def mostrar_periodo(nombre, tiempos):
    periodos = [b - a for a, b in zip(tiempos, tiempos[1:])]
    validos = [p for p in periodos if 0 < p < 2 * PERIODO_ESPERADO_MS]
    if not validos:
        print(f"{nombre}: no hay suficientes intervalos validos")
        return
    errores = [p - PERIODO_ESPERADO_MS for p in validos]
    print(f"{nombre}: periodo medio {mean(validos):.1f} ms "
          f"| error medio {mean(errores):+.1f} ms "
          f"| error maximo {max(map(abs, errores))} ms")
    if len(validos) != len(periodos):
        print(f"  Intervalos descartados: {len(periodos) - len(validos)}")


print("Pega el registro de PlatformIO y termina con una linea vacia:\n")
tiempos = {"IMU": [], "HOLA": []}
eventos = []
while True:
    try:
        linea = input()
    except EOFError:
        break
    if not linea.strip():
        break
    evento = instante_ms(linea)
    if evento:
        nombre, tiempo = evento
        tiempos[nombre].append(tiempo)
        eventos.append(evento)

for nombre in ("IMU", "HOLA"):
    mostrar_periodo(nombre, tiempos[nombre])

# Mide la distancia desde cada impresion IMU hasta el siguiente "Hola mundo".
separaciones = []
for (nombre_a, t_a), (nombre_b, t_b) in zip(eventos, eventos[1:]):
    if nombre_a == "IMU" and nombre_b == "HOLA" and 0 <= t_b - t_a < PERIODO_ESPERADO_MS:
        separaciones.append(t_b - t_a)

if separaciones:
    print(f"IMU -> HOLA: separacion media {mean(separaciones):.1f} ms "
          f"| minima {min(separaciones)} ms | maxima {max(separaciones)} ms")
else:
    print("IMU -> HOLA: no hay parejas validas")
