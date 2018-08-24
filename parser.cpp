// implement the parser here
#include "parser.hpp"
#include <assert.h>
#include <limits>// This is for the function bool fitsBits(); to check the boundaries of integers


Parser::Parser(const TokenList& tokenList) // assigns the tokenList from the lexer to the private token list of the class
{
	assemblyTokenList = tokenList;
	systemState = INITIAL_STATE;
}

Parser::~Parser()
{
	opcodeArguments.clear();
	constantsMap.clear();
}

void Parser::parseFile()
{
	TokenList::iterator it = assemblyTokenList.begin(); // could have done |auto| instead of |TokenList::iterator|
	inputValue currentInput; // Classifying the input type of the token
	string inputString; // This is the current string assigned by the current token
	correctGrammer = false; // variable used to indicate if the file correctly was parsed
	initializeRegisters();

	do {
		currentInput = classify(it, assemblyTokenList.end(), inputString);
		if ((currentInput != ERROR_IN) && (currentInput != EOT)) // also systemState != ERROR_STATE 
			++it; // increment iterator to next element if there was no errors

		switch (systemState) {
		case INITIAL_STATE:
			update_mode(systemState, currentInput, it);
			break;
			
		case DECLARATION_MODE_STATE:
			update_declaration(systemState, currentInput);
			break;

		case TEXT_MODE_STATE:
			update_instruction(systemState, currentInput);
			break;

		case NOP_STATE:
			update_nop(systemState, currentInput);
			break;

		case LABEL_STATE_S0:
			update_text_label(systemState, currentInput);
			break;

		case CONSTANT_STATE_S0:
			update_constant1(systemState, currentInput);
			break;

		case CONSTANT_STATE_S1:
			update_constant2(systemState, currentInput);
			break;

		case CONSTANT_STATE_S2:
			update_constant3(systemState, currentInput, inputString);
			break;

		case LAYOUT_STATE_INT_S0:
			update_intLayout1(systemState, currentInput, inputString);
			break;

		case LAYOUT_STATE_INT_S1:
			update_intLayout2(systemState, currentInput);
			break;

		case SEP_STATE: // == LAYOUT_STATE_INT_S2 but in the text mode section and SEP_State for the .text section
			update_sep(systemState, currentInput, inputString);
			break;

		case LAYOUT_STATE_STR_S0:
			update_strLayout1(systemState, currentInput);
			break;

		case STRING_DELIM_STATE1:  // == LAYOUT_STATE_STR_S1
			update_strLayout2(systemState, currentInput);
			break;

		case LAYOUT_STATE_STR_S2: 
			update_strLayout3(systemState, currentInput); 
			break;

		case STRING_DELIM_STATE2: // == LAYOUT_STATE_STR_S3
			update_strLayout4(systemState, currentInput, inputString);
			break;

		case EOL_STATE:
			update_eol(systemState, currentInput);
			break;

		case text_EOL_STATE:
			update_text_eol(systemState, currentInput);
			break;

		case OPCODE_STATE:
			update_opcode(systemState, currentInput, inputString);
			break;

		case ARG1_STATE:
			update_arg1(systemState, currentInput);
			break;

		case ARG2_REG_STATE:
			update_arg2Reg(systemState, currentInput);
			break;

		case ARG2_IMMEDIATE_STATE:
			update_arg2Immediate(systemState, currentInput);
			break;

		case ARG_SOURCE_STATE:
			update_arg2and3Source(systemState, currentInput);
			break;

		case ARG2_MEMREF_STATE:
			update_arg2Memref(systemState, currentInput);
			break;

		case OPENP_STATE:
			update_openP(systemState, currentInput, inputString);
			break;

		case CLOSEP_STATE:
			update_closeP(systemState, currentInput);
			break;

		case OFFSET_STATE: // THis state could be called OFFSET_AND_IMMEDIATE_STATE but naming convention to long
			update_offsetOrImmediate(systemState, currentInput);
			break;

		case ERROR_STATE: // Dummy State 
			break;
		};

	} while ((systemState != ERROR_STATE) && (it != assemblyTokenList.end()) ); // Will continue parsing until I have hit an error or until I have hit the end of the token list

	if (assemblyTokenList.empty())
		errorMessage = "Error:0: Attempted to parse an empty file";

	if(systemState != ERROR_STATE)
		correctGrammer = true;
}

bool Parser::successfullyParsed()
{
	return correctGrammer;
}

string Parser::errorMsg()
{
	if (!correctGrammer)
		return errorMessage;
	else
		return "";
}

string Parser::determineHexValue(size_t integer)
{
	if (integer == 0)
		return "0";
	else if (integer == 1)
		return "1";
	else if (integer == 2)
		return "2";
	else if (integer == 3)
		return "3";
	else if (integer == 4)
		return "4";
	else if (integer == 5)
		return "5";
	else if (integer == 6)
		return "6";
	else if (integer == 7)
		return "7";
	else if (integer == 8)
		return "8";
	else if (integer == 9)
		return "9";
	else if (integer == 10)
		return "a";
	else if (integer == 11)
		return "b";
	else if (integer == 12)
		return "c";
	else if (integer == 13)
		return "d";
	else if (integer == 14)
		return "e";
	else if (integer == 15)
		return "f";
	else
		return "ERROR"; // Will not get to this because in my asciiMemAllocation() and intLayoutMemAllocation() the max integer value that can be sent is 15
}

void Parser::helpIntMemAllocation(string inputString, const string layoutKey)
{
	if (layoutKey == ".word") {
		intLayoutMemAllocation(0, inputString, layoutKey);
		intLayoutMemAllocation(8, inputString, layoutKey);
		intLayoutMemAllocation(16, inputString, layoutKey);
		intLayoutMemAllocation(24, inputString, layoutKey);
	}
	else if (layoutKey == ".space") {
		int32_t sizeToBeAllocated = stol(inputString);
		for (int32_t i = 0; i < sizeToBeAllocated; i++) { // the counter will be AN int32_t to match the sizeToBeAllocated variable
			memoryLayout.push_back("00");
			lastAvailableAddress++;
		}
	}
	else if (layoutKey == ".half") {
		intLayoutMemAllocation(0, inputString, layoutKey);
		intLayoutMemAllocation(8, inputString, layoutKey);
	}
	else if (layoutKey == ".byte")
	{
		intLayoutMemAllocation(0, inputString, layoutKey);
	}
}

void Parser::asciiMemAllocation(string inputString)
{
	for (size_t i = 0; i < inputString.size(); i++) { // For each character store the proper hex string value
		bitset<8> binIn((uint8_t)inputString[i]); // Converts the character into its decimal value then converts it to binary
		size_t hexValueRight = 0, hexValueLeft = 0;
		if (binIn[0])
			hexValueRight += 1;
		if (binIn[1])
			hexValueRight += 2;
		if (binIn[2])
			hexValueRight += 4;
		if (binIn[3])
			hexValueRight += 8;
		if (binIn[4])
			hexValueLeft += 1;
		if (binIn[5])
			hexValueLeft += 2;
		if (binIn[6])
			hexValueLeft += 4;
		if (binIn[7])
			hexValueLeft += 8;

		memoryLayout.push_back(determineHexValue(hexValueLeft).append(determineHexValue(hexValueRight)) ); // ex. "7" + "f" = "7f" 
		lastAvailableAddress++;
	}
}

