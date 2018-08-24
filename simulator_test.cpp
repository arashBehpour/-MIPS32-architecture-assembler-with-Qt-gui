#include "catch.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "simulator.hpp"

#include <string>

TEST_CASE("test simulator .asm files", "[simulator]") 
{
	
	{// test unused registers
		std::string input = R"(
			.data
		VALUE = 34

			.text
			main:
        li $1, VALUE
        li $2, 7
        li $3, 7
        li $4, 7
        li $5, 7
        li $6, 7
        li $7, 7
        li $14, 7
		li $15, 7
        li $16, 7
        li $17, 7
        li $18, 7
        li $19, 7
        li $20, 7
		li $21, 7
        li $22, 7
        li $23, 7
		li $26, 7
        li $27, 7
        li $28, 7
        li $29, 7
        li $30, 7
		li $31, 7
        li $lo, 7
		li $hi, 7
        li $pc, 7)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);
		
		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		
		REQUIRE(vm.getPcValue() == 25);
		REQUIRE(vm.isEndOfFileCheck() == false);
		REQUIRE(vm.getErrorStatus() == false);
		
		vm.clearDataStruct();	

	}
	


	{// test uncovered code part 2
	std::string input = R"(
	.data
VALUE = 34

        .text
main:
        la $t5, VALUE
        la $2, 1(7)
		la $2, $t5
		li $t0, 7
		la $t2, 3($t0)
        multu $t0, $t0
        li $23, 2
		divu $t0, $23)";  // raw string

	std::istringstream iss(input);
	TokenList tList = tokenize(iss);

	Parser tListParsed(tList);
	tListParsed.parseFile();
	REQUIRE(tListParsed.successfullyParsed() == true);

	Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());

	vm.update_simulation_public();
	vm.update_pcReg();
	vm.update_simulation_public();
	vm.update_pcReg();
	vm.update_simulation_public();
	vm.update_pcReg();
	vm.update_simulation_public();
	vm.update_pcReg();
	vm.update_simulation_public();
	vm.update_pcReg();
	vm.update_simulation_public();
	vm.update_pcReg();
	vm.update_simulation_public();
	vm.update_pcReg();
	vm.update_simulation_public();
	vm.update_pcReg();

	REQUIRE(vm.getPcValue() == 8);
	REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test00.asm
		std::string input = R"( # simple infinite loop
				.data
				.text
			main:	j main)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 0);
		REQUIRE(vm.isEndOfFileCheck() == false);
	}

	{// test01.asm
		std::string input = R"( 
	.data
        .space 8
var1:   .word 1
var2:   .word -2

        .text
main:
     	la $t0, var1

	lw $t1, 0
	lw $t2, $t0
	lw $t3, 4($t0)
	lw $t4, 4(var1)
	lw $t5, var2)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 6);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test02.asm
		std::string input = R"( 
	.data
r1:     .space 4
r2:     .space 12
r3:     .space 4
var:    .word 7

        .text
main:
        la $t0, r2
     	lw $t1, var

	sw $t1, 0
	sw $t1, $t0
	sw $t1, 4($t0)
	sw $t1, 8(r2)
	sw $t1, r3)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 7);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test03.asm
		std::string input = R"(  
	.data
A = 0
B = 1
C = 2
D = 4
        .text
main:
        li $t0, 100
        li $t1, A
        li $t2, B
        li $t3, C
        li $t4, D)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 5);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test04.asm
		std::string input = R"(
	.data
VALUE = -1234

        .text
main:
        li $t0, VALUE
        move $t1, $t0
        move $t2, $t1
        move $t3, $t2
        move $t4, $t3
        move $t5, $t4
        move $t6, $t5
        move $t7, $t6)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 2);
		REQUIRE(vm.isEndOfFileCheck() == false);
	}

	{// test05.asm
		std::string input = R"( 
	.data
VALUE = -1
var:    .word 1
        .text
main:
        lw $t0, var
        add $t1, $t0, VALUE
        add $t2, $t1, $t0)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 3);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test06.asm
		std::string input = R"( 
	.data
VALUE = 12
var:    .word 31
        .text
main:
        lw $t0, var
        addu $t1, $t0, VALUE # 31+12=43
        addu $t2, $t1, $t0 # 43+31=74)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 3);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test07.asm
		std::string input = R"( 
	.data
VALUE = 2
var1:   .word 1
var2:   .word 12
var3:   .word -1
        .text
main:
        lw $t0, var1
        lw $t1, var2
        lw $t2, var3
        sub $t3, $t0, VALUE # 1-2 = -1
        sub $t4, $t1, $t0 # 12-1 = 11
        sub $t5, $t2, VALUE # -1-2 = -3)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 6);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test08.asm
		std::string input = R"( 
	.data
VALUE = 2
var1:   .word 1
var2:   .word 12
var3:   .word -1
        .text
main:
        lw $t0, var1
        lw $t1, var2
        lw $t2, var3
        subu $t3, $t0, VALUE # 1-2 = -1 = 4294967295
        subu $t4, $t1, $t0   # 12-1 = 11
        subu $t5, $t2, VALUE # -1-2 = -3 = 4294967293)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 6);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test09.asm
		std::string input = R"( 
	.data
