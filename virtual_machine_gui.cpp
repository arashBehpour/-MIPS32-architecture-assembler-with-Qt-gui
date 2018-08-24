#include "virtual_machine_gui.hpp"

//**********************************************Implementation of class myQtTableModel : public QAbstractTableModel********************************//
//***************************************************************************_*********************************************************************//
// IF I HAVE TIME ADD INDEX ACCESS SAFTEY MEASURES!!!!!!!

myQtTableModel::myQtTableModel(string memOrReg)
{
	memOrRg = memOrReg;
	if (memOrReg == "regLayout") {
		QVector<QString> column1;
		column1.push_back("");
		column1.push_back("");
		column1.push_back("");

		for (size_t i = 0; i < 32; i++) {
			QString moneySign = QString("$"); // column for Actual Register numbers
			string tempStr = std::to_string(i);
			QString temp = QString::fromStdString(tempStr);
			column1.push_back(moneySign.append(temp));
		}
		COLumns.push_back(column1);

		QVector<QString> column2; // column for Aliases
		column2.push_back("$pc");
		column2.push_back("$hi");
		column2.push_back("$lo");

		column2.push_back("$zero");
		column2.push_back("$at");
		column2.push_back("$v0");
		column2.push_back("$v1");
		column2.push_back("$a0");
		column2.push_back("$a1");
		column2.push_back("$a2");
		column2.push_back("$a3");


		for (size_t i = 0; i < 8; i++) {
			QString tSign = QString("$t");
			string tempStr = std::to_string(i);
			QString temp = QString::fromStdString(tempStr);
			column2.push_back(tSign.append(temp));
		}
		for (size_t i = 0; i < 8; i++) {
			QString sSign = QString("$s");
			string tempStr = std::to_string(i);
			QString temp = QString::fromStdString(tempStr);
			column2.push_back(sSign.append(temp));
		}
		column2.push_back("$t8");
		column2.push_back("$t9");
		column2.push_back("$k0");
		column2.push_back("$k1");
		column2.push_back("$gp");
		column2.push_back("$sp");
		column2.push_back("$fp");
		column2.push_back("$ra");
		COLumns.push_back(column2);

		QVector<QString> column3; // column for Register contents

		for (size_t i = 0; i < 35; i++) {
			column3.push_back(QString("0x00000000"));
		}

		COLumns.push_back(column3);

	}
	else if (memOrReg == "memLayout") {
		QVector<QString> column1; // Column
		for (size_t i = 0; i < 512; i++) {
			int temp = (int)i;
			QString initMemAddress = QString("0x%1").arg(temp, 8, 16, QLatin1Char('0'));
			column1.push_back(initMemAddress);
		}
		COLumns.push_back(column1);


		QVector<QString> column2;
		for (size_t i = 0; i < 512; i++) {
			column2.push_back(QString("0x00"));
		}
		COLumns.push_back(column2);
	}
	else {
		//default empty columns
		//qDebug() << "Error: Only set up to create a table model for reg/mem" << endl;
	}
}


