def readlines_mock(path="palabras.txt"):
    with open(path, "r") as f:
        return [line.strip() for line in f.readlines()]

def cuenta(palabra, lineas, alto, bajo):
    if bajo > alto:
        raise Exception("explode_bomb: fuera de rango")
    
    mid = (alto + bajo) // 2
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
    alto = len(lineas) - 1
    resultados = []

    for palabra in lineas:
        try:
            valor = cuenta(palabra, lineas, alto, 0)
            resultados.append((valor, palabra))
        except:
            continue  # ignora casos que causarían una bomba

    return resultados

# Mostrar posibles pares válidos: (numero, palabra)
if __name__ == "__main__":
    inputs_validos = buscar_inputs_validos()
    for num, palabra in inputs_validos:
        print(f"{num} {palabra}")