void Parser::intLayoutMemAllocation(size_t start, string inputString, const string layoutKey)
{
	if (layoutKey == ".word") {
		bitset<32> bitSet ((int32_t)stol(inputString));
		size_t hexLeftInt = 0, hexRightInt = 0;
		if (bitSet[start])
			hexRightInt += 1;
		if (bitSet[start + 1])
			hexRightInt += 2;
		if (bitSet[start + 2])
			hexRightInt += 4;
		if (bitSet[start + 3])
			hexRightInt += 8;
		if (bitSet[start + 4])
			hexLeftInt += 1;
		if (bitSet[start + 5])
			hexLeftInt += 2;
		if (bitSet[start + 6])
			hexLeftInt += 4;
		if (bitSet[start + 7])
			hexLeftInt += 8;

		memoryLayout.push_back(determineHexValue(hexLeftInt).append(determineHexValue(hexRightInt)) ); // ex. "7" + "f" = "7f" 
		lastAvailableAddress++;
	}
	else if (layoutKey == ".half") {
		bitset<16> bitSet ((int16_t)stol(inputString));
		size_t hexLeftInt = 0, hexRightInt = 0;
		if (bitSet[start])
			hexRightInt += 1;
		if (bitSet[start + 1])
			hexRightInt += 2;
		if (bitSet[start + 2])
			hexRightInt += 4;
		if (bitSet[start + 3])
			hexRightInt += 8;
		if (bitSet[start + 4])
			hexLeftInt += 1;
		if (bitSet[start + 5])
			hexLeftInt += 2;
		if (bitSet[start + 6])
			hexLeftInt += 4;
		if (bitSet[start + 7])
			hexLeftInt += 8;

		memoryLayout.push_back(determineHexValue(hexLeftInt).append(determineHexValue(hexRightInt)) ); // ex. "7" + "f" = "7f" 
		lastAvailableAddress++;
	}
	else if (layoutKey == ".byte") {
		bitset<8> bitSet ((int8_t)stol(inputString));
		size_t hexLeftInt = 0, hexRightInt = 0;
		if (bitSet[start])
			hexRightInt += 1;
		if (bitSet[start + 1])
			hexRightInt += 2;
		if (bitSet[start + 2])
			hexRightInt += 4;
		if (bitSet[start + 3])
			hexRightInt += 8;
		if (bitSet[start + 4])
			hexLeftInt += 1;
		if (bitSet[start + 5])
			hexLeftInt += 2;
		if (bitSet[start + 6])
			hexLeftInt += 4;
		if (bitSet[start + 7])
			hexLeftInt += 8;

		memoryLayout.push_back(determineHexValue(hexLeftInt).append(determineHexValue(hexRightInt)) ); // ex. "7" + "f" = "7f" 
		lastAvailableAddress++;
	}

}

void Parser::incrementVectorOfProgramInstructions(const string opcode, const vector<argument> opcodeArgs)
{
	if (!opcodeArgs.empty()) {
		instructionType curInstructions;
		curInstructions.instructionArgs = opcodeArgs;
		curInstructions.opcode = opcode;
		programInstructions.push_back(curInstructions);
	}
}

inputValue Parser::classify(TokenList::const_iterator it, TokenList::const_iterator end, string& inputString)
{
	inputValue input = EOT;
	if(it != end)
		lineOfError = it->line(); // assigns the line number of token that the iterator is pointing at

	if (it == end) {
		input = EOT;
	}
	else if (it->type() == EOL) {
		input = EOL_IN;
	}
	else if (it->type() == SEP) {
		input = SEP_IN;
	}
	else if (it->type() == EQUAL) {
		input = EQUAL_IN;
	}
	else if (it->type() == OPEN_PAREN) {
		input = OPENP_IN;
	}
	else if (it->type() == CLOSE_PAREN) {
		input = CLOSEP_IN;
	}
	else if (it->type() == STRING_DELIM) {
		input = STRING_DELIM_IN;
	}
	else if ((it->type() == STRING) && (it->value() == ".data")) {
		input = DECLARATION_MODE_IN;
	}
	else if ((it->type() == STRING) && (it->value() == ".text")) {
		input = TEXT_MODE_IN;
	}
	else if(it->type() == STRING) { // This string will at least have a length of 1. Also this is the default case when the value in the STRING token is none of the above
		inputString = it->value();
		char lastChar = inputString[inputString.length() - 1];
		char firstChar = inputString[0];

		if(mode == ".data")
			stringTokenDeclarationClassify(inputString, firstChar, lastChar, input); // inputString: to be able to change this value and use it in the program, input: will be the enum inputValue input
		else if (mode == ".text") {												     
			
			if ((inputString.length() != 1) && (lastChar == ':') && (isalpha(firstChar) || (firstChar == '_')) ) { // if STRING is a <Label>
				
				checkIfLabel(systemState, input, inputString); // Checks if the string is a label and If it is then sets the input = LABEL_IN
			}
			else if (inputString == "nop") {
				opcode = inputString;
				input = NOP_IN;
			}
			else if (isOpcode(inputString)) {
				opcode = inputString; // assigns the current verfied opcode to a private variable that will be used later
				input = OPCODE_IN;
			}
			else if (isRegister(inputString, registersMap) ) {  
				input = REGISTER_IN;
			}
			else if ((isalpha(firstChar) || (firstChar == '_')) ) { // POTENTIAL to be a new <constant> OR could be a constant already declared
				
				checkIfConstant(systemState, input, inputString);  // Checks to see if the string has potential to be a valid constant and if it is then sets the input = INITALIZED_CONSTANT_IN
			}
			else if ((firstChar == '-') || (firstChar == '+') || (isdigit(firstChar)) ) { // if STRING is an <integer>) 

				checkIfInteger(systemState, input, inputString, firstChar); // Checks to see if the string is an integer and if it is then sets the input = INTEGER_IN
			}
			else {
				systemState = ERROR_STATE;
				input = ERROR_IN;
				errorMessage = "Error:" + to_string(lineOfError) + ": Not a valid Instruction. Could not be defined in the text declaration MIPS assembly.";
				//UNQUIE ERROR MESSAGE: The STRING TOKEN could not be defined in the text instruction MIPS assembly.
			}

		}
	}

	return input;
}



