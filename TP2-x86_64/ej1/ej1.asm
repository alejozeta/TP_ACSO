; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat
extern strlen
extern strcpy
extern printf

string_proc_list_create_asm:
    mov     edi, 16
    call    malloc

    test    rax, rax
    je      .return_null_list_create

    mov     qword [rax], 0
    mov     qword [rax + 8], 0
    ret

.return_null_list_create:
    xor     rax, rax
    ret



string_proc_node_create_asm:
    ; ─── Guardar los argumentos antes de que se pisen ────────────────
    mov     ecx, edi           ; ecx = type   (32 bit, mantendrá el 8‑bit bajo)
    mov     rdx, rsi           ; rdx = hash   (lo necesitaremos tras malloc)

    ; ─── Reservar memoria ────────────────────────────────────────────
    mov     edi, 32            ; size_t 32  → rdi
    call    malloc             ; rax = ptr o NULL

    test    rax, rax
    je      .return_null_node_create

    ; ─── Inicializar campos del nodo ─────────────────────────────────
    and     ecx, 0xFF          ; aseguramos que sólo queden los 8 bits bajos

    mov     qword [rax],      0    ; node->next     = NULL
    mov     qword [rax + 8],  0    ; node->previous = NULL
    mov     byte  [rax + 16], cl   ; node->type     = (uint8_t)type
    mov     qword [rax + 24], rdx  ; node->hash     = hash

    ret

.return_null_node_create:
    xor     eax, eax               ; rax = NULL
    ret


string_proc_list_add_node_asm:
    ; ─── Prólogo ───────────────────────────────────────────────────────
    push    rbx                ; rbx es callee‑saved, lo vamos a usar
                                ; para guardar el puntero a la lista

    mov     rbx, rdi           ; rbx = list (lo necesitamos tras la llamada)

    ; ─── Crear el nuevo nodo ───────────────────────────────────────────
    ; 1er parámetro (type) → edi
    ; 2do parámetro (hash) → rsi
    mov     edi, esi           ; type
    mov     rsi, rdx           ; hash
    call    string_proc_node_create_asm

    ; rax = new_node (o NULL si falló malloc)
    test    rax, rax
    je      .fin               ; si malloc falló, salir sin hacer nada

    ; ─── ¿La lista está vacía? ─────────────────────────────────────────
    cmp     qword [rbx], 0     ; list->first == NULL ?
    je      .lista_vacia

    ; ─── Lista NO vacía: encadenar al final ────────────────────────────
    mov     rcx, [rbx + 8]     ; rcx = list->last (puntero al último nodo)

    ; last->next  = new_node
    mov     [rcx], rax         ; offset 0 dentro de string_proc_node

    ; new_node->previous = last
    mov     [rax + 8], rcx     ; offset 8 = previous

    ; list->last = new_node
    mov     [rbx + 8], rax

    jmp     .fin

.lista_vacia:
    ; list->first = new_node
    mov     [rbx],     rax
    ; list->last  = new_node
    mov     [rbx + 8], rax
    ; new_node ya tiene next = NULL y previous = NULL porque
    ;  lo inicializa string_proc_node_create_asm

.fin:
    pop     rbx
    ret


string_proc_list_concat_asm:
    ; rdi = list, sil = type, rdx = hash
    push    rbx
    push    r12
    push    r13
    push    r14
    push    r15

    test    rdi, rdi
    je      .return_null_concat

    mov     rbx, rdi            ; rbx = list
    mov     r12, rdx            ; r12 = hash
    mov     r13b, sil           ; r13b = type

    mov     r14, [rbx]          ; r14 = list->first
    test    r14, r14
    je      .return_null_concat

    ; malloc(strlen(hash) + 1)
    mov     rdi, r12
    call    strlen
    inc     rax
    mov     rdi, rax
    call    malloc
    test    rax, rax
    je      .return_null_concat

    mov     r15, rax            ; r15 = result
    mov     rdi, r15
    mov     rsi, r12
    call    strcpy              ; strcpy(result, hash)

.loop_concat:
    test    r14, r14
    je      .done_concat

    ; if (current->type == type)
    movzx   eax, byte [r14 + 16]
    cmp     al, r13b
    jne     .skip_concat

    ; result = str_concat(result, current->hash)
    mov     rdi, r15
    mov     rsi, [r14 + 24]
    call    str_concat

    ; free(result)
    mov     rdi, r15
    call    free

    mov     r15, rax            ; result = temp

.skip_concat:
    mov     r14, [r14]          ; current = current->next
    jmp     .loop_concat

.done_concat:
    mov     rax, r15

.cleanup_concat:
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    ret

.return_null_concat:
    xor     rax, rax
    jmp     .cleanup_concat