// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

@8192
D=A
@n
M=D // n = 8192

@SCREEN
D=A
@addr
M=D

(LOOP)
    @KBD
    D=M

    // i = 0
    @i
    M=0

    @BLACKEN
    D;JNE

    @WHITEN
    D;JEQ

    @LOOP
    0;JMP

(BLACKEN)
    @n
    D=M
    @i
    D=D-M

    @END
    D;JEQ

    // blacken
    @addr
    D=M
    @i
    A=D+M
    M=-1

    @i
    M=M+1 // i++

    @BLACKEN
    0;JMP

(WHITEN)
    @n
    D=M
    @i
    D=D-M

    @END
    D;JEQ

    @addr
    D=M
    @i
    A=D+M
    M=0

    @i
    M=M+1 // i++

    @WHITEN
    0;JMP

(END)
    @LOOP
    0;JMP
