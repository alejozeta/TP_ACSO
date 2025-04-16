def readlines_mock(path="palabras.txt"):
    with open(path, "r") as f:
        return [line.strip() for line in f.readlines() if line.strip()]


def cuenta(palabra, lineas, alto, bajo):
    if bajo > alto:
        raise Exception("explode_bomb: fuera de rango")

    mid = (bajo + alto) // 2
    linea = lineas[mid]
    c = ord(linea[0])

    if palabra == linea:
        return c
    elif palabra > linea:
        if mid >= alto:
            raise Exception("explode_bomb: derecha")
        return c + cuenta(palabra, lineas, alto, mid + 1)
    else:
        if mid <= bajo:
            raise Exception("explode_bomb: izquierda")
        return c + cuenta(palabra, lineas, mid - 1, bajo)


def buscar_inputs_validos():
    lineas = readlines_mock()
    encontrados = []

    for palabra in lineas:
        try:
            valor = cuenta(palabra, lineas, len(lineas) - 1, 0)
            encontrados.append((palabra, valor))
        except Exception as e:
            print(f"✖️ {palabra} falló: {e}")

    return encontrados


# Mostrar y guardar resultados
if __name__ == "__main__":
    resultados = buscar_inputs_validos()

    with open("valids.txt", "w") as f:
        for palabra, valor in resultados:
            linea = f"{palabra} {valor}\n"
            f.write(linea)
            print(f"✔️ {linea.strip()}")
