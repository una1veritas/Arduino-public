
    org     01000h
start:
    ld      hl, segseq + 7
    ld      b, 8
loop:
    ld      a, (hl)
    out     (129), a
    dec     hl
    djnz    loop
    jp      0000h

    org     01010h
segseq:
    db  $c0, $f9, $a4, $b0, $99, $88, $83, $c6