def readlines_mock(path="./TP2-x86_64/bomb87/palabras.txt"):
    with open(path, "r") as f:
        return [line.strip() for line in f.readlines()]

def cuenta(s, lines, low, high):
    if low > high:
        return 0
    mid = (low + high) // 2
    word = lines[mid]

    if word == s:
        return ord(word[0])
    elif word > s:
        if mid <= low:
            raise Exception("explode_bomb")
        return ord(word[0]) + cuenta(s, lines, low, mid - 1)
    else:
        if mid >= high:
            raise Exception("explode_bomb")
        return ord(word[0]) + cuenta(s, lines, mid + 1, high)

def probar_inputs():
    lines = readlines_mock()
    n_palabras = len(lines)

    for idx in range(1, n_palabras + 1):
        for palabra in lines:
            try:
                resultado = cuenta(palabra, lines, 0, idx - 1)
                ajustado = resultado - 401
                if 0 <= ajustado <= 398:
                    print("Entrada válida encontrada:")
                    print(f"  Número: {resultado}")
                    print(f"  Palabra: {palabra}")
                    print(f"  Índice: {idx}")
                    return
            except:
                continue

    print("No se encontró entrada válida.")

if __name__ == "__main__":
    probar_inputs()
