from pathlib import Path

def leer_lineas(path="palabras.txt"):
    """Emula readlines del binario."""
    return [l.rstrip("\n") for l in Path(path).read_text(encoding="latin1").splitlines()]

def cuenta(palabra, lineas, low, high):
    """Traducción directa de la función `cuenta` del ensamblador."""
    if low > high:
        raise ValueError("explode_bomb: fuera de rango")

    mid = (low + high) // 2
    linea_mid = lineas[mid]
    c = ord(linea_mid[0])

    if palabra == linea_mid:
        return c
    elif palabra > linea_mid:
        if mid >= high:
            raise ValueError("explode_bomb: derecha")
        return c + cuenta(palabra, lineas, mid + 1, high)
    else:  # palabra < linea_mid
        if mid <= low:
            raise ValueError("explode_bomb: izquierda")
        return c + cuenta(palabra, lineas, low, mid - 1)

lineas = leer_lineas()           # Vector ordenado
validos = []

for w in lineas:
    try:
        s = cuenta(w, lineas, 0, len(lineas) - 1)
        if 401 <= s <= 799:
            validos.append((w, s))
    except ValueError:
        pass                      # Ruta inválida → descartar

print(f"Encontré {len(validos)} candidatos. Ejemplos:")
for w, s in validos[:10]:
    print(f"{w} {s}")
