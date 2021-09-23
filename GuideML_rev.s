VERSION = 3
REVISION = 15

.macro DATE
.ascii "21.6.2008"
.endm

.macro VERS
.ascii "GuideML 3.15"
.endm

.macro VSTRING
.ascii "GuideML 3.15 (21.6.2008)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: GuideML 3.15 (21.6.2008)"
.byte 0
.endm
