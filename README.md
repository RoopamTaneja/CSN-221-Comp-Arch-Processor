
#GUI :
https://gitlab.com/luplab/rvcodecjs -- repo
https://luplab.gitlab.io/rvcodecjs/ -- link

ONLY RV32I + M ext + li pseudo
(No floating points)

types of lines in asm : 
1) blank lines
2) labels - <label_name><:><space><only comments(optional)>
3) instr
PS : No instr after label in same line
4) comments can come after 2, 3, or solely comments also
5) addi r1, r2, r3 (1 whitespace only, whitespace is important, commas not so much)
6) pls no extra instructions and no wrong syntaxes

Labels support for b-type and j-type, u can give labels or even write numeric offsets
pls don't start label names with digits


In your final binary, don't have any blank lines in start, middle or end (V.IMP), or anything else except pure 0s and 1s;

Data file format : 
0x0000: data
0x0004: data
..
4 digit hex addr and data in format above 
in data file : don't have any blank lines in start, middle or end (V.IMP), or anything else

READS and Writes only decimal values (as of now) from and to Data Memory

your memory addr in instr should be range : 0x00 to (i mean it's quite customisable so)