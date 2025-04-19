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
    push rbp
    mov  rbp, rsp

    mov  edi, 16          ; sizeof(string_proc_list)
    call malloc           ; rax ← bloque de 16 B
    test rax, rax
    je   .err

    mov qword [rax],    0 ; first = NULL
    mov qword [rax+8],  0 ; last  = NULL
    leave
    ret
.err:
    xor eax, eax
    leave
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
    ; ─── prólogo ──────────────────────────────────────────────
    push    rbp
    mov     rbp, rsp
    push    rbx
    push    r12
    push    r13
    push    r14
    push    r15                ;   ←  NUEVO: usaremos r15

    mov     rbx, rdi           ; list
    mov     r13d, esi          ; type (byte bajo: r13b)

    ; if (list == NULL || list->first == NULL)  return NULL
    test    rbx, rbx
    je      .ret_null
    mov     r12, [rbx]         ; current = list->first
    test    r12, r12
    je      .ret_null

    ; result = malloc(strlen(hash)+1)
    mov     rdi, rdx           ; hash
    call    strlen
    inc     rax
    mov     rdi, rax
    call    malloc
    test    rax, rax
    je      .ret_null
    mov     r14, rax           ; result

    ; strcpy(result, hash)
    mov     rdi, r14
    mov     rsi, rdx
    call    strcpy

.loop:
    test    r12, r12
    je      .done

    cmp     byte [r12 + 16], r13b   ; current->type == type ?
    jne     .next

    ; ----- temp = str_concat(result, current->hash) -----
    mov     rdi, r14
    mov     rsi, [r12 + 24]         ; current->hash
    call    str_concat              ; rax = temp
    mov     r15, rax                ;   ←  salvar temp ANTES de free

    mov     rdi, r14                ; free(result)
    call    free

    mov     r14, r15                ; result = temp

.next:
    mov     r12, [r12]              ; current = current->next
    jmp     .loop

.done:
    mov     rax, r14                ; return result

    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    leave
    ret

.ret_null:
    xor     eax, eax
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    leave
    ret
    