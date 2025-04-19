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
    mov     edi, 32
    call    malloc
    test    rax, rax
    je      .return_null_node_create

    mov     rdx, rsi              ; hash backup

    ; ESTA ES LA PARTE IMPORTANTE
    mov     ecx, edi              ; tomar 'type' como int
    and     ecx, 0xFF             ; asegurar uint8_t
    mov     byte [rax + 16], cl   ; guardar solo el byte

    mov     qword [rax], 0
    mov     qword [rax + 8], 0
    mov     qword [rax + 24], rdx

    ret

.return_null_node_create:
    xor     rax, rax
    ret



string_proc_list_add_node_asm:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 48

    mov     [rbp-24], rdi      ; list
    mov     eax, esi           ; type (int → eax)
    mov     [rbp-40], rdx      ; hash
    mov     [rbp-28], al       ; guarda solo el byte bajo (type)

    movzx   eax, byte [rbp-28] ; eax = type & 0xFF
    mov     rdx, [rbp-40]      ; rdx = hash
    mov     rsi, rdx
    mov     edi, eax           ; edi = type
    call    string_proc_node_create_asm

    mov     [rbp-8], rax
    cmp     qword [rbp-8], 0
    je      .L13

    mov     rax, [rbp-24]
    mov     rax, [rax]
    test    rax, rax
    jne     .L12

    mov     rax, [rbp-24]
    mov     rdx, [rbp-8]
    mov     [rax], rdx
    mov     rax, [rbp-24]
    mov     rdx, [rbp-8]
    mov     [rax+8], rdx
    jmp     .L9

.L12:
    mov     rax, [rbp-24]
    mov     rax, [rax+8]
    mov     rdx, [rbp-8]
    mov     [rax], rdx
    mov     rax, [rbp-24]
    mov     rdx, [rax+8]
    mov     rax, [rbp-8]
    mov     [rax+8], rdx
    mov     rax, [rbp-24]
    mov     rdx, [rbp-8]
    mov     [rax+8], rdx
    jmp     .L9

.L13:
    nop

.L9:
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