    org     1000h
start:
    ld      ix, 1010h
    ld      b, 8
loop:
    ld      a, (ix)
    out     (129), a
    inc     ix
    djnz    loop
    jp  0000h