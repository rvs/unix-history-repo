!
! BOOTSTRAP ON HK, LEAVING SINGLE USER
!
SET DEF HEX
SET DEF LONG
SET REL:0
HALT
UNJAM
INIT
LOAD BOOT
D R10 2		! DEVICE CHOICE 2=HK
D R11 2		! 2= RB_SINGLE
START 2
