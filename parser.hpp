// define the parser module here. This parser is specific for MIPS Assembly Architecture
#ifndef PARSER_HPP
#define PARSER_HPP

#include <istream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <bitset>
#include "token.hpp"
#include "lexer.hpp"

using namespace std;

struct LabelInfo {  
	bool isInstructionIndex; // The "variant type" from the project discription will be either the firstMemAddress of the label or the instructionIndex(the index from the program instructions vector)
	size_t instructionIndexOrFirstMemAddress; // If It is an instruction index then this value represents exactly that. If it is false then this value represents the first memory address for the label
};

				// EOT = end of tokens
enum inputValue { EOT, EOL_IN, SEP_IN, EQUAL_IN, OPENP_IN, CLOSEP_IN, STRING_DELIM_IN, DECLARATION_MODE_IN,
				  INT_LAYOUT_IN, STRING_LAYOUT_IN, INTEGER_IN, CHARS_IN, CONSTANT_FIRST_PART_IN, INITALIZED_CONSTANT_IN, LABEL_IN, ERROR_IN,
				 /*Instruction specfic inputValues*/ TEXT_MODE_IN, NOP_IN, OPCODE_IN, REGISTER_IN};

struct argument {
	string inString; // This is the string that contains the register, immediate, constant etc.
	inputValue inValue; // Has the type of value. So either INTEGER_IN, CONSTANT, etc.
};

struct instructionType {
	string opcode;
	vector<argument> instructionArgs;
};

class Parser 
{
private:
	//Initial state is the default state and will stay until it hits either a .data or .word 
	enum States{INITIAL_STATE, DECLARATION_MODE_STATE, LABEL_STATE_S0, 
				 CONSTANT_STATE_S0, CONSTANT_STATE_S1, CONSTANT_STATE_S2, LAYOUT_STATE_INT_S0, LAYOUT_STATE_INT_S1,
				 SEP_STATE, LAYOUT_STATE_STR_S0, STRING_DELIM_STATE1, STRING_DELIM_STATE2, LAYOUT_STATE_STR_S2,
				 EOL_STATE, ERROR_STATE, /*Instruction specfic states*/
				 TEXT_MODE_STATE, text_EOL_STATE, NOP_STATE, OPCODE_STATE, ARG1_STATE, ARG2_REG_STATE, ARG2_IMMEDIATE_STATE,
				 ARG_SOURCE_STATE, ARG2_MEMREF_STATE, OPENP_STATE, CLOSEP_STATE, OFFSET_STATE} systemState;

	TokenList assemblyTokenList; // This is the list of tokens that were generated from the lexer 
	bool correctGrammer; // This is a boolean variable that shows if there was an error parsing the assembly file
	
	unsigned int lineOfError; // keeps track of the current tokens line number to see if there was an error on that tokens line
	string errorMessage; // wherever I get an error I can just add the, it->line , to my errorMessage variable

	string constantName; // This is for saving the name of the constant
	unordered_map<string, string> constantsMap; // This is a map to store the constants and its corresponding integer values as strings
	
	string layoutKey; // This will store the value of either: ".word", ".half", ".byte", ".space", ".ascii", or "asciiz"
	string mode; // This variable is to keep track of which mode I am in, more importantly which state machine I am in(.text or .data)
	string opcode; // Saves the opcode name to be used later throughout the program
	vector<argument> opcodeArguments; // This is a vector of opcode arguments and everytime I get a new argument I push that string argument and its inputValue in this vector 

	string labelName; // Saves the labels name
	unordered_map<string, LabelInfo> labelsMap; // This is a map which uses the label as the key and stores its value(s). Ex. aye: .byte 1,2,3,4,5 #aye = {1,2,3,4,5}

	unordered_map<string, int32_t> registersMap; // This map contains all the available registers that can be used

												  //Milestone2 specific Uses:
	size_t lastAvailableAddress = 0x00000000; // Used for memory storage. 
	vector<string> memoryLayout; // After the parser is finished this variable will contain the initial memory. The initial memory will contain hex values of the bytes but in string format
	string determineHexValue(size_t integer); // When I send a 4 bit integer(1-15) this function will determine its hex value
	void helpIntMemAllocation(string inputString, const string layoutKey); // Helper function for each case of the integer memory allocation
	void asciiMemAllocation(string inputString); // Does both .ascii and .asciiz memory allocation
	void intLayoutMemAllocation(size_t start, string inputString, const string layoutKey); // Does .word, .half, and .byte memory allocation
	vector<instructionType> programInstructions; // Container for all the .text program instructions
	void incrementVectorOfProgramInstructions(const string opcode, const vector<argument> opcodeArgs);
	

