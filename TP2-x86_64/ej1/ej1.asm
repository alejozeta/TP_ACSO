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

    mov     rdx, rsi             ; hash backup
    movzx   ecx, dil             ; type en cl
    mov     qword [rax], 0       ; next = NULL
    mov     qword [rax + 8], 0   ; previous = NULL
    mov     byte [rax + 16], cl  ; type = cl
    mov     qword [rax + 24], rdx; hash = rdx

    ret

.return_null_node_create:
    xor     rax, rax
    ret


string_proc_list_add_node_asm:
    push    rbx
    mov     rbx, rdi             ; rbx = list

    mov     edi, esi             ; tomar type desde 2do arg
    and     edi, 0xFF            ; asegurar unsigned char
    mov     rsi, rdx             ; rsi = hash
    call    string_proc_node_create_asm

    test    rax, rax
    je      .return_list_add

    mov     rcx, [rbx]
    test    rcx, rcx
    je      .empty_list_add

    mov     rdx, [rbx + 8]
    mov     [rdx], rax
    mov     [rax + 8], rdx
    mov     [rbx + 8], rax
    jmp     .return_list_add

.empty_list_add:
    mov     [rbx], rax
    mov     [rbx + 8], rax

.return_list_add:
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