myQtTableModel::myQtTableModel(string memOrReg, unordered_map<string, int32_t> regLayout, vector<string> memLayout) //: QAbstractTableModel()
{
	memOrRg = memOrReg; // Saves if it was a register or memory model table constructed
	if (memOrReg == "regLayout") {
		QVector<QString> column1;
		column1.push_back("");
		column1.push_back("");
		column1.push_back("");

		for (size_t i = 0; i < 32; i++) {
			QString moneySign = QString("$"); // column for Actual Register numbers
			string tempStr = std::to_string(i);
			QString temp = QString::fromStdString(tempStr);
			column1.push_back(moneySign.append(temp));
		}
		COLumns.push_back(column1);

		QVector<QString> column2; // column for Aliases
		column2.push_back("$pc");
		column2.push_back("$hi");
		column2.push_back("$lo");

		column2.push_back("$zero");
		column2.push_back("$at");
		column2.push_back("$v0");
		column2.push_back("$v1");
		column2.push_back("$a0");
		column2.push_back("$a1");
		column2.push_back("$a2");
		column2.push_back("$a3");

		
		for (size_t i = 0; i < 8; i++) {
			QString tSign = QString("$t");
			string tempStr = std::to_string(i);
			QString temp = QString::fromStdString(tempStr);
			column2.push_back(tSign.append(temp));
		}
		for (size_t i = 0; i < 8; i++) {
			QString sSign = QString("$s");
			string tempStr = std::to_string(i);
			QString temp = QString::fromStdString(tempStr);
			column2.push_back(sSign.append(temp));
		}
		column2.push_back("$t8");
		column2.push_back("$t9");
		column2.push_back("$k0");
		column2.push_back("$k1");
		column2.push_back("$gp");
		column2.push_back("$sp");
		column2.push_back("$fp");
		column2.push_back("$ra");
		COLumns.push_back(column2);

		QVector<QString> column3; // column for Register contents
		
		helper_loadRegValues(regLayout, column3); 

		COLumns.push_back(column3);

	}
	else if (memOrReg == "memLayout") {
		QVector<QString> column1; // Column
		for (size_t i = 0; i < memLayout.size(); i++) {
			int temp = (int)i;
			QString initMemAddress = QString("0x%1").arg(temp, 8, 16, QLatin1Char('0'));
			column1.push_back(initMemAddress);
		}
		COLumns.push_back(column1);


		QVector<QString> column2;
		for (size_t i = 0; i < memLayout.size(); i++) { 
			QString temp = QString::fromStdString(("0x" + memLayout[i]));
			column2.push_back(temp);
		}

		COLumns.push_back(column2);
	}
	else {
		//default empty columns
		qDebug() << "Error: Only set up to create a table model for reg/mem" << endl;
	}

}

int myQtTableModel::rowCount(const QModelIndex & parent) const
{
	return COLumns[0].size();
}

int myQtTableModel::columnCount(const QModelIndex & parent) const
{
	return COLumns.size();;
}

QVariant myQtTableModel::data(const QModelIndex& index, int role) const // Value of int role = 0, for The key data to be rendered in the form of text(QString)
{
	if (role == Qt::DisplayRole) // Returns the data stored under the given role for the item referred to by the index.
	{
		return COLumns[index.column()][index.row()];
	}
	return QVariant::Invalid; // If you do not have a value to return, return an invalid QVariant instead of returning 0.
}

bool myQtTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::EditRole) {
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

void myQtTableModel::updateModel(unordered_map<string, int32_t> regLayout, vector<string> memLayout, string regOrMemModel)
{
	QVector<QString> lastColumnEdit;
	if (regOrMemModel == "memoryModel") {

		for (size_t i = 0; i < memLayout.size(); i++) { // This for loop will be used in the detData() function as well
			QString temp = QString::fromStdString(("0x" + memLayout[i]));
			lastColumnEdit.push_back(temp);
		}
		COLumns[1] = lastColumnEdit; // for memoryModel
	}
	else if (regOrMemModel == "registerModel") {

		helper_loadRegValues(regLayout, lastColumnEdit);
		COLumns[2] = lastColumnEdit; // for registerModel
	}
}

QVariant myQtTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// Returns the data for the given role and section in the header with the specified orientation.
	// For horizontal headers, the section number corresponds to the column number
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {

		if (section == 0) {
			if (memOrRg == "regLayout")
				return QString("Number");
			else if (memOrRg == "memLayout")
				return QString("Address (Hex)");
		}
		else if (section == 1) {
			if (memOrRg == "regLayout")
				return QString("Alias");
			else if (memOrRg == "memLayout")
				return QString("Value (Hex)");
		}
		else if (section == 2) {
			if (memOrRg == "regLayout")
				return QString("Value (Hex)");
		}
	}
	return QVariant::Invalid;
}

