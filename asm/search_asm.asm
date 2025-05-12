section .text
global hash_crc32_asm
global search_word_table_asm

hash_crc32_asm:
    xor     eax, eax
    crc32   rax, qword [rdi]    ; Обрабатываем 8 байт за раз
    crc32   rax, qword [rdi+8]
    crc32   rax, qword [rdi+16]
    crc32   rax, qword [rdi+24]
    ret

search_word_table_asm:
    push    rbp
    mov     rbp, rsp
    push    rbx
    push    r12
    push    r13
    push    r14                ; Для накопления суммы count

    ; Проверка входных параметров
    test    rdi, rdi
    jz      .error
    test    rsi, rsi
    jz      .error

    mov     r12, rdi           ; r12 = table
    mov     r13, rsi           ; r13 = word
    xor     r14d, r14d         ; r14d = 0 (общий счетчик)

    ; Вычисляем hash(word) % table->size
    mov     rdi, r13
    call    hash_crc32_asm
    xor     edx, edx
    mov     ecx, [r12]         ; table->size
    div     ecx                ; hash % size
    mov     rbx, [r12 + 8]     ; table->buckets
    mov     rbx, [rbx + rdx*8] ; buckets[index]

    ; Загружаем слово для поиска (32 байта)
    vmovdqu ymm0, [r13]        ; ymm0 = target word

.search_loop:
    test    rbx, rbx           ; Проверка конца цепочки
    jz      .loop_end

    ; Сравнение 32 байт
    vmovdqu ymm1, [rbx]        ; Загружаем слово из таблицы
    vpcmpeqb ymm1, ymm1, ymm0  ; Побайтовое сравнение
    vpmovmskb eax, ymm1        ; Получаем маску
    cmp     eax, 0xFFFFFFFF    ; Проверка полного совпадения
    jne     .next_entry

    ; Совпадение найдено - добавляем count
    mov     eax, [rbx + 32]    ; Загружаем entry->count
    add     r14d, eax          ; Увеличиваем общий счетчик

.next_entry:
    mov     rbx, [rbx + 40]    ; Переходим к следующему элементу
    jmp     .search_loop

.loop_end:
    mov     eax, r14d          ; Возвращаем суммарный count
    jmp     .exit

.error:
    xor     eax, eax           ; Возвращаем 0 при ошибке

.exit:
    vzeroupper                 ; Очищаем AVX-регистры
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    leave
    ret
section .note.GNU-stack noalloc noexec nowrite progbits
