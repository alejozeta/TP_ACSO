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


string_proc_list_create_asm:
    mov edi, 16         ; sizeof(string_proc_list)
    call malloc
    test rax, rax
    jz .fail

    xor rcx, rcx
    mov [rax], rcx      ; first = NULL
    mov [rax+8], rcx    ; last  = NULL
    ret

.fail:
    xor rax, rax
    ret


string_proc_node_create_asm:
    push rbp
    mov rbp, rsp

    mov edi, 32             ; sizeof(string_proc_node)
    call malloc
    test rax, rax
    jz .fail

    mov rcx, rax            ; rcx = pointer to the new node

    mov r8, rdx             ; save hash (3rd arg) in r8 for later use

    xor rdx, rdx
    mov [rcx], rdx          ; node->next = NULL
    mov [rcx+8], rdx        ; node->previous = NULL

    mov byte [rcx+16], sil  ; node->type = type (2nd arg)
    mov [rcx+24], r8        ; node->hash = hash

    mov rax, rcx            ; return the new node
    pop rbp
    ret

.fail:
    xor rax, rax
    pop rbp
    ret


string_proc_list_add_node_asm:
    push rbp
    mov rbp, rsp

    mov rax, [rdi+8]      ; rax = list->last
    test rax, rax
    jz .empty_list

    ; lista no vacía
    mov [rax], rsi        ; last->next = node
    mov [rsi+8], rax      ; node->previous = last
    mov [rdi+8], rsi      ; list->last = node
    jmp .done

.empty_list:
    ; list->first = node
    mov [rdi], rsi
    ; list->last = node
    mov [rdi+8], rsi
    ; node->next = NULL
    xor rax, rax
    mov [rsi], rax
    ; node->previous = NULL
    mov [rsi+8], rax

.done:
    pop rbp
    ret

string_proc_list_concat_asm:
    ; rdi = list1
    ; rsi = list2

    mov rax, [rdi+8]      ; rax = list1->last
    mov rcx, [rsi]        ; rcx = list2->first
    test rax, rax
    jz .list1_empty

    ; conectar list1->last <-> list2->first
    mov [rax], rcx        ; list1->last->next = list2->first
    test rcx, rcx
    jz .skip_link
    mov [rcx+8], rax      ; list2->first->previous = list1->last

.skip_link:
    ; list1->last = list2->last
    mov rdx, [rsi+8]
    mov [rdi+8], rdx
    ret

.list1_empty:
    ; list1 = list2
    mov rax, [rsi]
    mov [rdi], rax
    mov rax, [rsi+8]
    mov [rdi+8], rax
    ret

