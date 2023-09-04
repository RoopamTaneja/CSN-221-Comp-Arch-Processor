# Checks if a given positive no is prime or not
prime:
    li a5, 103 # Value to be checked
    sw a5, 0x00000000(zero)

load_val:
    lw a5, 0x00000000(zero)
    li a4, 2

loop:
    blt a4, a5, divide_check 
    jal zero, true

divide_check:
    rem a6, a5, a4
    beq a6, zero, false # rem == 0 => not prime
    addi a4, a4, 1 # else check for next divisor
    jal zero, loop

false:
    li a0, 0 # not prime
    sw a0, 0x00000004(zero)
    jal zero, print_result

true:
    li a0, 1 # prime
    sw a0, 0x00000004(zero)

print_result:
    # syscall code goes into a7
    li a0,1 # 1 = stdout
    lw a1, 0x00000004(zero)
    li a7, 64 # to print
    ecall
    li a0, 10
    ecall
