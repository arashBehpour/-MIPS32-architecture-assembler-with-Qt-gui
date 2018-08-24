#include "catch.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#include <string>

// DATATYPE RANGE CHECKING:
//long long int imin = std::numeric_limits< long int>::min(); // minimum value
//long int imax = std::numeric_limits<long int>::max(); // maximum value
//std::cout << imin << " " << imax << std::endl; 

// put your parser tests here

TEST_CASE("test basic data and instruction section", "[parser]") {

	{
		std::string input = ".data\n";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		TokenList::iterator it = tl.begin();
		//An iterator is any object that, pointing to some element in a range of elements (such as an array or a container), has 
		//the ability to iterate through the elements of that range using a set of
		//operators (with at least the increment (++) and dereference (*) operators).

		REQUIRE(*it == Token(STRING, 1, ".data"));
		REQUIRE(*(++it) == Token(EOL, 1));

		Parser t1Parsed(tl);
		t1Parsed.parseFile();
		REQUIRE(t1Parsed.successfullyParsed() == true);
	}
	
	{
		std::string input = ".text\n.data\n";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		TokenList::iterator it = tl.begin();

		REQUIRE(*it == Token(STRING, 1, ".text"));
		REQUIRE(*(++it) == Token(EOL, 1));
		REQUIRE(*(++it) == Token(STRING, 2, ".data"));
		REQUIRE(*(++it) == Token(EOL, 2));

		Parser t1Parsed(tl);
		t1Parsed.parseFile();
		REQUIRE(t1Parsed.successfullyParsed() == true);
	}

	{
		std::string input = ".data\n.text\n";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		TokenList::iterator it = tl.begin();

		Parser t1Parsed(tl);
		t1Parsed.parseFile();
		REQUIRE(t1Parsed.successfullyParsed() == true);
	}
	
	{
		std::string input = "";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		TokenList::iterator it = tl.begin();

		REQUIRE(tl.size() == 0);

		Parser t1Parsed(tl);
		t1Parsed.parseFile();
		REQUIRE(t1Parsed.successfullyParsed() == false);
	}
	
	{
		std::string input = "blehh";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);
		TokenList::iterator it = tl.begin();

		REQUIRE(*it == Token(STRING, 1, "blehh"));
		REQUIRE(*(++it) == Token(EOL, 1));

		Parser t1Parsed(tl);
		t1Parsed.parseFile();
		REQUIRE(t1Parsed.successfullyParsed() == false);
	}
}