void Parser::update_mode(States& systemState, inputValue input, TokenList::const_iterator it)
{
	//assert((systemState = INITIAL_STATE) || (systemState = EOL_STATE));

	switch (input) {
	case DECLARATION_MODE_IN:
		// For both the declaration mode and text mode an EOL token must follow to be considered correct grammer
		if (it->type() == EOL){
			systemState = DECLARATION_MODE_STATE;
			//mode = ".data";
		}
		else {
			systemState = ERROR_STATE;
			errorMessage = "Error:" + to_string(lineOfError) + ": The Data declaration section must be on its own line.";
		}
		break;

	case TEXT_MODE_IN:
		if (it->type() == EOL) {
			systemState = TEXT_MODE_STATE;
			//mode = ".text";
		}
		else {
			systemState = ERROR_STATE;
			errorMessage = "Error:" + to_string(lineOfError) + ": The Text declaration section must be on its own line.";
		}
		break;
	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": There has not been a data nor text declaration in the file.";
		break;
	};

	
}

void Parser::update_declaration(States& systemState, inputValue input)
{
	assert(systemState = DECLARATION_MODE_STATE);
	mode = ".data";

	switch (input) {
	case INT_LAYOUT_IN:
		systemState = LAYOUT_STATE_INT_S0;
		break;

	case STRING_LAYOUT_IN:
		systemState = LAYOUT_STATE_STR_S0;
		break;

	case EOL_IN:
		systemState = EOL_STATE;
		break;

	//case STRING_DELIM_IN: // This is needed when for example. .ascii "bleh" is on its own line because the .ascii will be the indication of being in the DECLARATION_MODE_STATE
		//systemState = STRING_DELIM_STATE1;
		//break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": Not a valid follow up argument after a declaration."; //Means the update declaration state cases are looking at the argument after <constant>,<label>, or <layout>
		break;
	}

}

void Parser::update_instruction(States & systemState, inputValue input) 
{
	assert(systemState = TEXT_MODE_STATE);
	mode = ".text";
	
	switch (input) {
	case EOL_IN: 
		systemState = text_EOL_STATE;
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": Just entered into the instruction section and initial instruction is Not valid.";
		break;
	};
}

void Parser::update_text_eol(States & systemState, inputValue input)
{
	assert(systemState == text_EOL_STATE);
	opcodeArguments.clear(); // clears the arguments for the next line of instructions/operations

	switch (input) {
	case OPCODE_IN:
		systemState = OPCODE_STATE;
		break;

	case NOP_IN:
		systemState = NOP_STATE;
		break;

	case LABEL_IN:
		systemState = LABEL_STATE_S0;
		break;

	case DECLARATION_MODE_IN:
		systemState = DECLARATION_MODE_STATE; // This case is for when the file looks like: ".text\n.data\n"
		break;

	case TEXT_MODE_IN:
		systemState = TEXT_MODE_STATE;
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": Not a valid instruction.";
		break;
	};

}

void Parser::update_nop(States & systemState, inputValue input) 
{
	assert(systemState == NOP_STATE);
	
	switch (input) {
	case EOL_IN:
		incrementVectorOfOpcodeArguments("nop", input);
		incrementVectorOfProgramInstructions(opcode, opcodeArguments);
		systemState = text_EOL_STATE;
		break;

	default: 
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": There Can not be more assembly text on the line where a 'nop' has been declared.";
		break;
	};
}

void Parser::update_opcode(States & systemState, inputValue input, string inputString)
{
	assert(systemState == OPCODE_STATE);
	//auto labelFind = labelsMap.find(inputString);

	if (/*(labelFind != labelsMap.end())*/ input == CONSTANT_FIRST_PART_IN && opcode == "j") { // This is because the instruction jump needs a label to jump too. I check in the simulator if that label is actually in the file or not
		incrementVectorOfOpcodeArguments(inputString, input);
		systemState = LABEL_STATE_S0;
	}
	else if (input == REGISTER_IN) {
		systemState = ARG1_STATE;
		incrementVectorOfOpcodeArguments(inputString, input); // This pushes the registers name and its inputValue(register_in) in my vector which will keep track of all the arguments on the current line
	}
	else {
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": There needs to be a register following the Opcode or if it is the special case of 'j' then the label must be predefined.";
	}

}

void Parser::update_arg1(States & systemState, inputValue input)
{
	assert(systemState == ARG1_STATE);

	switch (input) {
	case SEP_IN:
		systemState = SEP_STATE;
		break;
	case EOL_IN:
		if ((opcode == "mfhi") || (opcode == "mflo") || (opcode == "mthi") || (opcode == "mtlo")) {
			incrementVectorOfProgramInstructions(opcode, opcodeArguments);
			systemState = text_EOL_STATE;
		}
		else {
			systemState = ERROR_STATE; // B/C maybe(lineOfError-1): the error occured on the EOL input indicates that the error happened on the previous line
			errorMessage = "Error:" + to_string(lineOfError) + ": Input token is an EOL, but format was not one of the allowable opcodes: mfhi, mflo, mthi, mtlo";
		}
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": There is not a comma or new line following the register(first argument)";
		break;
	}
}

void Parser::update_arg2Reg(States & systemState, inputValue input)
{
	assert(systemState == ARG2_REG_STATE);

	switch (input) {
	case SEP_IN:
		if ((opcode == "add") || (opcode == "addu") || (opcode == "sub") || (opcode == "subu") || (opcode == "mul") || (opcode == "mulo") || 
			(opcode == "mulou") || (opcode == "div") || (opcode == "divu") || (opcode == "rem") || (opcode == "remu") || (opcode == "and") ||
			(opcode == "nor") || (opcode == "or") || (opcode == "xor") ) {

			systemState = SEP_STATE;
		}
		else {
			systemState = ERROR_STATE;
			errorMessage = "Error:" + to_string(lineOfError) + ": Input token is a comma, but format was not one of the allowable opcodes: add,addu,sub,subu,mul,mulo,mulou,div,divu,rem,remu,and,nor,or, xor";
		}
		break;
	case EOL_IN:
		if ((opcode == "move") || (opcode == "mult") || (opcode == "multu") || (opcode == "div") || (opcode == "divu") || (opcode == "abs") || 
			(opcode == "neg")  || (opcode == "negu")) {

			incrementVectorOfProgramInstructions(opcode, opcodeArguments);
			systemState = text_EOL_STATE;
		}
		else {
			systemState = ERROR_STATE; // B/C (maybe linOfError -1)the error occured on the EOL input indicates that the error happened on the previous line
			errorMessage = "Error:" + to_string(lineOfError) + ": Input token is EOL, but format was not one of the allowable opcodes: move, mult, multu, div, divu, abs, neg, negu";
		}
		break;

	default:
		systemState = ERROR_STATE; // B/C the argument error appeared when the state was on a previous line the input is on a new one, showing an incomplete instruction operation on the previous line
		errorMessage = "Error:" + to_string(lineOfError - 1) + ": There is Not a comma or new line following the register(second argument)";
		break;
	}
}

void Parser::update_arg2Immediate(States& systemState, inputValue input)
{
	assert(systemState == ARG2_IMMEDIATE_STATE);

	switch (input) {
	case EOL_IN:
		// here transfer the information into programInstructions
		incrementVectorOfProgramInstructions(opcode, opcodeArguments); 
		systemState = text_EOL_STATE;
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": There is not a new line following the <immediate> format.";
		break;
	}
}

