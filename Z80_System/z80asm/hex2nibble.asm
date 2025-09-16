    org     0040h

;convert one adecimal char in a to nibble in c 
;
hex2nibble:
    ld      c, 0xff ; the default value is error code.

    cp      'a'     ; check whether a lower case
    jr      c, digit_or_upper  ; a digit or an upper case if carry set
    and     $df     ; lower case to upper case
digit_or_upper:
    cp      '9' + 1 ; check whether a digit
    jr      nc, upper  ; possibly an upper case letter if carry not set
    sub     a, '0'  ; digit to integer value
    ret     s       ; it was not '0' - '9' 
    jr      output
upper:
    cp      'F'+1   ; check the digit whether less than 'F'
    ret     nc      ; error if it is larger than 'F'
    sub     'A'  ; A - F to integer 0 - 5
    ret     s       ; it was not 'A' - 'F'
    add     a, 10   ; A - F to 10 - 15 by +5
output:
    ld      c, a
    ret
