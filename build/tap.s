
@{{BLOCK(tap)

@=======================================================================
@
@	tap, 32x32@4, 
@	Transparent color : 00,00,00
@	+ palette 2 entries, not compressed
@	+ 16 tiles not compressed
@	Total size: 4 + 512 = 516
@
@	Time-stamp: 2019-11-29, 03:19:27
@	Exported by Cearn's GBA Image Transmogrifier, v0.8.15
@	( http://www.coranac.com/projects/#grit )
@
@=======================================================================

	.section .rodata
	.align	2
	.global tapTiles		@ 512 unsigned chars
	.hidden tapTiles
tapTiles:
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
	.word 0x00000000,0x00000000,0x00000000,0x11000000,0x11100000,0x11110000,0x11111000,0x11111100
	.word 0x00000000,0x00000000,0x00000000,0x00000001,0x00000001,0x00000011,0x00000111,0x00000111
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
	.word 0x00000000,0x00000000,0x00000000,0x10000000,0x10000000,0x11000000,0x11000000,0x11000000
	.word 0x00111110,0x00011111,0x00001111,0x00000111,0x00000111,0x00000011,0x10000011,0x11111111
	.word 0x00001111,0x00001111,0x00011110,0x00011110,0x00111100,0x00111100,0x01111011,0x01111111
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000

	.word 0x11000000,0x11000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
	.word 0x11111111,0x11111111,0x11000000,0x11000000,0x11000000,0x11000000,0x11000000,0x11000000
	.word 0x01111111,0x01111111,0x01110011,0x00000011,0x00000001,0x00000001,0x00000001,0x00000001
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000
	.word 0x11000000,0x11000000,0x11000000,0x11000000,0x11000000,0x11000000,0x10000000,0x00000000
	.word 0x00000001,0x00000001,0x00000001,0x00000001,0x00000011,0x00000011,0x00000011,0x00000000
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000

	.section .rodata
	.align	2
	.global tapPal		@ 4 unsigned chars
	.hidden tapPal
tapPal:
	.hword 0x0000,0x7FFF

@}}BLOCK(tap)