void Parser::update_arg2and3Source(States & systemState, inputValue input)
{
	assert(systemState == ARG_SOURCE_STATE);
	
		switch (input) {
		case SEP_IN:
			if ((opcode == "beq") || (opcode == "bne") || (opcode == "blt") || (opcode == "ble") || (opcode == "bgt") || (opcode == "bge")) {
				systemState = SEP_STATE;
			}
			else {
				systemState = ERROR_STATE;
				errorMessage = "Error:" + to_string(lineOfError) + ": Input token is a Comma, but format was not one of the allowable opcodes: beq, bne, blt, ble, bgt, bge.";
			}
			break;
		case EOL_IN:
			if ((opcode == "not") || (opcode == "add") || (opcode == "addu") || (opcode == "sub") || (opcode == "subu") || (opcode == "mul") || (opcode == "mulo") ||
				(opcode == "mulou") || (opcode == "div") || (opcode == "divu") || (opcode == "rem") || (opcode == "remu") || (opcode == "and") ||
				(opcode == "nor") || (opcode == "not") || (opcode == "or") || (opcode == "xor")  ) {

				incrementVectorOfProgramInstructions(opcode, opcodeArguments);
				systemState = text_EOL_STATE;
			}
			else {
				systemState = ERROR_STATE; // B/C the error occured on the EOL input indicates that the error happened on the previous line
				errorMessage = "Error:" + to_string(lineOfError) + ": Input token is an EOL, but format was not one of the allowable opcodes: add,addu,sub,subu,mul,mulo,mulou,div,divu,rem,remu,and,nor,not,or, xor.";
			}
			break;

		default:
			systemState = ERROR_STATE; // B/C the argument error appeared when the state was on a previous line the input is on a new one, showing an incomplete instruction operation on the previous line
			errorMessage = "Error:" + to_string(lineOfError - 1) + ": There is Not a new line nor a comma following the format <source>: register, integer, or Constant.";
			break;
		}

}

void Parser::update_arg2Memref(States & systemState, inputValue input)
{
	assert(systemState == ARG2_MEMREF_STATE);

	switch (input) {
	case EOL_IN: // This is for when there is no offset in the <memref> , also we will not have a parenthesis error because lexer checks for that already
		incrementVectorOfProgramInstructions(opcode, opcodeArguments);
		systemState = text_EOL_STATE;
		break;

	case CLOSEP_IN:
		systemState = CLOSEP_STATE;
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": For <memref> label and register formats there was not a new line1. Or for <memref> register reference formats there was not a closing parenthesis after the referenced register.";
		break;
	};
}


void Parser::update_openP(States & systemState, inputValue input, string inputString)
{
	assert(systemState == OPENP_STATE);
	
	auto labelFind = labelsMap.find(inputString);

	//inputValue is a register after an open parenthisis || When the immediate value is a constant || the input is an immediate value and covers when the immediate value is an integer ||
	if (input == REGISTER_IN || input == INITALIZED_CONSTANT_IN || input == INTEGER_IN || (labelFind != labelsMap.end()) ) { // This means the input <memref> is a label name already mapped to a memory address
		incrementVectorOfOpcodeArguments(inputString, input);
		systemState = ARG2_MEMREF_STATE;
	}
	else {
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": In the <memref> state but there is no valid register being referenced.";
	}

}

void Parser::update_closeP(States & systemState, inputValue input)
{
	assert(systemState == CLOSEP_STATE);

	switch (input) {
	case EOL_IN:
		incrementVectorOfProgramInstructions(opcode, opcodeArguments); 
		systemState = text_EOL_STATE;
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": There is extra information on the current line after the closed parenthesis in the <memref>.";
		break;
	}
}

void Parser::update_offsetOrImmediate(States & systemState, inputValue input)
{
	assert(systemState == OFFSET_STATE);

	switch (input) {
	case OPENP_IN: // When the previous input was an [OFFSET]
		systemState = OPENP_STATE;
		break;

	case EOL_IN: // When the previous input was an immediate
		incrementVectorOfProgramInstructions(opcode, opcodeArguments);
		systemState = text_EOL_STATE;
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": There is not a corresponding open parenthesis for the offset of the <memref>";
		break;
	}
}


void Parser::update_text_label(States& systemState, inputValue input) //  note: The .data for labels is actually handled in the update_eol() when it determines that it is a label
{
	assert(systemState == LABEL_STATE_S0);

	// This below is specifically for the .text labelnames
	if (input == EOL_IN || input == OPCODE_IN || input == NOP_IN) { // want to allocate for valid inputs only
		auto labelFind = labelsMap.find(labelName);
		if (labelFind == labelsMap.end()) { // If this is a new label add it to my labelsMap
			LabelInfo layoutStruc;
			layoutStruc.isInstructionIndex = true;
			layoutStruc.instructionIndexOrFirstMemAddress = programInstructions.size(); // If it is empty index will be 0, indicating that the label will map to the next instructionType element added. That is why the index will be the
			labelsMap.emplace(labelName, layoutStruc);									// The size() and not size()-1. HOWEVER NEED TO IMPLEMENT A CASE WHERE THERE IS A <label> EOL AT THE END OF THE FILE BECAUSE I AM ASSUMING THERE IS A AN INSTRUCION AFTER
		}																				//  If there is a last label before the end of file, then check that labels index and make sure its instructionIndexOrFirstMemAddress <= programInstructions.size() - 1;
	} 
	
	if (input == EOL_IN) {
		incrementVectorOfProgramInstructions(opcode, opcodeArguments);
		systemState = text_EOL_STATE;
	}
	else if (input == OPCODE_IN) 
		systemState = OPCODE_STATE;
	else if (input == NOP_IN) 
		systemState = NOP_STATE;
	else {
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": Not a valid format for a <label>.";
	}
}

void Parser::checkandUpdateDataLabelsMap(States & systemState) 
{
	string labelNameMinusColon;
	for (size_t i = 0; i < labelName.length(); i++)
		labelNameMinusColon = labelNameMinusColon + labelName[i];

	auto labelFind = labelsMap.find(labelNameMinusColon);
	if (labelFind != labelsMap.end()) {
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": There was another identical label declared previously."; // *MEANS IN OUR PARSER RE-ASSIGNING LABEL NAMES IS NOT ALLOWED*
	}
	else {
		LabelInfo layoutStruc;
		layoutStruc.isInstructionIndex = false; 
		layoutStruc.instructionIndexOrFirstMemAddress = lastAvailableAddress; // Since we are checking the label in the beginning the lastAvailableAdress will be the value mapped from the labelname. Aka first memory address of the label
		labelsMap.emplace(labelName, layoutStruc);
	}
}

