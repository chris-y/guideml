VERSION		EQU	3
REVISION	EQU	16

DATE	MACRO
		dc.b '1.10.2021'
		ENDM

VERS	MACRO
		dc.b 'GuideML 3.16'
		ENDM

VSTRING	MACRO
		dc.b 'GuideML 3.16 (1.10.2021)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: GuideML 3.16 (1.10.2021)',0
		ENDM
