#include "simulator.hpp"
#include "parser.hpp"
#include <assert.h>
#include <limits>
#include <thread>


//**************************Helper Function*******************************//
// Same function as in parser, If I have time integrate this without repetative code.
static string determineRegister(string inputStringRegister)
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

void Simulator::runParallelProcess() // This function will be run on a seperate thread called "thread1"
{
		
	while (parallelHasNotQuit) {
		// have a while loop that steps through the simulator as long the file hasnt finished
			while (!myMsgQueue.empty()) {
				if (isErrorStatus == false && isEndOfFile == false) {
					update_simulation();
					update_pcReg();
					isEndOfFileCheck();
				}
			}
	}

}

//***************************Simulator below******************************//
Simulator::Simulator(const unordered_map<string, int32_t> regMap, const vector<string> memory, const vector<instructionType> instructions, const unordered_map<string, LabelInfo> labelMap)
{
	registersMap = regMap;
	memoryLayout = memory;
	programInstructions = instructions;
	labelsMap = labelMap;

	PcReg = registersMap.find("$pc");

	systemState = WAIT_STATE;
	isErrorStatus = false;
	isEndOfFile = false;
	isControlInstr = false;

	while (memoryLayout.size() < 512)
		memoryLayout.push_back("00");

	parallelHasNotQuit = true;
	// uses the std::thread class move assignment operator to assign the RHS created thread to be the thread i created in my private: section of my class
	thread1 = std::thread(&Simulator::runParallelProcess,this); //this->runParallelProcess() function is put in a new thread
}

Simulator::~Simulator()
{
	// Not necessary to clear iterator

	parallelHasNotQuit = false; // This is so that the function that is running parallel can finish and reach the end of the functions scope
	thread1.join(); //thread excecution complete
}

void Simulator::clearDataStruct()
{
	// Clears all data structures contents and gives the size 0
	registersMap.clear();
	memoryLayout.clear();
	programInstructions.clear();
	labelsMap.clear();
}

void Simulator::runOrBreak_public(string command)
{
	if (command == "break") {
		if (!myMsgQueue.empty()) { // Does nothing if the simulation is not running
			string temp;
			myMsgQueue.wait_and_pop(temp); // This pops the value from the msgQueue allowing the runParallelProcess() function to stop running
			//cout << "Should be [run] : " << temp << endl;
		}
	}
	else if (command == "run") {
		if (isErrorStatus == false && isEndOfFile == false && myMsgQueue.empty()) // does nothing if the simulation is running
			myMsgQueue.push("run");
	}
}


void Simulator::setStartInstrIndex()
{
	unordered_map<string, LabelInfo>::iterator findLabel = labelsMap.find("main"); // The program counter starts at the main label, so want to find the label name "main"

	int32_t mainIndex = findLabel->second.instructionIndexOrFirstMemAddress; // This contains the index where the label main is located. SHould be located at index 0 for these test files
	PcReg->second = mainIndex; // Finds the program counter register and assigns the index(in programInstructions) of where the main is located in
}

void Simulator::update_simulation_public() { update_simulation(); } // Helper function which will be used by the VirtualMachineGUI class to "step"


bool Simulator::getErrorStatus() { return isErrorStatus; }

int32_t Simulator::getPcValue()
{
	int32_t pc = (PcReg->second);
	return pc;
}

vector<string> Simulator::getMemoryLayout() { return memoryLayout; }

unordered_map<string, int32_t> Simulator::getRegisterMap() { return registersMap; }

void Simulator::update_pcReg()
{
	int32_t printPc;
	printPc = (PcReg->second);
	if (isErrorStatus == false && isControlInstr == false && isEndOfFile == false) {
		printPc = (PcReg->second) + 1;
		(PcReg->second) = printPc; // Increment program counter and move on to next program instruction
	}
}

bool Simulator::isEndOfFileCheck()
{
	if ((PcReg->second) >= (int32_t)programInstructions.size()) // check to see if the progam counter index moved past the instructions available
		return (isEndOfFile = true);
	else
		return false;
}

