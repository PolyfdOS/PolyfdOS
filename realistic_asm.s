; realistic_asm.s - Assembly implementation of three-valued logic
; Low-level operations for the Realistic type
; Organization: Daftyon | Morocco

section .text

; realistic_not_asm: Assembly implementation of NOT
; Input: EAX = realistic value (0=FALSE, 1=TRUE, 2=REALISTIC)
; Output: EAX = negated value
global realistic_not_asm
realistic_not_asm:
    cmp eax, 0          ; Is it FALSE?
    je .return_true     ; Return TRUE
    
    cmp eax, 1          ; Is it TRUE?
    je .return_false    ; Return FALSE
    
    ; Otherwise REALISTIC or invalid → return REALISTIC
    mov eax, 2
    ret

.return_true:
    mov eax, 1
    ret

.return_false:
    mov eax, 0
    ret

; realistic_and_asm: Assembly implementation of AND
; Input: EAX = first value, EBX = second value
; Output: EAX = result
global realistic_and_asm
realistic_and_asm:
    ; If either is FALSE, result is FALSE
    cmp eax, 0
    je .return_false
    cmp ebx, 0
    je .return_false
    
    ; If both are TRUE, result is TRUE
    cmp eax, 1
    jne .check_realistic
    cmp ebx, 1
    je .return_true
    
.check_realistic:
    ; Otherwise, at least one is REALISTIC
    mov eax, 2
    ret

.return_false:
    mov eax, 0
    ret

.return_true:
    mov eax, 1
    ret

; realistic_or_asm: Assembly implementation of OR
; Input: EAX = first value, EBX = second value
; Output: EAX = result
global realistic_or_asm
realistic_or_asm:
    ; If either is TRUE, result is TRUE
    cmp eax, 1
    je .return_true
    cmp ebx, 1
    je .return_true
    
    ; If both are FALSE, result is FALSE
    cmp eax, 0
    jne .return_realistic
    cmp ebx, 0
    je .return_false
    
.return_realistic:
    mov eax, 2
    ret

.return_false:
    mov eax, 0
    ret

.return_true:
    mov eax, 1
    ret

; realistic_xor_asm: Assembly implementation of XOR
; Input: EAX = first value, EBX = second value
; Output: EAX = result
global realistic_xor_asm
realistic_xor_asm:
    ; If either is REALISTIC, result is REALISTIC
    cmp eax, 2
    je .return_realistic
    cmp ebx, 2
    je .return_realistic
    
    ; Both are known (TRUE or FALSE)
    ; XOR them directly
    xor eax, ebx
    ret

.return_realistic:
    mov eax, 2
    ret

; realistic_to_int: Convert realistic_t to integer for testing
; Input: EAX = realistic value
; Output: EAX = integer (0, 1, or 2)
global realistic_to_int
realistic_to_int:
    ; Already in correct format, just return
    ret

; realistic_from_bool: Convert boolean (0/1) to realistic_t
; Input: EAX = boolean value (0 or non-zero)
; Output: EAX = realistic value (R_FALSE or R_TRUE)
global realistic_from_bool
realistic_from_bool:
    test eax, eax       ; Check if zero
    jz .return_false
    mov eax, 1          ; Non-zero → TRUE
    ret

.return_false:
    mov eax, 0
    ret

; realistic_combine: Combine two values with custom operation
; Input: EAX = first value, EBX = second value, ECX = operation
;        ECX: 0=AND, 1=OR, 2=XOR, 3=IMPLIES, 4=EQUIV
; Output: EAX = result
global realistic_combine
realistic_combine:
    push ebx
    push ecx
    
    cmp ecx, 0
    je .do_and
    cmp ecx, 1
    je .do_or
    cmp ecx, 2
    je .do_xor
    
    ; Default: return REALISTIC
    mov eax, 2
    pop ecx
    pop ebx
    ret

.do_and:
    call realistic_and_asm
    pop ecx
    pop ebx
    ret

.do_or:
    call realistic_or_asm
    pop ecx
    pop ebx
    ret

.do_xor:
    call realistic_xor_asm
    pop ecx
    pop ebx
    ret
