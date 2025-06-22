# Dockerfile
FROM debian:bookworm-slim

# 1) Instala compilador, make y valgrind (incluye Helgrind)
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      build-essential \
      valgrind \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

# 2) Copia todo tu código dentro de la imagen
COPY . /workspace

# 3) Asegura permisos de ejecución para el script de pruebas
RUN chmod +x tester.sh

# 4) Compila tu target “custom” (el que usa test_custom.cc)
RUN make clean && make custom

# 5) Por defecto lanzamos un shell interactivo
CMD ["bash"]