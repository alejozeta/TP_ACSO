def readlines(path="palabras.txt"):
    with open(path, "r") as f:
        return [line.strip() for line in f.readlines()]

def explode_bomb():
    raise Exception("BOOM 💣")

def cuenta(palabra, lineas, bajo, alto):
    if bajo > alto:
        explode_bomb()

    mid = (bajo + alto) // 2
    linea = lineas[mid]
    ascii_val = ord(linea[0])

    if palabra == linea:
        return ascii_val
    elif palabra > linea:
        if mid >= alto:
            explode_bomb()
        return ascii_val + cuenta(palabra, lineas, mid + 1, alto)
    else:
        if mid <= bajo:
            explode_bomb()
        return ascii_val + cuenta(palabra, lineas, bajo, mid - 1)

def buscar_inputs_validos():
    lineas = readlines()
    lineas.sort()
    for palabra in lineas:
        try:
            valor = cuenta(palabra, lineas, 0, len(lineas) - 1)
            if 401 <= valor <= 799:
                print(f"Palabra: '{palabra}', Input correcto 2: {valor}")
        except Exception:
            continue

buscar_inputs_validos()