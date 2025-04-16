def readlines_mock(path="./TP2-x86_64/bomb87/palabras.txt"):
    with open(path, "r") as f:
        return [line.strip() for line in f.readlines()]
def cuenta(palabra, lineas, alto, bajo):
    if bajo > alto:
        return 0  # nunca debería llegar acá

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

def buscar_input_valido():
    lineas = readlines_mock()
    for i in range(1, len(lineas) + 1):
        for palabra in lineas:
            try:
                resultado = cuenta(palabra, lineas, i - 1, 0)
                if 401 <= resultado <= 799:
                    print("\n🎉 ¡Input válido encontrado!")
                    print(f"🧮 Número : {resultado}")
                    print(f"📝 Palabra: {palabra}")
                    print(f"🔢 Índice : {i}")
                    return
            except:
                continue
    print("❌ No se encontró ningún input válido.")

if __name__ == "__main__":
    buscar_input_valido()