TEST_CASE("test the pass assembly files", "[parser]")
{
	{// test00.asm
		std::string input = R"( #ignore line
		# A test file of data declarations only
			.data
		var1:	.word 1024             # int var1 = 1024

		var2:	.half 12               # short var2 = 12
	
		var3:	.byte 0                # char var3 = 0

		var4:	.byte 1, 2, 3, 4, 5, 6, 7, 8  # var4 = {1,2,3,4,5,6,7,8}

		var5:	.space 512             # reserve 512 bytes

		var6:	.ascii "hello"

		var7:	.asciiz "goodbye"

		.text)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);
	}


	{// test02.asm
		std::string input = R"(
		# a test for basic data movement
			.data
		avar:	.word 	0
		bvar:	.half	1
		cvar:	.byte	3

			.text
		main:	
			li $t0, 45
			lw $t1, avar
			lh $t2, bvar
			lb $t2, cvar
			sw $t1, avar
			sh $t2, bvar
			sb $t2, cvar

			move $t0, $0)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);
	}

	{// test03.asm
		std::string input = R"(
		# a test of basic arithmetic operations
			.data
		x:	.word	1
		y:	.word	1
		z:	.space 	4

			.text
		main:
			lw 	$t0, x
			lw 	$t1, y
			add 	$t2, $t0, $t1
			add 	$t2, $t0, 2
			addu 	$t2, $t0, $t1
			addu 	$t2, $t0, 2
			sub 	$t2, $t0, $t1
			sub 	$t2, $t0, 2
			subu 	$t2, $t0, $t1
			subu 	$t2, $t0, 2
			mul 	$t2, $t0, $t1
			mul 	$t2, $t0, 2
			mulo 	$t2, $t0, $t1
			mulo 	$t2, $t0, 2
			mulou 	$t2, $t0, $t1
			mulou 	$t2, $t0, 2
			mult	$t0, $t1
			multu	$t0, $t1
			div 	$t2, $t0, $t1
			div 	$t2, $t0, 2
			divu 	$t2, $t0, $t1
			divu 	$t2, $t0, 2
			div	$t0, $t1
			divu	$t0, $t1
			rem 	$t2, $t0, $t1
			rem 	$t2, $t0, 2
			remu 	$t2, $t0, $t1
			remu 	$t2, $t0, 2
			abs	$t0, $t1
			neg	$t0, $t1
			negu	$t0, $t1)";  // raw string

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);
	}

	
		{// test04.asm
			std::string input = R"(
		# test of basic logical instructions
			.data
		TRUE = 1
		FALSE = 0

		test1:	.word TRUE
		test2:	.word FALSE
	
			.text
	main:
		lw	$t0, test1
		lw	$t1, test2
	
		and	$t2, $t0, $t1
		and	$t2, $t0, TRUE
		nor	$t2, $t0, $t1
		nor	$t2, $t0, TRUE
		not	$t2, $t0
		not	$t2, $t0
		or	$t2, $t0, $t1
		or	$t2, $t0, TRUE
		xor	$t2, $t0, $t1
		xor	$t2, $t0, TRUE)";  // raw string

			std::istringstream iss(input);
			TokenList tList = tokenize(iss);

			Parser tListParsed(tList);
			tListParsed.parseFile();
			REQUIRE(tListParsed.successfullyParsed() == true);
		}
	
		{// test05.asm
			std::string input = R"(
			# test of the basic control instructions
			.data
			x:  .word 1
			y:   .word 2

			.text
			main:
				lw $t0, x
			    lw $t1, y
				.text
			beq $t0, $t0, next1
		next1:

			bne $t0, $t1, next2
		next2:

			blt $t0, $t1, next3
		next3:

			ble $t0, $t0, next4
		next4:

			bgt $t1, $t0, next5
		next5:

			bge $t0, $t0, next6
		next6:
	
		end:
			j	end)";  // raw string

			std::istringstream iss(input);
			TokenList tList = tokenize(iss);

			Parser tListParsed(tList);
			tListParsed.parseFile();
			REQUIRE(tListParsed.successfullyParsed() == true);
		}
		

		{ // test06.asm
			std::string input = R"(
			# Example program to compute the sum of squares from Jorgensen [2016]

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
			mul $t3, $t1, $t1
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
		}

		{// test07.asm
			std::string input = R"(
			# a test of all register names
	.data

	.text
main:
	sw $1, $0
	sw $at, $zero
	sw $2, $0
	sw $v0, $zero
	sw $3, $0
	sw $v1, $zero
	sw $4, $0
	sw $a0, $zero
	sw $5, $0
	sw $a1, $zero
	sw $6, $0
	sw $a2, $zero
	sw $7, $0
	sw $a3, $zero
	sw $8, $0
	sw $t0, $zero
	sw $9, $0
	sw $t1, $zero
	sw $10, $0
	sw $t2, $zero
	sw $11, $0
	sw $t3, $zero
	sw $12, $0
	sw $t4, $zero
	sw $13, $0
	sw $t5, $zero
	sw $14, $0
	sw $t6, $zero
	sw $15, $0
	sw $t7, $zero
	sw $16, $0
	sw $s0, $zero
	sw $17, $0
	sw $s1, $zero
	sw $18, $0
	sw $s2, $zero
	sw $19, $0
	sw $s3, $zero
	sw $20, $0
	sw $s4, $zero
	sw $21, $0
	sw $s5, $zero
	sw $22, $0
	sw $s6, $zero
	sw $23, $0
	sw $s7, $zero
	sw $24, $0
	sw $t8, $zero
	sw $25, $0
	sw $t9, $zero
	sw $26, $0
	sw $k0, $zero
	sw $27, $0
	sw $k1, $zero
	sw $28, $0
	sw $gp, $zero
	sw $29, $0
	sw $sp, $zero
	sw $30, $0
	sw $fp, $zero
	sw $31, $0
	sw $ra, $zero

end:
	j end)";  // raw string

			std::istringstream iss(input);
			TokenList tList = tokenize(iss);

			Parser tListParsed(tList);
			tListParsed.parseFile();
			REQUIRE(tListParsed.successfullyParsed() == true);
		}
	
		{// test08.asm
			std::string input = R"(
			# a test of address modes
			.data
		str:	.ascii	"hello"

			.text
		main:
			lb $t8, str
			la $t0, str
			lb $t1, ($t0)
			lb $t2, 1($t0)
			lb $t3, 2($t0)
			lb $t4, 3($t0)
			lb $t5, 4($t0)
	
	end:	nop
			j end)";  // raw string

			std::istringstream iss(input);
			TokenList tList = tokenize(iss);

			Parser tListParsed(tList);
			tListParsed.parseFile();
			REQUIRE(tListParsed.successfullyParsed() == true);
		}

}