void Parser::update_eol(States & systemState, inputValue input)
{
	assert(systemState == EOL_STATE);

	if ( (input == LABEL_IN) || (input == STRING_LAYOUT_IN) || (input == INT_LAYOUT_IN) || (input == DECLARATION_MODE_IN) ) {
		
		if (input == STRING_LAYOUT_IN) // If there is no label attached to the string layout go directly to LAYOUT_STATE_STR0. example on line: .ascii "bleh" 
			systemState = LAYOUT_STATE_STR_S0;
		else if (input == INT_LAYOUT_IN)
			systemState = LAYOUT_STATE_INT_S0;
		else {
			systemState = DECLARATION_MODE_STATE;
			if (input == LABEL_IN)
				checkandUpdateDataLabelsMap(systemState);  // If it is a valid label continue parsing, if not == ERROR_STATE. Also in this function adds the label to the labelMap
		}
		
	}
	else if (input == CONSTANT_FIRST_PART_IN || input == INITALIZED_CONSTANT_IN) {
		systemState = CONSTANT_STATE_S0; 
	}
	else if (input == TEXT_MODE_IN) { 
		systemState = TEXT_MODE_STATE;
	}
	else {
		systemState = ERROR_STATE; 
		errorMessage = "Error:" + to_string(lineOfError) + ": Not a valid data nor text declaration.";
	}
}

void Parser::update_constant1(States & systemState, inputValue input)
{
	assert(systemState == CONSTANT_STATE_S0);

	switch (input) {
	case EQUAL_IN:
		systemState = CONSTANT_STATE_S1;
		break;
	
	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": constant did not have a corresponding equal sign(=).";
		break;
	};
}

void Parser::update_constant2(States & systemState, inputValue input)
{
	assert(systemState == CONSTANT_STATE_S1);

	switch (input) {
	case INTEGER_IN:
		systemState = CONSTANT_STATE_S2;
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": constant did not have an integer assigned to it.";
		break;
	};
}

void Parser::update_constant3(States & systemState, inputValue input, string inputString)
{
	assert(systemState == CONSTANT_STATE_S2);

	if (input == EOL_IN) {
		systemState = EOL_STATE;
		//ADDing THE CONSTANT TO A MAP, remember we have constantName string variable and inputString should be the integer because in the EOL_IN classify case I do not reassign inputString 
		auto constantFind = constantsMap.find(constantName); // auto is just an iterator for the unordered_map
		if (constantFind != constantsMap.end()) { // If the constant is in the map, then overwrite the value. Else emplace it into the unordered_map
			constantFind->second = inputString;
		}
		else {
			constantsMap.emplace(constantName, inputString); 
		}
	}
	else {
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": extra information on the line where the constant is declared.";
	}
}

void Parser::update_intLayout1(States & systemState, inputValue input, string inputString)
{
	assert(systemState == LAYOUT_STATE_INT_S0);
	//The initial Integer or previous Initialized Constant before the repetitive cases
	intLayout_IntOrConstantCheck(systemState, input, inputString); // The inputString should be the already initialized constant if the input is a constant
}

void Parser::update_intLayout2(States & systemState, inputValue input)
{
	assert(systemState == LAYOUT_STATE_INT_S1);

	switch (input) {
	case EOL_IN:
		systemState = EOL_STATE;
		break;

	case SEP_IN:
		systemState = SEP_STATE; // This is basically the LAYOUT_STATE_INT_S2
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": extra information on the line where an int layout is declared.";
		break;
	};
}

void Parser::update_sep(States & systemState, inputValue input, string inputString) //In the text mode section it is easier to say that: update_sep()
{
	assert(systemState == SEP_STATE);
	if(mode == ".data")
		intLayout_IntOrConstantCheck(systemState, input, inputString);
	else if (mode == ".text") 
	{
		auto gotConstant = constantsMap.find(inputString);

		if (opcodeArguments.size() < 2) // execute when I only I have the first argument stored in the arguments vector
		{ 
			auto gotLabel = labelsMap.find(inputString); // See if the inputString is already a predefined label name, If there is then we can use this name 

			if (((opcode == "move") || (opcode == "add") || (opcode == "addu") || (opcode == "sub") || (opcode == "subu") || (opcode == "mul") ||
				(opcode == "mulo") || (opcode == "mulou") || (opcode == "mult") || (opcode == "multu") || (opcode == "div") || (opcode == "divu") ||
				(opcode == "rem") || (opcode == "remu") || (opcode == "abs") || (opcode == "neg") || (opcode == "negu") || (opcode == "and") ||
				(opcode == "nor") || (opcode == "or") || (opcode == "xor")) && (input == REGISTER_IN)) {

				incrementVectorOfOpcodeArguments(inputString, input); // This pushes argument 2 into my vector with an input value of a register
				systemState = ARG2_REG_STATE;				
			}
			else if ((opcode == "li") && ((input == INTEGER_IN) || (gotConstant != constantsMap.end()) )  ) {

				incrementVectorOfOpcodeArguments(inputString, input); // This pushes argument 2 into my vector with an inputValue of either a integer or Constant
				systemState = ARG2_IMMEDIATE_STATE;
			}
			else if (((opcode == "not") || (opcode == "beq") || (opcode == "bne") || (opcode == "blt") || (opcode == "ble") || (opcode == "bgt") ||
				      (opcode == "bge")) && ((input == INTEGER_IN) || (gotConstant != constantsMap.end()) || (input == REGISTER_IN) ||
				      (input == CONSTANT_FIRST_PART_IN))  ) { //**ACTUALLY THE (INPUT==CONSTANT_FIRST) IS NOT PREDEFINED because it is a future initialized label, but after parsing that specific constant = a labels name

				incrementVectorOfOpcodeArguments(inputString, input); // This pushes argument 2 into my vector with an inputValue of either a integer, Constant or a register 
				systemState = ARG_SOURCE_STATE;
			}
			else if (((opcode == "lw") || (opcode == "lh") || (opcode == "lb") || (opcode == "la") || (opcode == "sw") || (opcode == "sh") ||
					  (opcode == "sb")) && ( (gotLabel != labelsMap.end()) || (input == REGISTER_IN) || (input == INITALIZED_CONSTANT_IN) ||
					  (input == INTEGER_IN) || (input == OPENP_IN)) ) { // The INTEGER_IN = [OFFSET] or can also be INTEGER_IN = Immediate : WORKING HERE!!!!!!!

				if (input == REGISTER_IN || (gotLabel != labelsMap.end() || input == INITALIZED_CONSTANT_IN) ) {
					incrementVectorOfOpcodeArguments(inputString, input); // This pushes argument 2 into my vector with an inputValue of either a label or register
					systemState = ARG2_MEMREF_STATE;
				}
				else if (input == OPENP_IN) {
					systemState = OPENP_STATE;
				}
				else if (input == INTEGER_IN) {
					incrementVectorOfOpcodeArguments(inputString, input);
					systemState = OFFSET_STATE; // besides an Offset, The integer can also be an Immediate value
				}
			}
			else {
				systemState = ERROR_STATE;
				errorMessage = "Error:" + to_string(lineOfError) + ": The opcode does not match with its corresponding second argument";
			}
		}
		else if(opcodeArguments.size() < 4) // execute with arguments 2 and 3, which are stored in the arguments vector
		{
			if (((opcode == "add") || (opcode == "addu") || (opcode == "sub") || (opcode == "subu") || (opcode == "mul") || (opcode == "mulo") || 
				(opcode == "mulou") || (opcode == "div") || (opcode == "divu") || (opcode == "rem") || (opcode == "remu") || 
				(opcode == "and") || (opcode == "nor") || (opcode == "or") || (opcode == "xor"))    && 
				((input == INTEGER_IN) || (gotConstant != constantsMap.end()) || (input == REGISTER_IN))    ) {

				incrementVectorOfOpcodeArguments(inputString, input); // This pushes argument 3 into my vector with an inputValue of either a integer, Constant or register
				systemState = ARG_SOURCE_STATE;
			}
			else if (((opcode == "beq") || (opcode == "bne") || (opcode == "blt") || (opcode == "ble") || (opcode == "bgt") ||
					 (opcode == "bge")) && ( (gotConstant != constantsMap.end()) || (input == CONSTANT_FIRST_PART_IN))  ) {  //**** the CONSTANT_FIRST_PART_IN is a predicted label that we can jump to using jump instructions

				incrementVectorOfOpcodeArguments(inputString, input); // This pushes argument 3 into my vector with an inputValue of a label
				systemState = LABEL_STATE_S0;
			}
			else {
				systemState = ERROR_STATE;
				errorMessage = "Error:" + to_string(lineOfError) + ": The opcode does not match with its corresponding third argument";
			}
		}
		else {
			systemState = ERROR_STATE;
			errorMessage = "Error:" + to_string(lineOfError) + ": There are multiple arguments on the same line. 3 is the maximum arguments allowed. ";
		}

	}

}