									//** Helper functions **//
	inputValue classify(TokenList::const_iterator it, TokenList::const_iterator end, string& inputString); 
	bool fitsBits(const string layoutKey, const string inputInteger, const inputValue input); // function for bounds checking, if an integer starts with '+' or '-' then it should be treated as a signed type, otherwise it should be treated as unsigned
	void intLayout_IntOrConstantCheck(States& systemState, inputValue input, string inputString);
	void stringTokenDeclarationClassify(string& inputString, char firstChar, char lastChar, inputValue& input);
	void checkandUpdateDataLabelsMap(States& systemState); // This function is purely for checking labels in the .data section
	void checkIfLabel(States& systemState, inputValue& input, string inputString);
	void checkIfConstant(States& systemState, inputValue& input, string inputString);
	void checkIfInteger(States& systemState, inputValue& input, string inputString, char firstChar);
	bool isOpcode(string inputString);
	bool isRegister(string inputString, unordered_map<string, int32_t> registersMap); 
	void initializeRegisters(); // Sets all the possible registers in the variable registersMap, that can be used(based on the table of milestone 1 discription) and value is 0(For now might need to change based on MILESTONE 2)
	void incrementVectorOfOpcodeArguments(const string inputString, const inputValue input);


									//** Update systemState functions **//
	void update_mode(States& systemState, inputValue input, TokenList::const_iterator it); 
	void update_declaration(States& systemState, inputValue input); 
	void update_eol(States& systemState, inputValue input); // in addition here if the input=LABEL_IN :I update the <label> and I basically say LABEL_data_STATE = DECLARATION_MODE_STATE
	void update_constant1(States& systemState, inputValue input);
	void update_constant2(States& systemState, inputValue input);
	void update_constant3(States& systemState, inputValue input, string inputString);
	void update_intLayout1(States& systemState, inputValue input, string inputString);
	void update_intLayout2(States& systemState, inputValue input);
	void update_sep(States& systemState, inputValue input, string inputString);
	void update_strLayout1(States& systemState, inputValue input);
	void update_strLayout2(States& systemState, inputValue input);
	void update_strLayout3(States& systemState, inputValue input);
	void update_strLayout4(States& systemState, inputValue input, string inputString);
	
	//***** .text mode specific functions below
	void update_instruction(States& systemState, inputValue input);
	void update_text_label(States& systemState, inputValue input);
	void update_text_eol(States& systemState, inputValue input);
	void update_nop(States& systemState, inputValue input);
	void update_opcode(States& systemState, inputValue input, string inputString);
	void update_arg1(States& systemState, inputValue input);
	void update_arg2Reg(States& systemState, inputValue input);
	void update_arg2Immediate(States& systemState, inputValue input);
	void update_arg2and3Source(States& systemState, inputValue input);
	void update_arg2Memref(States& systemState, inputValue input);
	void update_openP(States& systemState, inputValue input, string inputString);
	void update_closeP(States& systemState, inputValue input);
	void update_offsetOrImmediate(States& systemState, inputValue input);

public:
	Parser(const TokenList& tokenList); // constructor that recieves a token list and sets the list equal to the private variable assemblyTokens. note: can not parse without something to parse(tokenList)
	~Parser(); // Deconstructor

	void parseFile(); // A function to return true or false based on if the assembly file has correct grammer. Also within this function store information in a data structure

	string errorMsg(); // If there is a parsing error, I will take the line number from the token, and display a specific error message. Use variables lineNumberError and errorMessage

	bool successfullyParsed(); // This returns a boolean based on the assembly file parsed

	unordered_map<string, int32_t> getInitialRegisterContents()
	{
		return registersMap; // Allows the virtual machine to gain access to the intial contents in each register(will be initialized to 0)
	}

	vector<string> getInitialMemoryLayout()
	{
		return memoryLayout; // Allows the virtual machine to gain access to the memory layout created in the .data section and in the VM will allocate more memory until I reach 512 bytes
	}

	vector<instructionType> getProgramInstructions()
	{
		return programInstructions; // Allows the virtual machine to gain access to the program instructions that were in the .text section of the file. Simulator program couter will go off this vector
	}

	unordered_map<string, LabelInfo> getLabelsMap() 
	{
		return labelsMap; // Allows the virtual machine to gain access to the labels map that contains if a label is mapped to a memorylayout(.data) or if it is mapped to an instruction index(.text) from the programInstructions vector
	}

	string determineRegister(string inputStringRegister); // Given an input string(register like $t0) will determine what register is being used. Need this Public function because there are aliases for registers as well in the simulator.

};

#endif // !PARSER_HPP

