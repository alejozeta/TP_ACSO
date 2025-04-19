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
    ;string_proc_node* string_proc_node_create(uint8_t type, char* hash) 
    push    rbp
    mov     rbp, rsp
    sub     rsp, 16                 ; espacio local:
                                    ;   [rbp‑8]  → hash  (8 B)
                                    ;   [rbp‑12] → type  (4 B)
                                    ;              4 B padding


    mov     dword [rbp-12], edi     ; type   (byte bajo válido)
    mov     qword [rbp-8],  rsi     ; hash

    mov     edi, 32                 ; sizeof(string_proc_node) = 32
    call    malloc                  ; rax = puntero o NULL

    test    rax, rax
    je      .return_null

    mov     ecx, dword [rbp-12]     ; ecx = type (32 bits)
    and     ecx, 0xFF               ; nos quedamos con 8 bits
    mov     rdx, qword [rbp-8]      ; rdx = hash

    mov     qword [rax],      0     ; next     = NULL
    mov     qword [rax + 8],  0     ; previous = NULL
    mov     byte  [rax + 16], cl    ; type     = (uint8_t)type
    mov     qword [rax + 24], rdx   ; hash

    leave
    ret

.return_null:
    xor     eax, eax                ; rax = NULL
    leave
    ret

string_proc_list_add_node_asm:
    ; void string_proc_list_add_node(string_proc_list* list, uint8_t type, char* hash)
    push    rbp
    mov     rbp, rsp
    push    rbx                     ; rbx es callee‑saved → salvamos
                                    ; rbx contendrá 'list'

    mov     rbx, rdi                ; rbx = list (primer argumento)

    ; ─── Crear el nuevo nodo ─────────────────────────────────────────
    mov     edi, esi                ; 1er parámetro (type)  → edi
    mov     rsi, rdx                ; 2º parámetro (hash)  → rsi
    call    string_proc_node_create_asm
                                     ; rax = new_node o NULL
    test    rax, rax
    je      .fin                    ; si malloc falló → salir

    ; ─── ¿Lista vacía? ───────────────────────────────────────────────
    cmp     qword [rbx], 0          ; list->first == NULL ?
    je      .lista_vacia

    ; ---- Lista NO vacía --------------------------------------------
    mov     rcx, [rbx + 8]          ; rcx = list->last
    mov     [rcx], rax              ; last->next        = new_node
    mov     [rax + 8], rcx          ; new_node->previous = last
    mov     [rbx + 8], rax          ; list->last        = new_node
    jmp     .fin

.lista_vacia:
    mov     [rbx],     rax          ; list->first = new_node
    mov     [rbx + 8], rax          ; list->last  = new_node
    ; (new_node ya tiene next = previous = NULL)

.fin:
    pop     rbx
    leave
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