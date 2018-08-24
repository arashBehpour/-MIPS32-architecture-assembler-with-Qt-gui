#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "parser.hpp"
#include "threadSafeMessage.hpp"
#include <sstream> 
#include <iomanip>

using namespace std;

class Simulator 
{

private:
	enum SimStates { WAIT_STATE, SIMULATING_STATE, QUIT_STATE } systemState;

	unordered_map<string, int32_t> registersMap; // I like the names :)
	vector<string> memoryLayout;
	vector<instructionType> programInstructions;
	unordered_map<string, LabelInfo> labelsMap;

	unordered_map<string, int32_t>::iterator PcReg; // "points to"/contains the current instruction being simulated in the vector of programInstructions. this Iterator should not move

	bool isErrorStatus;
	bool isEndOfFile;
	bool isControlInstr;
	
	void update_simulatorInterface(string userInput); // This is the Simulators Interface. Later I will do inheritance with another class to make the GUI interface
	void update_simulation();

	ThreadSafeMessageQueue<string> myMsgQueue; // This is the queue that could possibly contain the string "run" based on wether the simulator is running or not
	std::thread thread1; // Creating an empty thread variable that will be later assigned to a function to perform the run/break operation in parallel
	bool parallelHasNotQuit;

	void runParallelProcess();

public:
	Simulator(const unordered_map<string, int32_t> regMap, const vector<string> memory, const vector<instructionType> instructions, const unordered_map<string, LabelInfo> labelMap); // Constructor
	~Simulator(); // Deconstructor

	void setStartInstrIndex(); // This sets up and assigns the $pc register to the index where the main label is located
	void update_pcReg(); // Performed right after updating the simulation
	bool isEndOfFileCheck(); // Performed right after updating the pc register

	void update_simulation_public(); // A public version for updating the simulation. Performs the actual instruction
	//return the memoryAddress map and the register map to be displayed on the GUI
	bool getErrorStatus();
	int32_t getPcValue();
	vector<string> getMemoryLayout();
	unordered_map<string, int32_t> getRegisterMap();

	void simulate(); 
	void clearDataStruct();

	void runOrBreak_public(string command); // This function is called when I want to run or break the program, and is public so I can call it in the virtualMachineGui class
};



#endif // !SIMULATOR_HPP