TEST_CASE("test the fail assembly files", "[parser]")
{
	{// test00.asm
		string input = R"( #ignore line
				# A test file of data declarations only
			.data
		var1:	.word 1024             # int var1 = 1024

		var2:	.half 12               # short var2 = 12
	
		var3:	.byte 0                # char var3 = 0

		var4:	.byte 1, 2, 3, 4, 5, 6, 7,   # PARSE ERROR

		var5:	.space 512             # reserve 512 bytes

		var6:	.ascii "hello"

		var7:	.asciiz "goodbye"

			.text)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 10) == "Error:10: ");
	}
	
	{// test02.asm
		string input = R"( 
				.data
			var:	.word 0
					.word 1
					.word 2

				.text
	
			# lexer error line 10 
			main:	la $t0, var 
					lw $s1, ($t0)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		REQUIRE(tList.size() > 0);
		REQUIRE(tList.back().type() == ERROR);
		REQUIRE(tList.back().value().substr(0, 10) == "Error:11: ");
	}
	

	{// test03.asm
		string input = R"(
				# a test for basic data movement
			.data
		avar:	.word 	0
		bvar:	.half	1
		cvar:	.byte	3

			.text
		main:	
			li $t0, $t1 # Parse Error
			lw $t1, avar
			lh $t2, bvar
			lb $t2, cvar
			sw $t1, avar
			sh $t2, bvar
			sb $t2, cvar

	move $t0, $0)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 10) == "Error:10: ");
	}


	{// test04.asm
		string input = R"(
			# a test of basic arithmetic operations
	.data
x:	.word	1
y:	.word	1
z:	.space 	4

	.text
main:
	lw 	$t0, x
	lw 	$t1, y
	add 	$t2, $t0, $t1
	add 	$t2, $t0, 2
	addu 	$t2, $t0, $t1
	addu 	$t2, $t0, 2
	sub 	$t2, $t0, $t1
	sub 	$t2, $t0, 2
	subu 	$t2, $t0, $t1
	subu 	$t2, $t0, 2
	mul 	$t2, $t0, $t1
	mul 	$t2, $t0, 2
	mulo 	$t2, $t0, $t1
	mulo 	$t2, $t0, 2
	mulou 	$t2, $t0, $t1
	mulou 	$t2, $t0, 2
	mult	$t0, $t1
	multu	$t0, $t1
	div 	$t2, $t0, $t1
	div 	$t2, $t0, 2
	divu 	$t2, $t0, $t1
	divu 	$t2, $t0, 2
	div	$t0, $t1
	divu	$t0, $t1
	rem 	$t2, $t0, $t1
	rem 	$t2, $t0	# parse error
	remu 	$t2, $t0, $t1
	remu 	$t2, $t0, 2
	abs	$t0, $t1
	neg	$t0, $t1
	negu	$t0, $t1)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 10) == "Error:35: ");
	}


	{// test05.asm
		string input = R"(
			# test of basic logical instructions
				.data
			TRUE = 1
			FALSE = 0

			test1:	.word TRUE
			test2:	.word FALSE
	
				.text
			main:
				lw	$t0, test1
				lw	$t1, test2
	
				and	$t2, $t0, $t1
				and	$t2, $t0, TRUE
				nor	$t2, $t0, $t1
				nor	$t2, $t0, TRUE, FALSE # parse error
				not	$t2, $t0
				not	$t2, $t0
				or	$t2, $t0, $t1
				or	$t2, $t0, TRUE
				xor	$t2, $t0, $t1
				xor	$t2, $t0, TRUE)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 10) == "Error:18: ");
	}

	{// test06.asm
		string input = R"(
			# test of the basic control instructions
				.data
			x:	.word 1
			y:	.word 2

				.text
			main:
				lw $t0, x
				lw $t1, y
	
				beq $t0, $t0, next1
			next1:

				bne $t0, $t1, next2
			next2:

				blt $t0, $t1, next3
			next3:

				ble $t0, $t0, next4
			next4:

				bgt $t1, next5      # parse error
			next5:

				bge $t0, $t0, next6
			next6:
	
			end:
				j end)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 10) == "Error:24: "); 
	}

	{
		{// test07.asm
			string input = R"(
			# Example program to compute the sum of squares from Jorgensen [2016]

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
				mul $t3, $t1, $t1
				add $t2, $t2, $32	# parse error
				add $t1, $t1, 1
				ble $t1, $t0, sumLoop
				sw  $t2, sumOfSquares

			end:
				j end)";

			std::istringstream iss(input);
			TokenList tList = tokenize(iss);

			Parser tListParsed(tList);
			tListParsed.parseFile();
			REQUIRE(tListParsed.successfullyParsed() == false);
			REQUIRE(tListParsed.errorMsg().substr(0, 10) == "Error:21: ");
		}
	}

}



