def readlines_mock(path="palabras.txt"):
    with open(path, "r") as f:
        return [line.strip() for line in f.readlines()]

def cuenta(palabra, lineas, alto, bajo):
    if bajo > alto:
        return 0  # fuera de rango

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
    encontrados = []

    for i in range(1, len(lineas) + 1):
    
        for palabra in lineas:
            print(f"Buscando '{palabra}' en la línea {i}...")
            try:
                resultado = cuenta(palabra, lineas, i - 1, 0)
                if 401 <= resultado <= 799:
                    encontrados.append((resultado, palabra, i))
            except:
                continue

    if encontrados:
        print("🎯 Entradas válidas encontradas:")
        for r, p, i in encontrados:
            print(f"  ➤ Número: {r}, Palabra: {p}, Índice: {i}")
        print("\n👉 Usá cualquiera como input: '<número> <palabra>'")
    else:
        print("❌ No se encontró ninguna entrada válida.")

if __name__ == "__main__":
    buscar_inputs_validos()