void Simulator::simulate()
{
		string userInput;
		do {
			int32_t fprintPc;
			switch (systemState) {
			case WAIT_STATE:							// The WAIT_STATE can also be considered the INTERFACE_STATE
				update_simulatorInterface(userInput);
				break;

			case SIMULATING_STATE:
				update_simulation();
				update_pcReg();

				fprintPc = PcReg->second;
				printf("0x%08x\n", fprintPc); // Prints the program counter in 32 bit hex

				isEndOfFileCheck();
				systemState = WAIT_STATE;
				break;

			case QUIT_STATE:
				//Dummy state because once the systemState is set to QUIT_STATE the simulator stops
				break;
			};

		} while (systemState != QUIT_STATE);
}


void Simulator::update_simulatorInterface(string userInput)
{
	//cout << "is empty if value=1 : " << myMsgQueue.empty() << endl;

	cout << "simmips> ";
	// Here wait for the user to enter a command(print, status, step, quit)
	getline(cin, userInput); // This gets the the command the user just entered and stores it into string userInput

	if (userInput.substr(0, 7) == "print $") { // prints the register contents, either a register number, alias, or one of pc, hi, or lo in hex format
		
		if (myMsgQueue.empty()) {
			string reg;
			for (size_t i = 6; i < userInput.size(); i++) // Start at 6 to include the $ sign
				reg.push_back(userInput[i]);

			reg = determineRegister(reg); // If the string is "error" then means input was not valid
			if (reg == "error")
				cerr << "Error : not a valid register. Either enter $0 - $31 or the registers alias(ex.$t0 for $8)." << endl;
			else {
				auto findReg = registersMap.find(reg);
				int32_t printRegContents;
				printRegContents = findReg->second;
				printf("0x%08x\n", printRegContents);
			}
		}
		else
			cerr << "Error: simulation running. Type break to halt." << endl;

		systemState = WAIT_STATE;

	}
	else if (userInput.substr(0, 7) == "print &") { // prints the byte at memory address a, in hex format
		
		if (myMsgQueue.empty()) {
			string memReference;
			for (size_t i = 7; i < userInput.size(); i++) // ASSUMING THE USER HAS INPUTED A MEMREF OF 32 BITS (0x0000000)
				memReference.push_back(userInput[i]);

			size_t memAddress = stol(memReference, nullptr, 16); // converts the string which is in hex format to an integer

			if (memAddress >= memoryLayout.size())
				cerr << "Error : Memory reference Out of Bounds!" << endl;
			else {
				int32_t i = (int32_t)stol(memoryLayout[memAddress], nullptr, 16); // converts the string(hex format) to an integer before it is displayed as hex(0x00) using printf
				printf("0x%02x\n", i);
			}
		}
		else
			cerr << "Error: simulation running. Type break to halt." << endl;

		systemState = WAIT_STATE;

	}
	else if (userInput == "step") { // executes the current instruction and print the new program counter, or if an error has occurred the error message
		
		if (isErrorStatus == false && isEndOfFile == false && myMsgQueue.empty()) {
			systemState = SIMULATING_STATE;
		}
		else {
			if(!myMsgQueue.empty())
				cerr << "Error: simulation running. Type break to halt." << endl;

			systemState = WAIT_STATE; // incrementing virtual machine at the end of the .asm file should do nothing
			//cerr << "Error : can not proceed through simulation b/c of previous instruction simulated. Or reached the end of the file, enter <quit>." << endl;
		}
		
	}
	else if (userInput == "status") { // prints the error status as a message, print nothing if no error has been encountered
		if (isErrorStatus == true)
			cerr << "Error : on previous Instruction simulated. <print $pc> to see where the error occured." << endl;
		systemState = WAIT_STATE;
	}
	else if (userInput == "run") {
		runOrBreak_public("run");
		systemState = WAIT_STATE; 

	}
	else if (userInput == "break") {
		runOrBreak_public("break");
		systemState = WAIT_STATE;

	}
	else if (userInput == "quit") {
		parallelHasNotQuit = false;
		systemState = QUIT_STATE;
	}
	else {
		cerr << "Error : unknown command." << endl;
		systemState = WAIT_STATE;
	}
}

