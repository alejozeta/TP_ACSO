import subprocess

def probar_inputs(binario="./bomb", archivo="valids.txt"):
    with open(archivo, "r") as f:
        lineas = [line.strip() for line in f if line.strip()]

    for linea in lineas:
        print(f"🧪 Probando: {linea} ... ", end="")
        try:
            proc = subprocess.run(
                [binario],
                input=linea.encode(),
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=2
            )
            if proc.returncode == 0:
                print("✅ PASA")
            else:
                print(f"💥 EXPLOTÓ (exit={proc.returncode})")
        except subprocess.TimeoutExpired:
            print("⏱️ Timeout")
        except Exception as e:
            print(f"⚠️ Error: {e}")

if __name__ == "__main__":
    probar_inputs()