void Parser::update_strLayout1(States & systemState, inputValue input)
{
	assert(systemState == LAYOUT_STATE_STR_S0);

	switch (input) {
	case STRING_DELIM_IN:
		systemState = STRING_DELIM_STATE1;
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": No string delimiter after string layout has been declared.";
		break;
	};
}

void Parser::update_strLayout2(States & systemState, inputValue input)
{
	assert(systemState == STRING_DELIM_STATE1);

	// Anytype of string in the the string delimiters is valid
	if (input == CHARS_IN || input == CONSTANT_FIRST_PART_IN || input == LABEL_IN || input == INITALIZED_CONSTANT_IN || input == INTEGER_IN) {
		systemState = LAYOUT_STATE_STR_S2;
	}
}

void Parser::update_strLayout3(States & systemState, inputValue input)
{
	assert(systemState == LAYOUT_STATE_STR_S2);

	switch (input) {
	case STRING_DELIM_IN:
		systemState = STRING_DELIM_STATE2;
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": No closing string delimiter after the <string>.";
		break;
	};
}

void Parser::update_strLayout4(States & systemState, inputValue input, string inputString)
{
	assert(systemState = STRING_DELIM_STATE2);

	switch (input) {
	case EOL_IN:
		//ALLOCATE MEMORY for either .ascii or .asciiz. Will use the layoutKey variable to determine which format, but inputString which will be the <string> information can be properly allocated.
		// Each character gets their own byte
		if (layoutKey == ".ascii")
			asciiMemAllocation(inputString);
		else if (layoutKey == ".asciiz") {
			asciiMemAllocation(inputString);
			memoryLayout.push_back("00"); // adding the terminated null character as a byte = zero
			lastAvailableAddress++;
		}

		systemState = EOL_STATE;
		break;

	default:
		systemState = ERROR_STATE;
		errorMessage = "Error:" + to_string(lineOfError) + ": extra information on the line where a string layout is declared.";
		break;
	};
}



bool Parser::fitsBits(const string layoutKey, const string inputInteger, const inputValue input) 
{
	// the inputInteger can be either an <integer> or the value of the <constant>
	char firstCharInteger = inputInteger[0];
	int64_t convertedInput = stoll(inputInteger); //Range: –9,223,372,036,854,775,808 through 9,223,372,036,854,775,807 
	
	if (layoutKey == ".word" || layoutKey == ".space" || (input == INITALIZED_CONSTANT_IN && (layoutKey == ".word" || layoutKey == ".space")) ) {
		if (firstCharInteger == '+' || firstCharInteger == '-') { // These are signed integers. RANGE: -2,147,483,648 (-2^31) through 2,147,483,647 (2^31 - 1)
			int32_t i32min = numeric_limits<int32_t>::min(); // min() function that returns the smallest finite value of the given type 
			
			if ((convertedInput >= i32min) && (convertedInput <= 2147483647))
				return true;
		}
		else { // These are unsigned integers. RANGE: 0 through 4,294,967,295 (2^32 - 1) 
			if (convertedInput <= 4294967295)
				return true;
		}
	}
	else if (layoutKey == ".half" || (input == INITALIZED_CONSTANT_IN && layoutKey == ".half") ) {
		if (firstCharInteger == '+' || firstCharInteger == '-') { // These are signed integers. RANGE: -32768 (-2^15) through 32767 (2^15 - 1)
			if ((convertedInput >= -32768) && (convertedInput <= 32767))
				return true;
		}
		else { // These are unsigned integers. RANGE: 0 through 65535 (2^16 - 1)
			if (convertedInput <= 665535)
				return true;
		}
	}
	else if (layoutKey == ".byte" || (input == INITALIZED_CONSTANT_IN && layoutKey == ".byte") ) {
		if (firstCharInteger == '+' || firstCharInteger == '-') { // These are signed integers. RANGE: -128 (-2^7) through 127
			if ((convertedInput >= -128) && (convertedInput <= 127))
				return true;
		}
		else { // These are unsigned integers. RANGE: 0 through 255 (2^8 - 1)
			if (convertedInput <= 255)
				return true;
		}
	}

	return false; // If in an if statement does not return true means that this function will return false because it did not satisfy the necessary condition
}

void Parser::intLayout_IntOrConstantCheck(States & systemState, inputValue input, string inputString)
{
	if (input == INTEGER_IN) {
		// 1) use the inputString that will contain the integer, check to see if integer is valid size with its corresponding specific key layout(".word", ".half", ".byte", ".space")
		// 2) ALLOCATE MEMORY for the data as we go, Depending on the key layout for the specific integer
		if (fitsBits(layoutKey, inputString, input)) {
			//ALLOCATE APPROPRIATE SIZE HERE BASED ON LAYOUT KEY. for example .space specfies how many bytes of memory to be alllocated and fills them with zero. DIFFERENT THAN .word
			
			helpIntMemAllocation(inputString, layoutKey);
			systemState = LAYOUT_STATE_INT_S1;
		}
		else {
			systemState = ERROR_STATE;
			errorMessage = "Error:" + to_string(lineOfError) + ": the integer(in bits) did not fit its corresponding layout.";
		}
	}
	else if (input == INITALIZED_CONSTANT_IN) {
		auto it = constantsMap.find(inputString); // The inputstring should be the already initalized constant
		if (fitsBits(layoutKey, it->second, input)) {
			//ALLOCATing APPROPRIATE HERE based on the keyLayout: CONSTANT will not be used in an int_layout declaration for this parser. for example cant do .byte CONSTANT,B,Z
			//helpIntMemAllocation(inputString, layoutKey);   

			systemState = LAYOUT_STATE_INT_S1;
		}
		else {
			systemState = ERROR_STATE;
			errorMessage = "Error:" + to_string(lineOfError) + ": the previously defined constant(in bits) did not fit its corresponding layout.";
		}
	}
	else {
		systemState = ERROR_STATE;
		if(input == EOL_IN) // B/C the error occured on the EOL input indicates that the error happened on the previous line
			errorMessage = "Error:" + to_string(lineOfError) + ": There is no follow up integer for its layout on previous line.";
		else
			errorMessage = "Error:" + to_string(lineOfError) + ": Not a valid integer for its layout.";
	}
}


