import subprocess

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

def buscar_input_valido():
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

def ejecutar_fase3_con_input(input_str, archivo="input_fase3.txt", binario="./bomb"):
    with open(archivo, "w") as f:
        f.write(input_str + "\n")
    
    print(f"Probando con input: {input_str}")
    result = subprocess.run([binario], stdin=open(archivo), capture_output=True, text=True)

    if "BOOM" in result.stdout or "explode_bomb" in result.stderr:
        print("💥 La bomba explotó.")
    else:
        print("✅ ¡Fase 3 superada!")
        print(result.stdout)

if __name__ == "__main__":
    input_fase3 = buscar_input_valido()
    if input_fase3:
        ejecutar_fase3_con_input(input_fase3)
    else:
        print("❌ No se encontró ningún input válido.")
