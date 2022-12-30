VERSION = 3
REVISION = 17

.macro DATE
.ascii "30.12.2022"
.endm

.macro VERS
.ascii "GuideML 3.17"
.endm

.macro VSTRING
.ascii "GuideML 3.17 (30.12.2022)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: GuideML 3.17 (30.12.2022)"
.byte 0
.endm
