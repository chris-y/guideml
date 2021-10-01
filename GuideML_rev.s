VERSION = 3
REVISION = 16

.macro DATE
.ascii "1.10.2021"
.endm

.macro VERS
.ascii "GuideML 3.16"
.endm

.macro VSTRING
.ascii "GuideML 3.16 (1.10.2021)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: GuideML 3.16 (1.10.2021)"
.byte 0
.endm