VALUE = 4
VALUE2 = -4
var1:   .word 2
var2:   .word -2
var3:   .word 1073741824 # = 2^30
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        mult $t0, $t1 # 2*4 = 8
        mflo $t8
        mfhi $t9

        lw $t0, var2
        lw $t1, var1
        mult $t0, $t1 # -2*2 = -4 
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE
        mult $t0, $t1 # 1073741824*4 = 4294967296 (overflow)
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE2
        mult $t0, $t1 # 1073741824*-4 = -4294967296 (overflow)
        mflo $t8
        mfhi $t9)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();

		REQUIRE(vm.getPcValue() == 20);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test10.asm
		std::string input = R"( 
	.data
VALUE = 4
var1:   .word 2
var2:   .word 1073741824 # = 2^30
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        mult $t0, $t1 # 2*4 = 8
        mflo $t8
        mfhi $t9

        lw $t0, var2
        li $t1, VALUE
        mult $t0, $t1 # 1073741824*4 = 4294967296 (overflow)
        mflo $t8
        mfhi $t9)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 10);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test11.asm
		std::string input = R"(  
	.data
VALUE = 4
VALUE2 = -4
var1:   .word 2
var2:   .word -2
var3:   .word 1073741824 # = 2^30
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        div $t0, $t1 # 2/4 = 0 rem 2
        mflo $t8
        mfhi $t9

        lw $t0, var2
        lw $t1, var1
        div $t0, $t1 # -2/2 = -1 rem 0 
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE
        div $t0, $t1 # 1073741824/4 = 268435456 rem 0
        mflo $t8
        mfhi $t9

        lw $t0, var3
        li $t1, VALUE2
        div $t0, $t1 # 1073741824/-4 = -268435456 rem 0 
        mflo $t8
        mfhi $t9

        # divide by 0
        div $t0, $0)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();

		REQUIRE(vm.getPcValue() == 21);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test12.asm
		std::string input = R"( 
	.data
VALUE = 4
var1:   .word 2
var2:   .word 1073741825 # = 2^30+1
        .text
main:
        lw $t0, var1
        li $t1, VALUE
        div $t0, $t1 # 2/4 = 0 rem 2
        mflo $t8
        mfhi $t9

        lw $t0, var2
        li $t1, VALUE
        div $t0, $t1 # 1073741825/4 = 268435456 rem 1
        mflo $t8
        mfhi $t9

        # divide by 0
        div $t0, $0)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 11);
		REQUIRE(vm.isEndOfFileCheck() == true);
}

	{// test13.asm
		std::string input = R"( 
	.data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        and $t2, $t0, $t1  
        and $t2, $t0, VALUE)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
	
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 4);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test14.asm
		std::string input = R"( 
	.data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        nor $t2, $t0, $t1  
        nor $t2, $t0, VALUE)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 4);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test15.asm
		std::string input = R"( 
	.data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        or $t2, $t0, $t1  
        or $t2, $t0, VALUE)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
	
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 4);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test16.asm
		std::string input = R"( 
	.data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        xor $t2, $t0, $t1  
        xor $t2, $t0, VALUE)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
	
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 4);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test17.asm
		std::string input = R"( 
	.data
VALUE = 3
var1:   .word 12
var2:   .word 10
        .text
main:
        lw $t0, var1
        lw $t1, var2
        not $t2, $t0
        not $t2, $t1
        not $t2, VALUE)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 4);
		REQUIRE(vm.isEndOfFileCheck() == false);
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.getPcValue() == 5);
		REQUIRE(vm.isEndOfFileCheck() == true);
	}

	{// test18.asm
		std::string input = R"( 
	.data
        .text
main:
        nop
        j next
        nop
next:
        nop
        j main)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.isEndOfFileCheck() == false);
	}

	{// test19.asm
		std::string input = R"( 
	.data
var0:   .word 0
var1:   .word 1
var2:   .word 2
var3:   .word 3
        .text
main:
        lw $t0, var0
        lw $t1, var1
        lw $t2, var2
        lw $t3, var3

        beq $t0, $t1, check1
        beq $t0, $t0, check1
        nop
check1:
        bne $t0, $t0, check2
        bne $t0, $t1, check2
        nop
check2:
        bgt $t0, $t0, check3
        bgt $t3, $t1, check3
        nop
check3:
        bge $t0, $t1, check4
        bge $t3, $t2, check4
        nop
check4:
        blt $t3, $t1, check5
        blt $t1, $t3, check5
        nop
check5:
        ble $t3, $t1, check6
        ble $t3, $t3, check6
        nop
check6:
        nop
        j check6)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
	
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();

		REQUIRE(vm.isEndOfFileCheck() == false);
	}

	{// test20.asm
		std::string input = R"( # Example program to compute the sum of squares from Jorgensen [2016]
	#---------------------------------------------------------------
	# data declarations
	
	.data
n:		.word 10
sumOfSquares:	.word 0

	#---------------------------------------------------------------
	# the program
	.text
main:
	lw $t0,n
	li $t1,1
	li $t2,0

sumLoop:
	mult $t1, $t1
        mflo $t3
	add $t2, $t2, $t3
	add $t1, $t1, 1
	ble $t1, $t0, sumLoop
	sw  $t2, sumOfSquares

end:
	j end)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);

		Simulator vm(tListParsed.getInitialRegisterContents(), tListParsed.getInitialMemoryLayout(), tListParsed.getProgramInstructions(), tListParsed.getLabelsMap());
	
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		vm.update_simulation_public();
		vm.update_pcReg();
		REQUIRE(vm.isEndOfFileCheck() == false);
	
	}
	
}