void Parser::stringTokenDeclarationClassify(string& inputString, char firstChar, char lastChar, inputValue& input)
{
	if ((inputString == ".word") || (inputString == ".half") || (inputString == ".byte") || (inputString == ".space")) {
		input = INT_LAYOUT_IN;

		if (inputString == ".word") { //LSB first -> The allocation occurs in intLayout_IntOrConstantCheck() function and update_strLayout3() function which is for .ascii and .asciiz
									  //followed by a comma separated list of 32 bit integers I. Allocate 4 bytes of space
			layoutKey = ".word"; //--> Private variable so I will not lose the layout key
		}
		else if (inputString == ".half") {
			//followed by a comma separated list of 16 bit integers I. Allocate 2 bytes of space
			layoutKey = ".half";
		}
		else if (inputString == ".byte") {
			//followed by a comma separated list of 8 bit integers. Allocate 1 byte of space
			layoutKey = ".byte";
		}
		else if (inputString == ".space") {
			//followed by an integer I specifying the amount of space in bytes. Allocate I bytes of space
			layoutKey = ".space";
		}
	}
	else if ((inputString == ".ascii") || (inputString == ".asciiz")) {
		input = STRING_LAYOUT_IN;

		if (inputString == ".ascii") {
			//followed by a string of length L (not automatically null-terminated). Allocate L bytes of space
			layoutKey = ".ascii";
		}
		else if (inputString == ".asciiz") {
			//followed by a string of length L (automatically null-terminated). Same as .ascii but terminate the string with a null (decimal value 0) character
			layoutKey = ".asciiz";
		}
	}
	else if (systemState == STRING_DELIM_STATE1) { // CHECK to see if STRING token is the characters inbetween a <string> ; Make sure previous systemState = STRING_DELIM
		for (size_t i = 0; i < inputString.length(); i++) {
			if (!isprint(inputString[i])) {
				systemState = ERROR_STATE; input = ERROR_IN;
				errorMessage = "Error:" + to_string(lineOfError) + ": The characters in the <string> must be printable by ascii.";
			}
		}

		if (systemState != ERROR_STATE)
			input = CHARS_IN;
	}
	else if ((inputString.length() != 1) && (lastChar == ':') && (isalpha(firstChar) || (firstChar == '_'))) { // CHECK to see if STRING is a <Label>
		
		checkIfLabel(systemState, input, inputString); // Checks if the string is a label and performs certain actions depending on the result

	}
	else if ((isalpha(firstChar) || (firstChar == '_'))) { // POTENTIAL to be a new <constant> OR could be a constant already declared

		checkIfConstant(systemState, input, inputString);

	}
	else if ((firstChar == '-') || (firstChar == '+') || (isdigit(firstChar))) { // CHECK to see if STRING is an <integer>
		
		checkIfInteger(systemState, input, inputString, firstChar);

	}
	else {
		systemState = ERROR_STATE;
		input = ERROR_IN;
		errorMessage = "Error:" + to_string(lineOfError) + ": Not a valid Declaration. Could not be defined in the data declaration MIPS assembly.";
	}
}

void Parser::checkIfLabel(States& systemState, inputValue& input, string inputString)
{
	for (size_t i = 0; i < inputString.length() - 1; i++) {  // It is length() - 1 because i am not including the last character which should be ':'
		if (!isdigit(inputString[i]) && !isalpha(inputString[i]) && !(inputString[i] == '_')) {
			systemState = ERROR_STATE;
			input = ERROR_IN;
			errorMessage = "Error:" + to_string(lineOfError) + ": Not proper way to initialize a <Label>.";
			break;
		}
	}

	if (systemState != ERROR_STATE) {
		input = LABEL_IN;
		labelName = ""; // Re-initialize the labelName to null so it does not add on to the current existing label names
		for (size_t i = 0; i < inputString.length() - 1; i++)  // saves the labels name(but not ':') just in case if I assign a label to an integer from the int_layout
			labelName = labelName + inputString[i];
	}
}

void Parser::checkIfConstant(States& systemState, inputValue& input, string inputString)
{
	auto got = constantsMap.find(inputString); // == std::unordered_map<string,int>::const_iterator got

	if (got != constantsMap.end()) {
		input = INITALIZED_CONSTANT_IN;
	}
	else {
		for (size_t i = 0; i < inputString.length(); i++) {
			if (!isdigit(inputString[i]) && !isalpha(inputString[i]) && !(inputString[i] == '_')) {
				systemState = ERROR_STATE;
				input = ERROR_IN;
				errorMessage = "Error:" + to_string(lineOfError) + ": Not proper way to initialize a <constant>.";
				break;
			}
		}

		if (systemState != ERROR_STATE) {
			input = CONSTANT_FIRST_PART_IN;
			constantName = inputString; // private variable so now I will not be afraid of losing the name of the assigned integer
		}
	}
}

void Parser::checkIfInteger(States & systemState, inputValue & input, string inputString, char firstChar)
{
	if ((inputString.length() == 1) && !isdigit(firstChar)) {
		systemState = ERROR_STATE;
		input = ERROR_IN;
		errorMessage = "Error:" + to_string(lineOfError) + ": An Integer requires a digit with its sign.";
	}
	else {
		size_t i = 0;
		if ((firstChar == '-') || (firstChar == '+'))
			i = 1;

		while (i < inputString.length()) {
			if (!isdigit(inputString[i]) ) {
				systemState = ERROR_STATE;
				input = ERROR_IN;
				errorMessage = "Error:" + to_string(lineOfError) + ": An Integer is only made up of digits.";
				break;
			}
			i++;
		}

		if (systemState != ERROR_STATE) {
			input = INTEGER_IN;
			inputString = inputString;
		}
	}
}

