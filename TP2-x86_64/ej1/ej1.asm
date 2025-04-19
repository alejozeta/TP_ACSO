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
    call malloc           
    test rax, rax         ; si falla salta a error(fallo malloc)  
    je   .err

    mov qword [rax],    0 ; los seteo a null
    mov qword [rax+8],  0 
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
    sub     rsp, 16                 ; hash + type = 12B + 4B padding  

    mov     dword [rbp-12], edi     ; type   (byte bajo válido)
    mov     qword [rbp-8],  rsi     ; hash

    mov     edi, 32                 ; sizeof(string_proc_node) = 32 
    call    malloc                  ;

    test    rax, rax
    je      .return_null            ; si falla malloc

    mov     ecx, dword [rbp-12]     ; 
    and     ecx, 0xFF               ; nos quedamos con 8 bits del ecx
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
    push    rbx                     

    mov     rbx, rdi                ; rbx = list 

    mov     edi, esi                ; edi = type
    mov     rsi, rdx                ; rsi = hash
    call    string_proc_node_create_asm
    test    rax, rax
    je      .fin                    ; si falla malloc termino

    cmp     qword [rbx], 0          ; esta vacia la lista?
    je      .lista_vacia

    mov     rcx, [rbx + 8]          ; rcx = list->last
    mov     [rcx], rax              ; last->next        = new_node
    mov     [rax + 8], rcx          ; new_node->previous = last
    mov     [rbx + 8], rax          ; list->last        = new_node
    jmp     .fin

.lista_vacia:
    mov     [rbx],     rax          ; list->first = new_node
    mov     [rbx + 8], rax          ; list->last  = new_node

.fin:
    pop     rbx
    leave
    ret


string_proc_list_concat_asm:
    push    rbp
    mov     rbp, rsp
    push    rbx
    push    r12
    push    r13
    push    r14
    push    r15

    mov     rbx, rdi           ; list
    mov     r13d, esi          ; type (byte bajo = r13b)
    mov     r15, rdx           

    ; if (list == NULL || list->first == NULL) return NULL
    test    rbx, rbx
    je      .ret_null
    mov     r12, [rbx]         ; current = list->first
    test    r12, r12
    je      .ret_null

    ; result = malloc(strlen(hash)+1)
    mov     rdi, r15            ; hash
    call    strlen
    inc     rax
    mov     rdi, rax
    call    malloc
    test    rax, rax
    je      .ret_null           ; si falla malloc salto
    mov     r14, rax            ; result

    ; strcpy(result, hash)
    mov     rdi, r14           
    mov     rsi, r15           ; src  (hash guardado)
    call    strcpy


.loop: ;looooooops
    test    r12, r12
    je      .done

    cmp     byte [r12 + 16], r13b    ; current->type == type ?
    jne     .next

    mov     rdi, r14
    mov     rsi, [r12 + 24]
    call    str_concat
    mov     r15, rax

    mov     rdi, r14                 ; free(result)
    call    free

    mov     r14, r15                 ; result = temp

.next:
    mov     r12, [r12]               ; current = current->next
    jmp     .loop

.done:
    mov     rax, r14                 ; return result
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