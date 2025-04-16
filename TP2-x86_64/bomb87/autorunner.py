import subprocess

# Fase 1 y 2 conocidas
FASE_1 = "Confia en el tiempo, que suele dar dulces salidas a muchas amargas dificultades"
FASE_2 = "-1000 7062"

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

def buscar_input_fase3():
    lineas = readlines_mock()
    alto = len(lineas) - 1

    for palabra in lineas:
        try:
            res = cuenta(palabra, lineas, alto, 0)
            if 401 <= res <= 799:
                return f"{res} {palabra}"
        except:
            continue
    return None

def ejecutar_bomb_con_input(input_total, archivo="input_bomb.txt", binario="./bomb"):
    with open(archivo, "w") as f:
        for linea in input_total:
            f.write(linea + "\n")

    print(f"Probando con input:\n{chr(10).join(input_total)}\n")
    result = subprocess.run([binario], stdin=open(archivo), capture_output=True, text=True)

    if "BOOM" in result.stdout or "explode_bomb" in result.stderr:
        print("💥 La bomba explotó.")
    else:
        print("✅ ¡Fase 3 superada!")
        print(result.stdout)

if __name__ == "__main__":
    fase3 = buscar_input_fase3()
    if fase3:
        input_total = [FASE_1, FASE_2, fase3]
        ejecutar_bomb_con_input(input_total)
    else:
        print("❌ No se encontró un input válido para fase 3.")