bool Parser::isOpcode(string inputString)
{
	//Data Movement instructions
	if ((inputString == "lw") || (inputString == "lh") || (inputString == "lb") || (inputString == "li") || (inputString == "la") ||
		(inputString == "sw") || (inputString == "sh") || (inputString == "sb") || (inputString == "move") || (inputString == "mfhi") ||
		(inputString == "mflo") || (inputString == "mthi") || (inputString == "mtlo") || (inputString == "add") || (inputString == "addu") ||
		(inputString == "sub") || (inputString == "subu") || (inputString == "mul") || (inputString == "mulo") || (inputString == "mulou") ||
		(inputString == "mult") || (inputString == "multu") || (inputString == "div") || (inputString == "divu") || (inputString == "rem") ||
		(inputString == "remu") || (inputString == "abs") || (inputString == "neg") || (inputString == "negu") || (inputString == "and") ||
		(inputString == "nor") || (inputString == "not") || (inputString == "or") || (inputString == "xor") || (inputString == "j") ||
		(inputString == "beq") || (inputString == "bne") || (inputString == "blt") || (inputString == "ble") || (inputString == "bgt") ||
		(inputString == "bge")) { // All of the possible opcodes

		return true;
	}
	else
		return false;
}

bool Parser::isRegister(string inputString, unordered_map<string, int32_t> registersMap) 
{
	string curRegister = determineRegister(inputString);
	unordered_map<string, int32_t>::const_iterator found = registersMap.find(curRegister);
	if (found != registersMap.end())
		return true;
	else
		return false;
}

void Parser::initializeRegisters()
{
	//Actual registers
	registersMap.emplace("$0", 0);
	registersMap.emplace("$1", 0);
	registersMap.emplace("$2", 0);
	registersMap.emplace("$3", 0);
	registersMap.emplace("$4", 0);
	registersMap.emplace("$5", 0);
	registersMap.emplace("$6", 0);
	registersMap.emplace("$7", 0);
	registersMap.emplace("$8", 0);
	registersMap.emplace("$9", 0);
	registersMap.emplace("$10", 0);
	registersMap.emplace("$11", 0);
	registersMap.emplace("$12", 0);
	registersMap.emplace("$13", 0);
	registersMap.emplace("$14", 0);
	registersMap.emplace("$15", 0);
	registersMap.emplace("$16", 0);
	registersMap.emplace("$17", 0);
	registersMap.emplace("$18", 0);
	registersMap.emplace("$19", 0);
	registersMap.emplace("$20", 0);
	registersMap.emplace("$21", 0);
	registersMap.emplace("$22", 0);
	registersMap.emplace("$23", 0);
	registersMap.emplace("$24", 0);
	registersMap.emplace("$25", 0);
	registersMap.emplace("$26", 0);
	registersMap.emplace("$27", 0);
	registersMap.emplace("$28", 0);
	registersMap.emplace("$29", 0);
	registersMap.emplace("$30", 0);
	registersMap.emplace("$31", 0);
	registersMap.emplace("$pc", 0);
	registersMap.emplace("$hi", 0);
	registersMap.emplace("$lo", 0);
}

void Parser::incrementVectorOfOpcodeArguments(const string inputString, const inputValue input)
{
	argument curArg;
	curArg.inValue = input;
	if (input == INITALIZED_CONSTANT_IN) {
		auto constantFind = constantsMap.find(inputString);
		curArg.inString = constantFind->second;
	}
	else 
		curArg.inString = inputString;

	opcodeArguments.push_back(curArg);
}

string Parser::determineRegister(string inputStringRegister)
{
	// The second condition is the Alias for the specific register
	if (inputStringRegister == "$0" || inputStringRegister == "$zero")// Hardwired to the value of zero
		return "$0";
	else if (inputStringRegister == "$1" || inputStringRegister == "$at")// Assembler Temporary (reserved for assembler pseudoinstructions)
		return "$0";
	else if (inputStringRegister == "$2" || inputStringRegister == "$v0") // expression evaluation and function results
		return "$2";
	else if (inputStringRegister == "$3" || inputStringRegister == "$v1")
		return "$3";
	else if (inputStringRegister == "$4" || inputStringRegister == "$a0")// first 4 arguments of procedures
		return "$4";
	else if (inputStringRegister == "$5" || inputStringRegister == "$a1")
		return "$5";
	else if (inputStringRegister == "$6" || inputStringRegister == "$a2")
		return "$6";
	else if (inputStringRegister == "$7" || inputStringRegister == "$a3")
		return "$7";
	else if (inputStringRegister == "$8" || inputStringRegister == "$t0")// Temporaries (not saved)
		return "$8";
	else if (inputStringRegister == "$9" || inputStringRegister == "$t1")
		return "$9";
	else if (inputStringRegister == "$10" || inputStringRegister == "$t2")
		return "$10";
	else if (inputStringRegister == "$11" || inputStringRegister == "$t3")
		return "$11";
	else if (inputStringRegister == "$12" || inputStringRegister == "$t4")
		return "$12";
	else if (inputStringRegister == "$13" || inputStringRegister == "$t5")
		return "$13";
	else if (inputStringRegister == "$14" || inputStringRegister == "$t6")
		return "$14";
	else if (inputStringRegister == "$15" || inputStringRegister == "$t7")
		return "$15";
	else if (inputStringRegister == "$16" || inputStringRegister == "$s0")// Saved Values
		return "$16";
	else if (inputStringRegister == "$17" || inputStringRegister == "$s1")
		return "$17";
	else if (inputStringRegister == "$18" || inputStringRegister == "$s2")
		return "$18";
	else if (inputStringRegister == "$19" || inputStringRegister == "$s3")
		return "$19";
	else if (inputStringRegister == "$20" || inputStringRegister == "$s4")
		return "$20";
	else if (inputStringRegister == "$21" || inputStringRegister == "$s5")
		return "$21";
	else if (inputStringRegister == "$22" || inputStringRegister == "$s6")
		return "$22";
	else if (inputStringRegister == "$23" || inputStringRegister == "$s7")
		return "$23";
	else if (inputStringRegister == "$24" || inputStringRegister == "$t8")// Temporaries (not saved)
		return "$24";
	else if (inputStringRegister == "$25" || inputStringRegister == "$t9")
		return "$25";
	else if (inputStringRegister == "$26" || inputStringRegister == "$k0")// for interrupt and exception handlers
		return "$26";
	else if (inputStringRegister == "$27" || inputStringRegister == "$k1")
		return "$27";
	else if (inputStringRegister == "$28" || inputStringRegister == "$gp")// the global pointer
		return "$28";
	else if (inputStringRegister == "$29" || inputStringRegister == "$sp") // stack pointer
		return "$29";
	else if (inputStringRegister == "$30" || inputStringRegister == "$fp") // frame pointer
		return "$30";
	else if (inputStringRegister == "$31" || inputStringRegister == "$ra") // return address
		return "$31";
	else if (inputStringRegister == "$pc") // program counter
		return "$pc";
	else if (inputStringRegister == "$hi") // 2 registers used by ALU
		return "$hi";
	else if (inputStringRegister == "$lo")
		return "$lo";
	else
		return "error"; // If the input string which is supposed to represent a register does not match any of the naming protocals
}