void Simulator::update_simulation()
{
	instructionType curInstruction; 
	size_t curInstructionIndex = PcReg->second; // This is the index where we are performing the current instruction
	curInstruction = programInstructions[curInstructionIndex]; // assigning an instructionType to the instructionType located at the current index
	size_t numOfArgs = curInstruction.instructionArgs.size(); // number of arguments in the current instruction
	string curOpcode = curInstruction.opcode; // the current opcode
	vector<argument> curArgs = curInstruction.instructionArgs; // This is a vector of the actual arguments(registers, immediates, constants, etc.) and its inputValue(ex. REGISTER_IN)
	isControlInstr = false;


	if ((numOfArgs == 1) && (curOpcode == "j" || curOpcode == "nop" || curOpcode == "mfhi" || curOpcode == "mflo")) {
		
		argument arg1 = curArgs[0];
		if (curOpcode == "j") {
			unordered_map<string, LabelInfo>::iterator itLabel = labelsMap.find(arg1.inString); // iterator finds the label it is supposed to jump too
			LabelInfo labelInfor = itLabel->second;

			if ((itLabel == labelsMap.end()) || ((int32_t)(labelInfor.instructionIndexOrFirstMemAddress) >= (int32_t)programInstructions.size()) )
				isErrorStatus = true; // error if any label is unknown or the program counter becomes out-of-bounds
			else {
				isControlInstr = true;
				PcReg->second = labelInfor.instructionIndexOrFirstMemAddress;
			}	
		}
		else if (curOpcode == "nop") {}// Do nothing because after the update_simulation() I automatically increment the counter
		else if (curOpcode == "mfhi" || curOpcode == "mflo") {
			string realReg = determineRegister(arg1.inString);
			unordered_map<string, int32_t>::iterator findReg = registersMap.find(realReg);
			string tempStr;
			if (curOpcode == "mfhi")
				tempStr = "$hi";
			else
				tempStr = "$lo";

			unordered_map<string, int32_t>::iterator findhiOrLo = registersMap.find(tempStr);
			findReg->second = findhiOrLo->second;
		}

	}
	else if ((numOfArgs == 2 || numOfArgs == 3) && (curOpcode == "lw" || curOpcode == "la" || curOpcode == "sw") ) {

		string realReg = determineRegister(curArgs[0].inString); // arg1
		argument memArg;
		argument offsetArg;
		if (numOfArgs == 2)
			memArg = curArgs[1];
		else {
			offsetArg = curArgs[1];
			memArg = curArgs[2];
		}
		unordered_map<string, int32_t>::iterator findReg = registersMap.find(realReg);

		if (curOpcode == "lw" || curOpcode == "la") {
			if (memArg.inValue == CONSTANT_FIRST_PART_IN) { // Label
				unordered_map<string, LabelInfo>::iterator itLabel = labelsMap.find(memArg.inString);
				LabelInfo labelInfor = itLabel->second;
				size_t memOrindex = labelInfor.instructionIndexOrFirstMemAddress; // This is either the first memory address for "lw" or the instruction index for "la"

				if (numOfArgs == 2) {
					if ((itLabel == labelsMap.end()) || (memOrindex + 3 >= memoryLayout.size()))
						isErrorStatus = true; // error if any label is unknown or the program counter becomes out-of-bounds
					else { // Depending on opcode stores in either the memory address or value in the address
						if (curOpcode == "lw") {
							string data32Bits = memoryLayout[memOrindex + 3] + memoryLayout[memOrindex + 2] + memoryLayout[memOrindex + 1] + memoryLayout[memOrindex]; // This wil be used for "lw" storing the 32 bit value in a register
							findReg->second = (int32_t)stoll(data32Bits, nullptr, 16);
						}
						else if(curOpcode == "la")
							findReg->second = (int32_t)memOrindex;
					}	
				}
				else { // In the case below is similar to above except we have an offset which makes the number of arguments 3
					if ((itLabel == labelsMap.end()) || (memOrindex + (int32_t)stoll(offsetArg.inString) + 3 >= memoryLayout.size()) )
						isErrorStatus = true; // error if any label is unknown or the program counter becomes out-of-bounds
					else {
						if (curOpcode == "lw") {
							memOrindex += (int32_t)stoll(offsetArg.inString); // Account for the offset in the starting location
							string data32Bits = memoryLayout[memOrindex + 3] + memoryLayout[memOrindex + 2] + memoryLayout[memOrindex + 1] + memoryLayout[memOrindex]; // This wil be used for "lw" storing the offset  32 bit value in a register
							findReg->second = (int32_t)stoll(data32Bits, nullptr, 16);
						}
						else if (curOpcode == "la")
							findReg->second = (int32_t)memOrindex + (int32_t)stoll(offsetArg.inString);
					}
				}
			}
			else if (memArg.inValue == INITALIZED_CONSTANT_IN || memArg.inValue == INTEGER_IN) { // Immediate(constant or integer)
				if (numOfArgs == 2) // "lw" and "la" act the same if they have an immediate, similar to "li"
					findReg->second = (int32_t)stoll(memArg.inString);
				else
					findReg->second = (int32_t)stoll(memArg.inString) + (int32_t)stoll(offsetArg.inString);
			}
			else if (memArg.inValue == REGISTER_IN) { // Register
				string realReg2 = determineRegister(memArg.inString);
				unordered_map<string, int32_t>::iterator findReg2 = registersMap.find(realReg2);
				int32_t memOrindex = findReg2->second;
				
				if (numOfArgs == 2) {
					if(memOrindex + 3 >= (int32_t)memoryLayout.size())
						isErrorStatus = true; // error program counter becomes out-of-bounds
					else {
						if (curOpcode == "lw") {
							string data32Bits = memoryLayout[memOrindex + 3] + memoryLayout[memOrindex + 2] + memoryLayout[memOrindex + 1] + memoryLayout[memOrindex];
							findReg->second = (int32_t)stoll(data32Bits, nullptr, 16);
						}
						else if(curOpcode == "la")
							findReg->second = memOrindex;
					}
				}
				else { // Arguments here is 3 that includes an offset
					int32_t offSet = (int32_t)stoll(offsetArg.inString);
					if ((memOrindex + offSet + 3) >= (int32_t)memoryLayout.size())
						isErrorStatus = true; // error program counter becomes out-of-bounds 
					else {
						if (curOpcode == "lw") {
							memOrindex += offSet; // Account for the offset in the starting location
							const string data32Bits = memoryLayout[memOrindex + 3] + memoryLayout[memOrindex + 2] + memoryLayout[memOrindex + 1] + memoryLayout[memOrindex];
							findReg->second = (int32_t)stoll(data32Bits, nullptr, 16);
						}
						else if (curOpcode == "la")
							findReg->second = memOrindex + offSet;
					}
				}
			}
		}
		else if (curOpcode == "sw") {
			std::stringstream hexStream; // going to be using a stringstream object that will represent our integer in a string hex format

			if (memArg.inValue == CONSTANT_FIRST_PART_IN || memArg.inValue == INITALIZED_CONSTANT_IN || memArg.inValue == INTEGER_IN || memArg.inValue == REGISTER_IN) { // Label, Immediate or register
				size_t memref; // This is the first memory address to start storing in bytes
				unordered_map<string, LabelInfo>::iterator itLabel; //-> If the input is a label
				LabelInfo labelInfor;
				string realReg2; //-> If the input is a register
				unordered_map<string, int32_t>::iterator findReg2;


				hexStream << setfill('0') << setw(8) << std::hex << findReg->second; // This converts the integer into a 8 character long(each 4 bits) hex value in a string stream
				string regValue = hexStream.str(); // converts the stream into an actual string

				if (memArg.inValue == CONSTANT_FIRST_PART_IN) {
					itLabel = labelsMap.find(memArg.inString);
					labelInfor = itLabel->second;
					memref = labelInfor.instructionIndexOrFirstMemAddress; 
				}
				else if (memArg.inValue == REGISTER_IN) {
					realReg2 = determineRegister(memArg.inString);
					findReg2 = registersMap.find(realReg2);
					memref = findReg2->second;
				}
				else // Else the input is an immediate (constant or integer)
					memref = (int32_t)stoll(memArg.inString);


				if (numOfArgs == 2) {
					if ((memArg.inValue == CONSTANT_FIRST_PART_IN && itLabel == labelsMap.end()) || (memref + 3 >= memoryLayout.size()) )
						isErrorStatus = true; // error if any label is unknown or the program counter becomes out-of-bounds
					else {
						memoryLayout[memref + 3] = regValue.substr(0, 2); // Storing the value in LSB
						memoryLayout[memref + 2] = regValue.substr(2, 2);
						memoryLayout[memref + 1] = regValue.substr(4, 2);
						memoryLayout[memref] = regValue.substr(6, 2);
					}
				}
				else { // In the case below is similar to above except we have an offset which makes the number of arguments 3
					if ((memArg.inValue == CONSTANT_FIRST_PART_IN && itLabel == labelsMap.end()) || (memref + (int32_t)stol(offsetArg.inString) + 3 >= memoryLayout.size()) )
						isErrorStatus = true; // error if any label is unknown or the program counter becomes out-of-bounds
					else {
						memref += (int32_t)stoll(offsetArg.inString);
						memoryLayout[memref + 3] = regValue.substr(0, 2); // Storing the value in LSB
						memoryLayout[memref + 2] = regValue.substr(2, 2);
						memoryLayout[memref + 1] = regValue.substr(4, 2);
						memoryLayout[memref] = regValue.substr(6, 2);
					}
				}
			}
		}

	}
	else if ((numOfArgs == 2) && (curOpcode == "li" || curOpcode == "move" || curOpcode == "not" || curOpcode == "mult" ||
								  curOpcode == "multu" || curOpcode == "div" || curOpcode == "divu") ) {

		string realReg = determineRegister(curArgs[0].inString); // arg1
		argument arg2 = curArgs[1];
		unordered_map<string, int32_t>::iterator findReg = registersMap.find(realReg);

		if (curOpcode == "li") {
			findReg->second = (int32_t)(stoll(arg2.inString));
		}
		else if (curOpcode == "move") {
			string realReg2 = determineRegister(arg2.inString);
			unordered_map<string, int32_t>::iterator findReg2 = registersMap.find(realReg2);

			int32_t temp = findReg->second;
			findReg->second = findReg2->second;
			findReg2->second = temp;
		}
		else if (curOpcode == "mult" || curOpcode == "multu" || curOpcode == "div" || curOpcode == "divu") {
			string realReg2 = determineRegister(arg2.inString);
			unordered_map<string, int32_t>::iterator findReg2 = registersMap.find(realReg2); // locate the real register for arg2

			unordered_map<string, int32_t>::iterator findhi = registersMap.find("$hi"); // locate the the hi and lo registers
			unordered_map<string, int32_t>::iterator findlo = registersMap.find("$lo");

			if (curOpcode == "mult") {
				uint32_t reg1, reg2;
				bitset<32> reg1BitSet(findReg->second);
				bitset<32> reg2BitSet(findReg2->second);
				if (reg1BitSet[31] == 1) // 2s complement
					reg1 = (uint32_t)(~(findReg->second)) + 1;
				else 
					reg1 = (uint32_t)(findReg->second);

				if (reg2BitSet[31] == 1) // 2s complement
					reg2 = (uint32_t)(~(findReg2->second)) + 1;
				else
					reg2 = (uint32_t)(findReg2->second);


				uint64_t multipliedValue = (uint64_t)(reg1)*(uint64_t)(reg2);
				uint32_t lo, hi;
				if (reg1BitSet[31] != reg2BitSet[31])
					multipliedValue = (uint64_t)(~(multipliedValue)) + 1;

					
				hi = (uint32_t)((multipliedValue & 0xffffffff00000000) >> 32);
				lo = (uint32_t)(multipliedValue & 0x00000000ffffffff);
				findlo->second = lo;
				findhi->second = hi;
			}
			else if (curOpcode == "multu") {
				uint64_t multipliedValue = (uint64_t)(findReg->second)*(uint64_t)(findReg2->second);
				uint32_t hi, lo;

				hi = (uint32_t)((multipliedValue & 0xffffffff00000000) >> 32);
				lo = (uint32_t)(multipliedValue & 0x00000000ffffffff);

				findlo->second = lo;
				findhi->second = hi;
			}
			else if (curOpcode == "div") {
				if (findReg2->second != 0) { // Undefined when attempting to divide by zero
					uint32_t reg1, reg2;
					bitset<32> reg1BitSet(findReg->second);
					bitset<32> reg2BitSet(findReg2->second);
					if (reg1BitSet[31] == 1) // 2s complement
						reg1 = (uint32_t)(~(findReg->second)) + 1;
					else
						reg1 = (uint32_t)(findReg->second);

					if (reg2BitSet[31] == 1) // 2s complement
						reg2 = (uint32_t)(~(findReg2->second)) + 1;
					else
						reg2 = (uint32_t)(findReg2->second);

					uint32_t divideValue = (uint32_t)(reg1)/(uint32_t)(reg2);
					if (reg1BitSet[31] != reg2BitSet[31])
						divideValue = (uint32_t)(~(divideValue)) + 1;

					int32_t divRem = (findReg->second) % (findReg2->second);
					if (divRem < 0)
						divRem *= -1;

					findlo->second = divideValue;
					findhi->second = divRem;
				}
			}
			else if (curOpcode == "divu") {
				if (findReg2->second != 0) { // Undefined when attempting to divide by zero
					uint32_t divideValue = (uint32_t)(findReg->second) / (uint32_t)(findReg2->second);
					uint32_t divRem = (uint32_t)(findReg->second) % (uint32_t)(findReg2->second);

					findlo->second = divideValue;
					findhi->second = divRem;
				}
			}
		}
		else if (curOpcode == "not") {
			if (arg2.inValue == REGISTER_IN) {
				string realReg2 = determineRegister(arg2.inString);
				unordered_map<string, int32_t>::iterator findReg2 = registersMap.find(realReg2); // locate the real register for arg2
				
				findReg->second = ~(findReg2->second);
			}
			else if (arg2.inValue == INITALIZED_CONSTANT_IN || arg2.inValue == INTEGER_IN)
				findReg->second = ~((int32_t)stoll(arg2.inString));
		}

	}							 // lw, la, sw are technically 2 argument opcodes but is 3 when there is an offset
	else if ((numOfArgs == 3) && (curOpcode == "and" || curOpcode == "nor" || curOpcode == "or" || curOpcode == "xor" || 
								  curOpcode == "add" || curOpcode == "addu" || curOpcode == "sub" || curOpcode == "subu" || 
								  curOpcode == "beq" || curOpcode == "bne" || curOpcode == "blt" ||
								  curOpcode == "ble" || curOpcode == "bgt" || curOpcode == "bge") ) {

		string realReg = determineRegister(curArgs[0].inString); // arg1
		argument arg2 = curArgs[1];
		argument arg3 = curArgs[2];
		unordered_map<string, int32_t>::iterator findReg = registersMap.find(realReg);

		if (curOpcode == "and" || curOpcode == "nor" || curOpcode == "or" || curOpcode == "xor" ||
				 curOpcode == "add" || curOpcode == "addu" || curOpcode == "sub" || curOpcode == "subu") {

			string realReg2 = determineRegister(arg2.inString);
			unordered_map<string, int32_t>::iterator findReg2 = registersMap.find(realReg2); // locate the real register for arg2
			int32_t argument3;
			if (arg3.inValue == REGISTER_IN) {
				string realReg3 = determineRegister(arg3.inString);
				unordered_map<string, int32_t>::iterator findReg3 = registersMap.find(realReg3); // locate the real register for arg3
				argument3 = findReg3->second;
			}
			else if (arg3.inValue == INITALIZED_CONSTANT_IN || arg3.inValue == INTEGER_IN) {
				argument3 = (int32_t)stoll(arg3.inString);
			}
			int32_t i32min = numeric_limits<int32_t>::min();
			int32_t i32max = numeric_limits<int32_t>::max();


			if (curOpcode == "and")
				findReg->second = (findReg2->second) & argument3;
			else if (curOpcode == "nor")
				findReg->second = ~((findReg2->second) | argument3);
			else if (curOpcode == "or")
				findReg->second = (findReg2->second) | argument3;
			else if (curOpcode == "xor")
				findReg->second = (findReg2->second) ^ argument3;
			else if (curOpcode == "add") {
				if ((findReg2->second + argument3 < i32min) || (findReg2->second + argument3 > i32max) )//arithmetic error occurs on 2-s complement overflow with no change to the registers.
					isErrorStatus = true;
				else
					findReg->second = findReg2->second + argument3;
			}
			else if (curOpcode == "addu") 
				findReg->second = (uint32_t)(findReg2->second + argument3);
			else if (curOpcode == "sub") {
				if ((findReg2->second - argument3 < i32min) || (findReg2->second - argument3 > i32max) )//arithmetic error occurs on 2-s complement overflow with no change to the registers.
					isErrorStatus = true;
				else
					findReg->second = findReg2->second - argument3;
			}
			else if (curOpcode == "subu") 
				findReg->second = (uint32_t)(findReg2->second - argument3);
		}
		else if (curOpcode == "beq" || curOpcode == "bne" || curOpcode == "blt" || curOpcode == "ble" || curOpcode == "bgt" || curOpcode == "bge") {
			unordered_map<string, LabelInfo>::iterator itLabel = labelsMap.find(arg3.inString); // iterator finds the label it is supposed to branch to
			LabelInfo labelInfor = itLabel->second;
			int32_t argument2;
			if (arg2.inValue == REGISTER_IN) {
				string realReg2 = determineRegister(arg2.inString);
				unordered_map<string, int32_t>::iterator findReg2 = registersMap.find(realReg2); // locate the real register for arg2
				argument2 = findReg2->second;
			}
			else if (arg2.inValue == INITALIZED_CONSTANT_IN || arg2.inValue == INTEGER_IN) {
				argument2 = (int32_t)stoll(arg2.inString);
			}
			

			if ((itLabel == labelsMap.end()) || ((int32_t)(labelInfor.instructionIndexOrFirstMemAddress) >= (int32_t)programInstructions.size()))
				isErrorStatus = true; // error if any label is unknown or the program counter becomes out-of-bounds
			else {
				isControlInstr = true;
				if ((curOpcode == "beq" && argument2 == findReg->second) || (curOpcode == "bne" && argument2 != findReg->second) ||
					(curOpcode == "blt" && findReg->second < argument2) || (curOpcode == "ble" && findReg->second <= argument2) ||
					(curOpcode == "bgt" && findReg->second > argument2) || (curOpcode == "bge" && findReg->second >= argument2)) {

					PcReg->second = labelInfor.instructionIndexOrFirstMemAddress;
				}
				else
					isControlInstr = false;
			}
		}
		
	}
	else // not a valid opcode with the specifed arguments for the simulator
		isErrorStatus = true;


}