//ADDITIONAL TESTS NEEDED FOR 95% TEST COVERAGE. MOSTLY FOR FAILED TEST CASES
//------------------------------------------------------------------------------------------------------------------------------------------//

TEST_CASE("Additional tests for data declaration mode", "[parser]") {

	//*********************************************************************************** These tests should pass

	{
		string input = R"( #ignore this line
				.data 
			label:
				.data
			constant = 3
				.data
			voov:	.word 23321)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == true);
		REQUIRE(tListParsed.errorMsg() == "");
	}

	{ 
		string input = R"( #ignore this line
				.data
			CONSTANT = 4
			CONSTANT = 10)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == true);
	}
	
	{
		string input = R"( #ignore this line
				.data
			var1:  .word 3000
			var2:  .half -333
			var3:  .byte  +127
			var4:  .space  +100000)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == true);
	}
	
	{
		string input = R"( #ignore this line
	.data
	.ascii  "hello"
	.asciiz  "#boi^" )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == true);
	}

	{
		string input = R"( #ignore this line
				.data
			var:

			var2:  .ascii "Ban" )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == true);
	}
	
	//*********************************************************************************** These tests should fail

	{
		string input = R"(	#ignore this line
				.data
				=)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"(	#ignore this line
				.data
			v$ho:	.byte 8)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"(	#ignore this line
				.data
			var: =)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			CA%T = 33)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.byte   +)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.byte  5=)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.half   -67$3\n)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}
	
	{
		string input = R"(.data   hola\n)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:1: ");
	}

	{
		string input = R"(.text   hola\n)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:1: ");
	}

	
	{
		string input = R"( #ignore this line
				.data
			$%$)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			CONSTANT equal 6)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			CONSTANT , 6)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			CONSTANT = ,)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			CONSTANT = 6 =)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.word 6 bleed )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.ascii =)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.asciiz "ye)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.asciiz "ye,)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.asciiz "ye" =)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.asciiz "ye" bleed)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}
	
	
	{
		string input = R"( #ignore this line
				.data
			x: .byte 2
			y: .byte 8
			x:  .half 72)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:5: ");
	}
	
	{
		string input = R"( #ignore this line
				.data
			var:  
			var: )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:4: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.byte 2,5,=)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			.byte +250)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			var: .half -50000)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			var: .half =)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			var: .byte -400)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}
	
	{
		string input = R"( #ignore this line
				.data
			var: .space 21477283650)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.data
			var: .word -21477283650)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}
	
	{
		string input = R"( #ignore this line
				.data
			CONSTANT = +250
			.byte    CONSTANT)"; // note: THIS TEST HERE WILL NOT HAVE THE PARSER ALLOCATE MEMORY BECAUSE .byte CONSTANT is not required

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:4: ");
	}
	


	
}


