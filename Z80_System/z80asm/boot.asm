;
SBOTTOM equ 	$ef00
MONITOR equ     $f000
;
	    org 	$0000
RST_00:
	    ld 		sp, SBOTTOM
	    jp  	MONITOR
;
