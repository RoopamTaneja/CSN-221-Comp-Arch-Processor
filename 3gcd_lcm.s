.globl gcd_lcm

# Finds gcd and lcm of two non-zero positive nos
gcd_lcm:
    li a4, 81
    li a5, 45
    sw a4, 0x00000000(zero)
    sw a5, 0x00000004(zero)

load_val:
    lw a4, 0x00000000(zero)
    lw a5, 0x00000004(zero) 

find_min: # a6 = min(a4,a5)
    blt a4, a5, load_other
    addi a6, a5, 0
    j loop_gcd

load_other:
    addi a6, a4, 0

loop_gcd:
    li a3, 1
    blt a6, a3, exit # while (a6>0)
    rem a0, a4, a6 # a0 = a4%a6
    rem a1, a5, a6 # a1 = a4%a6
    or a2, a0, a1 # a0 or a1 == 0 only iff both rem == 0
    beq a2, zero, store_n_print_gcd # in that case it's our gcd
    addi a6,a6,-1 # a6--
    j loop_gcd

store_n_print_gcd:
    sw a6, 0x00000008(zero)
    li a0, 1
    lw a1, 0x00000008(zero)
    li a7, 64
    ecall

lcm:
    lw a6, 0x00000008(zero) # load gcd
    # lcm = (a*b)/gcd
    div a7, a4, a6
    mul a7, a7, a5

store_n_print_lcm:
    sw a7, 0x0000000C(zero)
    li a0,1
    lw a1, 0x0000000C(zero)
    li a7, 64
    ecall

exit:
    jr ra