TEST_CASE("Additional tests for text mode", "[parser]")
{
	//*********************************************************************************** These tests should pass

	{
		string input = R"( #ignore this line
				.text
			label:
				.text
				li $t0, 45
				mfhi $t2
				.text
				 nop
				 .text
				label2: nop)"; // MY PARSER ASSUMES THAT THE LABEL NAMES ARE UNIQUE

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile(); 

		REQUIRE(tListParsed.successfullyParsed() == true);
	}

	{
		string input = R"(	#ignore this line
				.text
				label:  li  $t0,6
				label2: beq $t0, $t0, next1
				nop
				label3: j label)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == true);
	}
	
	{
		string input = R"( #ignore this line
				.data
			CONSTANT = 7
			var:  .word 6
				.text
				 lw $t1, 7
				 lw $t0, CONSTANT
				 lw $t2, (3)
				 lw $t3, (CONSTANT)
				 lw $t4, (var))";
				  
		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();
		REQUIRE(tListParsed.successfullyParsed() == true);
	}
	

	//*********************************************************************************** These tests should fail
	
	{
		string input = R"(	#ignore this line
				.text
				label: nop stuff)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"(	#ignore this line
				.text
				 li bleed, 45 )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"(	#ignore this line
				.text
				 li $t0 bleed)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 li $t0 )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: "); 
	}

	{
		string input = R"( #ignore this line
				.text
				 mult $t0, $t3, )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 sub $t0, $t3 )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 li $t0, 45 $t3 )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}
	
	{
		string input = R"( #ignore this line
				.text
				 beq $t0, 6)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 add $t0, $t3, 3, =)"; // CHECK THIS OUT BECAUE IT IS NOT GIVING AN ERROR IN update_intLayout3 funciton 

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 mult $t0, $t3, 3)"; 

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 lw $t2 $t3)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 lw $t2,(2) bleed)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 lw $t2,($t3) =)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 lw $t2, 1[$t2))";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
			main: 
			= )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:4: ");
	}

	{
		string input = R"( #ignore this line
				.text
				.text GOKU)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				lw  $t0,$t1 =)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 lw $t2, 1(Blehh) )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 lw $t2, 1($t3) =)";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}

	{
		string input = R"( #ignore this line
				.text
				 lw $t2, 1,Blehh) )";

		std::istringstream iss(input);
		TokenList tList = tokenize(iss);

		Parser tListParsed(tList);
		tListParsed.parseFile();

		REQUIRE(tListParsed.successfullyParsed() == false);
		REQUIRE(tListParsed.errorMsg().substr(0, 9) == "Error:3: ");
	}


}



