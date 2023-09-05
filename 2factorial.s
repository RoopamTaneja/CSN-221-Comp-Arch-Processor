# Finds factorial of a (not very large) positive no
factorial:
    li a4, 12 # Value whose factorial needs to be found
    sw a4, 0x00000000(zero)

load_val:
    lw a4, 0x00000000(zero) # loading from memory
    li a5, 1
    li a6, 1

loop:
    blt a4, a6, store_n_print
    mul a5, a5, a4
    addi a4, a4, -1
    jal zero, loop

store_n_print:
    sw a5, 0x00000004(zero) # storing result
    # printing result
    li a0, 1
    lw a1, 0x00000004(zero)
    li a7, 64 # to print
    ecall
    li a0, 10
    ecall