void myQtTableModel::helper_loadRegValues(unordered_map<string, int32_t> regLayout, QVector<QString>& column3)
{
	QString initRegContents;

	auto itPc = regLayout.find("$pc");
	initRegContents = QString("0x%1").arg((int)(itPc->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itHi = regLayout.find("$hi");
	initRegContents = QString("0x%1").arg((int)(itHi->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itLo = regLayout.find("$lo");
	initRegContents = QString("0x%1").arg((int)(itLo->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR0 = regLayout.find("$0");
	initRegContents = QString("0x%1").arg((int)(itR0->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR1 = regLayout.find("$1");
	initRegContents = QString("0x%1").arg((int)(itR1->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR2 = regLayout.find("$2");
	initRegContents = QString("0x%1").arg((int)(itR2->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR3 = regLayout.find("$3");
	initRegContents = QString("0x%1").arg((int)(itR3->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR4 = regLayout.find("$4");
	initRegContents = QString("0x%1").arg((int)(itR4->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR5 = regLayout.find("$5");
	initRegContents = QString("0x%1").arg((int)(itR5->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR6 = regLayout.find("$6");
	initRegContents = QString("0x%1").arg((int)(itR6->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR7 = regLayout.find("$7");
	initRegContents = QString("0x%1").arg((int)(itR7->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR8 = regLayout.find("$8");
	initRegContents = QString("0x%1").arg((int)(itR8->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR9 = regLayout.find("$9");
	initRegContents = QString("0x%1").arg((int)(itR9->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR10 = regLayout.find("$10");
	initRegContents = QString("0x%1").arg((int)(itR10->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR11 = regLayout.find("$11");
	initRegContents = QString("0x%1").arg((int)(itR11->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR12 = regLayout.find("$12");
	initRegContents = QString("0x%1").arg((int)(itR12->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR13 = regLayout.find("$13");
	initRegContents = QString("0x%1").arg((int)(itR13->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR14 = regLayout.find("$14");
	initRegContents = QString("0x%1").arg((int)(itR14->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR15 = regLayout.find("$15");
	initRegContents = QString("0x%1").arg((int)(itR15->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR16 = regLayout.find("$16");
	initRegContents = QString("0x%1").arg((int)(itR16->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR17 = regLayout.find("$17");
	initRegContents = QString("0x%1").arg((int)(itR17->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR18 = regLayout.find("$18");
	initRegContents = QString("0x%1").arg((int)(itR18->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR19 = regLayout.find("$19");
	initRegContents = QString("0x%1").arg((int)(itR19->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR20 = regLayout.find("$20");
	initRegContents = QString("0x%1").arg((int)(itR20->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR21 = regLayout.find("$21");
	initRegContents = QString("0x%1").arg((int)(itR21->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR22 = regLayout.find("$22");
	initRegContents = QString("0x%1").arg((int)(itR22->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR23 = regLayout.find("$23");
	initRegContents = QString("0x%1").arg((int)(itR23->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR24 = regLayout.find("$24");
	initRegContents = QString("0x%1").arg((int)(itR24->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR25 = regLayout.find("$25");
	initRegContents = QString("0x%1").arg((int)(itR25->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR26 = regLayout.find("$26");
	initRegContents = QString("0x%1").arg((int)(itR26->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR27 = regLayout.find("$27");
	initRegContents = QString("0x%1").arg((int)(itR27->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR28 = regLayout.find("$28");
	initRegContents = QString("0x%1").arg((int)(itR28->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR29 = regLayout.find("$29");
	initRegContents = QString("0x%1").arg((int)(itR29->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR30 = regLayout.find("$30");
	initRegContents = QString("0x%1").arg((int)(itR30->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
	auto itR31 = regLayout.find("$31");
	initRegContents = QString("0x%1").arg((int)(itR31->second), 8, 16, QLatin1Char('0'));
	column3.push_back(initRegContents);
}


//***************************************************************************************************************************************\\
//***************************************************************************************************************************************\\
//***************************************************************************************************************************************\\
//***************************************************************************************************************************************\\
//***************************************************************************************************************************************\\
//***************************************************************************************************************************************\\
//***************************************************************************************************************************************\\



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------Helper Functions--------------------------------------------------------------//
void VirtualMachineGUI::colorWhite()
{
	QTextBlockFormat preExecLineBlock;
	preExecLineBlock.setBackground(Qt::white);

	cursor.select(QTextCursor::LineUnderCursor);
	cursor.setBlockFormat(preExecLineBlock);
	text->setTextCursor(cursor);
}

void VirtualMachineGUI::colorYellow()
{
	QTextBlockFormat execLineBlock;
	execLineBlock.setBackground(Qt::yellow);

	cursor.select(QTextCursor::LineUnderCursor);
	cursor.setBlockFormat(execLineBlock);
	cursor.clearSelection();
	text->setTextCursor(cursor);
}


// TODO implement the GUI class

VirtualMachineGUI::VirtualMachineGUI()
{
	errorOpeningFile = false;
	hasBreaked = false;

	mainWindow = new QMainWindow(this); // Creates the main window that will contain all the Gui widgets
	containerWidget = new QWidget();
	vlayout = new QVBoxLayout();
	hlayoutForTable = new QHBoxLayout();
	hlayoutForStatus = new QHBoxLayout();
	hlayoutForButtons = new QHBoxLayout();


	text = new QPlainTextEdit(); // This is a widget for the plain text assembly file
	text->setObjectName("text");
	text->setReadOnly(true);

	registers = new QTableView();
	registers->setObjectName("registers");
	registers->setStyleSheet("QHeaderView::section { background-color:lightgray }");

	memory = new QTableView();
	memory->setObjectName("memory");
	memory->setStyleSheet("QHeaderView::section { background-color:lightgray }");

	statusLabel = new QLabel("Status:");
	status = new QLineEdit();
	status->setReadOnly(true);
	status->setObjectName("status");

	step = new QPushButton("Step");
	step->setObjectName("step");

	run = new QPushButton("Run");
	run->setObjectName("run");

	breakRun = new QPushButton("Break");
	breakRun->setObjectName("break");
	breakRun->setEnabled(false); // Initially disabled until run is pressed

	mainWindow->setCentralWidget(containerWidget);
	containerWidget->setLayout(vlayout);
		
	vlayout->addLayout(hlayoutForTable);

	hlayoutForTable->addWidget(text);
	hlayoutForTable->addWidget(registers);
	hlayoutForTable->addWidget(memory);

	vlayout->addLayout(hlayoutForStatus);

	hlayoutForStatus->addWidget(statusLabel);
	hlayoutForStatus->addWidget(status);

	vlayout->addLayout(hlayoutForButtons);
	
	hlayoutForButtons->addWidget(step);
	hlayoutForButtons->addWidget(run);
	hlayoutForButtons->addWidget(breakRun);
	
	connect(step, SIGNAL(clicked()), this, SLOT(stepSlot()));	// Everytime I step will call the slot function stepSlot() to update_simulation()
	connect(run, SIGNAL(clicked()), this, SLOT(runSlot()));
	connect(breakRun, SIGNAL(clicked()), this, SLOT(breakSlot()));
}

VirtualMachineGUI::~VirtualMachineGUI()
{
	/*if (errorOpeningFile == false) {

		vmpointer->clearDataStruct();
		delete vmpointer;
		vmpointer = nullptr;
	}*/
}

void VirtualMachineGUI::createPlainTextWidget()
{
	QFile qFileName(fileName);
	qFileName.open(QFile::ReadOnly | QFile::Text);
	QTextStream qFileNameStream(&qFileName);
	text->setPlainText(qFileNameStream.readAll());

	
	cursor = text->textCursor(); // Sets the cursor to be in the beginning of the file
	for (size_t i = 0; i < initMainLine; i++) { 
		cursor.movePosition(QTextCursor::NextBlock); 
	}
	
	// This is the initial highlight
	colorYellow();
}

void VirtualMachineGUI::testingHighlight_public()
{
	//vmpointer->update_simulation_public();
	highlightText();
}

void VirtualMachineGUI::highlightText()
{
	if ((vmpointer->isEndOfFileCheck() == false) || ((vmpointer->isEndOfFileCheck() == true) && hasBreaked == true) ) {

		colorWhite();

		//Highlight algorithim for branching: depending on the difference from previous line and current line will determine if the
		//									 highlighter will move upward or downward. Also depending on which direction I am moving
		//									 I will skip the empty lines accordingly
		int32_t curSumOfInitMain_pcReg = (int32_t)initMainLine + (vmpointer->getPcValue()); // This is the line I need to get too
		int32_t lineDifference = curSumOfInitMain_pcReg - prevSumOfInitMain__pcReg;
		//cout << "difference" << lineDifference << endl;

		if (lineDifference > 0) {

			if ((vmpointer->isEndOfFileCheck() == true) && hasBreaked == true) // FOR THE RUN CASE!
				lineDifference = lineDifference - 1; // This is (-1) in the run case because when i run I will be already at the endOFFIle=true but in any NON-run case I would not have reached endOfFile=false

			for (int32_t i = 0; i < lineDifference; i++) {
				cursor.movePosition(QTextCursor::NextBlock); // Moves the cursor to the next block(line)

				// Begin: This is to skip empty lines,<labels:>, and comments, in the plain text file <labels:>
				QTextCursor beginCursor = cursor;
				QTextCursor endCursor = cursor;
				beginCursor.movePosition(QTextCursor::StartOfLine);
				endCursor.movePosition(QTextCursor::EndOfLine);


				QTextCursor labelCheckCursor = cursor;
				labelCheckCursor.setPosition(beginCursor.position(), QTextCursor::MoveAnchor);
				labelCheckCursor.setPosition(endCursor.position(), QTextCursor::KeepAnchor);
				QString cursorString = labelCheckCursor.selectedText();
				QChar lastCursorStringChar = 'X'; // This can be any character except ':' because this is a dummy character used to nullify last case in while loop
				QChar firstCursorStringChar = 'X';

				if (cursorString.length() > 0) {
					lastCursorStringChar = cursorString[cursorString.length() - 1];
					cursorString.remove(' ');
					firstCursorStringChar = cursorString[0];
				}

				while (beginCursor.position() == endCursor.position() || lastCursorStringChar == ':' || firstCursorStringChar == '#') {
					cursor.movePosition(QTextCursor::NextBlock); // Highlights the next line yellow

					beginCursor.movePosition(QTextCursor::NextBlock);
					endCursor.movePosition(QTextCursor::NextBlock);

					beginCursor.movePosition(QTextCursor::StartOfLine);
					endCursor.movePosition(QTextCursor::EndOfLine);


					labelCheckCursor.setPosition(beginCursor.position(), QTextCursor::MoveAnchor);
					labelCheckCursor.setPosition(endCursor.position(), QTextCursor::KeepAnchor);
					cursorString = labelCheckCursor.selectedText();
					if (cursorString.length() > 0) {
						lastCursorStringChar = cursorString[cursorString.length() - 1];
						cursorString.remove(' ');
						firstCursorStringChar = cursorString[0];
					}
					else {
						lastCursorStringChar = 'X';
						firstCursorStringChar = 'X';
					}
				} // End: algorithim to skip empty lines,labels and comments(#)


			}
		}
		else if (lineDifference < 0) {

			lineDifference = lineDifference * (-1);
			for (int32_t i = 0; i < lineDifference; i++) {
				cursor.movePosition(QTextCursor::PreviousBlock); // Moves the cursor to the previous block(line)

				// Begin: This is to skip empty lines, <Labels:>, comments in the plain text file 
				QTextCursor beginCursor = cursor;
				QTextCursor endCursor = cursor;
				beginCursor.movePosition(QTextCursor::StartOfLine);
				endCursor.movePosition(QTextCursor::EndOfLine);

				QTextCursor labelCheckCursor = cursor;
				labelCheckCursor.setPosition(beginCursor.position(), QTextCursor::MoveAnchor);
				labelCheckCursor.setPosition(endCursor.position(), QTextCursor::KeepAnchor);
				QString cursorString = labelCheckCursor.selectedText();
				QChar lastCursorStringChar = 'X'; // a Dummy character used to nullify the last case in the while loop
				QChar firstCursorStringChar = 'X';

				if (cursorString.length() > 0) {
					lastCursorStringChar = cursorString[cursorString.length() - 1]; // Assigning the last character of the
					cursorString.remove(' ');
					firstCursorStringChar = cursorString[0];
				}

				while (beginCursor.position() == endCursor.position() || lastCursorStringChar == ':' || firstCursorStringChar == '#') {
					cursor.movePosition(QTextCursor::PreviousBlock);

					beginCursor.movePosition(QTextCursor::PreviousBlock);
					endCursor.movePosition(QTextCursor::PreviousBlock);

					beginCursor.movePosition(QTextCursor::StartOfLine);
					endCursor.movePosition(QTextCursor::EndOfLine);

					labelCheckCursor.setPosition(beginCursor.position(), QTextCursor::MoveAnchor);
					labelCheckCursor.setPosition(endCursor.position(), QTextCursor::KeepAnchor);
					cursorString = labelCheckCursor.selectedText();
					if (cursorString.length() > 0) {
						lastCursorStringChar = cursorString[cursorString.length() - 1];
						cursorString.remove(' ');
						firstCursorStringChar = cursorString[0];
					}
					else {
						lastCursorStringChar = 'X';
						firstCursorStringChar = 'X';
					}
				} // End: algorithim to skip empty lines, <labels:> and comments(#)
			} // End: for loop for moving the cursor to the corresponding line
		}// End: of if statement to move cursor upwards

	
		prevSumOfInitMain__pcReg = curSumOfInitMain_pcReg; // This saves the sum of (where the main starts) and the (location of the next instruction or pcReg value)
	
		colorYellow();
	}
	//else if ((vmpointer->isEndOfFileCheck() == true) && hasBreaked == true) {
	//	
	//	colorWhite();

	//	int32_t curSumOfInitMain_pcReg = (int32_t)initMainLine + (vmpointer->getPcValue()); // This is the line I need to get too
	//	cout << "initial main line#: " << initMainLine << endl;
	//	cout << "pc reg: " << (vmpointer->getPcValue()) << endl;
	//	
	//	for (int32_t i = prevSumOfInitMain__pcReg; i < curSumOfInitMain_pcReg - 1; i++)
	//		cursor.movePosition(QTextCursor::NextBlock);
	//	

	//	prevSumOfInitMain__pcReg = curSumOfInitMain_pcReg; // This saves the sum of (where the main starts) and the (location of the next instruction or pcReg value)

	//	colorYellow();
	//}

	text->setTextCursor(cursor);
}


void VirtualMachineGUI::load(QString filename)
{
	fileName = filename; // This assigns the filename path to the private Qstring variable  
	string filenameStr = filename.toStdString(); 
	ifstream assemblyFile(filenameStr); // constructs an ifstream object using the file path 
	
	if (!assemblyFile.fail()) {

		TokenList assemblyTokenList = tokenize(assemblyFile);

		if (assemblyTokenList.back().type() == ERROR) {
			status->setText("Error: Failed to Tokenize .asm file");
		}
		else {

			Parser mipsParser(assemblyTokenList); // This constructs a parser called mipsParser that takes the token list generated by the lexer
			mipsParser.parseFile();

			if (mipsParser.successfullyParsed() == false) {
				status->setText("Error: Failed to Parse .asm file");
			}
			else {
				vmpointer = new Simulator(mipsParser.getInitialRegisterContents(), mipsParser.getInitialMemoryLayout(), mipsParser.getProgramInstructions(), mipsParser.getLabelsMap()); // Constructs the basic simulator parameters
				
				unordered_map<string, LabelInfo> labelMap = mipsParser.getLabelsMap(); // This is to make sure there is a main in the labels map
				unordered_map<string, LabelInfo>::iterator findMain = labelMap.find("main"); // The program counter starts at the main label, so want to find the label name "main"

				if (findMain == labelMap.end())
					status->setText("Error: Failed to find a main: label in the .asm file");
				else {
					// I need to find what line number does main appear in the .asm file and assign it to my private variable initMainLine
					for (auto itMain = assemblyTokenList.begin(); itMain != assemblyTokenList.end(); ++itMain) {
						if ((itMain->value()) == "main:") {
							initMainLine = (itMain->line());
							prevSumOfInitMain__pcReg = initMainLine; // Initially PcReg = 0 so the sum is equal to the initial line number in main
							break;
						}
					}

					vmpointer->setStartInstrIndex();
					createPlainTextWidget(); 
					status->setText("Ok");
					
					constructModel();
				}
			}
		}
	}
	else {
		// Here I would create the error model to be displayed, EDIT model to fit an error case
		errorOpeningFile = true;
		errorModel();
		status->setText("Error: Failed to open file");
	}
}


void VirtualMachineGUI::constructModel()
{
	registersModel = new myQtTableModel("regLayout", (vmpointer->getRegisterMap()), (vmpointer->getMemoryLayout()) );
	registersModel->setObjectName("regModel");
	memoryModel = new myQtTableModel("memLayout", (vmpointer->getRegisterMap()), (vmpointer->getMemoryLayout()) );
	memoryModel->setObjectName("memModel");

	registers->setModel(registersModel);
	registers->setColumnWidth(0, 140); // 140 is the best number to fit all three columns from the registerModel
	registers->setColumnWidth(1, 140);
	registers->setColumnWidth(2, 140);

	memory->setModel(memoryModel);
	memory->setColumnWidth(0, 140); // 140 is the best number to fit all 2 columns from the memoryModel and leave an additional empty "column"
	memory->setColumnWidth(1, 140);
}

void VirtualMachineGUI::errorModel()
{
	registersModel = new myQtTableModel("regLayout");
	registersModel->setObjectName("regModel");
	memoryModel = new myQtTableModel("memLayout");
	memoryModel->setObjectName("memModel");

	registers->setModel(registersModel);
	registers->setColumnWidth(0, 140); // 140 is the best number to fit all three columns from the registerModel
	registers->setColumnWidth(1, 140);
	registers->setColumnWidth(2, 140);

	memory->setModel(memoryModel);
	memory->setColumnWidth(0, 140); // 140 is the best number to fit all 2 columns from the memoryModel and leave an additional empty "column"
	memory->setColumnWidth(1, 140);
}

void VirtualMachineGUI::updateModelView()
{
	QModelIndex dummyIndex;
	QVariant dummyMemQvariant = QVariant(tr("memoryModel"));
	QVariant dummyRegQvariant = QVariant(tr("registerModel"));

	registersModel->updateModel((vmpointer->getRegisterMap()), (vmpointer->getMemoryLayout()), "registerModel");
	memoryModel->updateModel((vmpointer->getRegisterMap()), (vmpointer->getMemoryLayout()), "memoryModel");

	registersModel->setData(dummyIndex, dummyRegQvariant, Qt::EditRole); // Only thing I use setData() for is to emit the dataChanged() signal
	memoryModel->setData(dummyIndex, dummyMemQvariant, Qt::EditRole);
}

void VirtualMachineGUI::enableAndDisableView(bool boolValue)
{
	bool oppBoolValue;
	if (boolValue == true)
		oppBoolValue = false;
	else
		oppBoolValue = true;

	breakRun->setEnabled(boolValue);
	run->setEnabled(oppBoolValue);
	step->setEnabled(oppBoolValue);

	text->setEnabled(oppBoolValue);
	registers->setEnabled(oppBoolValue);
	memory->setEnabled(oppBoolValue);
	status->setEnabled(oppBoolValue);
}

void VirtualMachineGUI::stepSlot()
{
	if (errorOpeningFile == false)
	{
		// GUI will update based on if the user has clicked the step button, and the register/memory contents will update accordingly
		if (((vmpointer->isEndOfFileCheck()) == false)) {
			vmpointer->update_simulation_public(); // This updates the virtual machine and its values
			vmpointer->update_pcReg();

			updateModelView();

			highlightText();
		}
		else {
			status->setText("Error: reached the end of the file, can not Step anymore.");
		}
		vmpointer->isEndOfFileCheck();
	}
}

void VirtualMachineGUI::runSlot()
{
	if (errorOpeningFile == false) {

		if (((vmpointer->isEndOfFileCheck()) == false)) {
			
			enableAndDisableView(true);

			vmpointer->runOrBreak_public("run"); // Will keep stepping until I reached the end of the file or if I press the break button
		}
		else {
			status->setText("Error: reached the end of the file, can not run the program.");
		}
		vmpointer->isEndOfFileCheck();
	}

}

void VirtualMachineGUI::breakSlot()
{
	if (errorOpeningFile == false) {

		vmpointer->runOrBreak_public("break"); // Will keep stepping until I reached the end of the file or if I press the break button

		enableAndDisableView(false);

		updateModelView();

		hasBreaked = true;
		highlightText();
		hasBreaked = false;

		if(vmpointer->isEndOfFileCheck() == true)
			status->setText("Error: reached the end of the file, can not run the program.");
	}

}

void VirtualMachineGUI::guiShow()
{
	mainWindow->showMaximized();
}


