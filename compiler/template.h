#define MIPS_LI		"li		%0, %1"

#define MIPS_SW		"sw		%1, %0"
#define MIPS_LW		"lw		%0, %1"

#define MIPS_MOVE	"move	%0, %1"
#define MIPS_ADDIU	"addiu	%0, %0, %1"
#define MIPS_ADDU	"addu	%0, %1, %0"

#define MIPS_SUBU	"subu	%0, %1, %0"

#define MIPS_MUL	"mul		%0, %0, %1"
#define MIPS_DIV	"div		%0, %0, %1"

#define MIPS_SLL	"sll		%0, %1, %2"

#define MIPS_SLT	"slt		%0, %0, %1"

#define MIPS_BEQ	"beq		%1, %2, %0;nop"		
#define MIPS_BNE	"bne		%1, %2, %0;nop"	

#define MIPS_B		"b		%0"

#define MIPS_JAL	"jal		%1"