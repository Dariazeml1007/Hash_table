.section .text
.global search_word_table_asm
.type search_word_table_asm, @function

search_word_table_asm:
    push rbp
    mov rbp, rsp

    ; Сохраняем входные параметры
    ; table = rdi
    ; word  = rsi

    ; Загружаем хэш слова
    call hash_intrinsic
    xor rax, rax
    mov rcx, QWORD PTR [rdi + 8*rax]  ; buckets[index]

.loop:
    test rcx, rcx
    jz .not_found

    ; Грузим слово из HashEntry->word (32 байта)
    vmovdqu ymm0, [rcx]       ; entry->word
    vpcmpeqb ymm0, ymm0, [rsi] ; сравниваем с word
    vpmovmskb eax, ymm0
    cmp eax, -1
    je .found

    mov rcx, [rcx + 40]        ; entry = entry->next
    jmp .loop

.found:
    mov eax, [rcx + 32]        ; return entry->count
    pop rbp
    ret

.not_found:
    mov eax, -1                ; HASH_NOT_FOUND_WORD
    pop rbp
    ret
