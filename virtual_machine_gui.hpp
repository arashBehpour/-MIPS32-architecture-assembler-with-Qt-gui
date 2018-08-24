#ifndef VIRTUAL_MACHINE_GUI_HPP
#define VIRTUAL_MACHINE_GUI_HPP

#include <QString>
#include <QWidget>
#include <QMainWindow>
#include <QDebug>
#include <QLayout>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QAbstractTableModel>
#include <QTextBlock>

#include "parser.hpp"
#include "simulator.hpp"

#include "fstream" // this is to create the ifstream object
#include <cstdlib> 
#include <iostream>
#include <sstream>


// The class below is a custom AbstractTableModel for my QTableView objects to use
// Referenced website: http://programmingexamples.net/wiki/Qt/ModelView/QAbstractTableModel
class myQtTableModel : public QAbstractTableModel
{
public:
	myQtTableModel(string memOrReg, unordered_map<string, int32_t> regLayout, vector<string> memLayout); // When I construct my qtTableModel it will differ depending on if I want to construct the memoryLayout or registerLayout
	myQtTableModel(string memOrReg);

	int rowCount(const QModelIndex& parent) const;
	int columnCount(const QModelIndex& parent) const;
	QVariant data(const QModelIndex& index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role);

	void updateModel(unordered_map<string, int32_t> regLayout, vector<string> memLayout, string regOrMemModel);

protected:
	QVector<QVector<QString>> COLumns;
	string memOrRg;

	void helper_loadRegValues(unordered_map<string, int32_t> regLayout, QVector<QString>& column3);
};


//*************************************************************************************************************************************************//
//*************************************************************************************************************************************************//
//*************************************************************************************************************************************************//
//*************************************************************************************************************************************************//
//*************************************************************************************************************************************************//


// TODO define the GUI class
// The class below is the VirtualMachineGUI class

class VirtualMachineGUI: public QWidget{
	Q_OBJECT
private:
	QMainWindow *mainWindow;
	QWidget *containerWidget;
	QVBoxLayout *vlayout;
	QHBoxLayout *hlayoutForTable;
	QHBoxLayout *hlayoutForStatus;
	QHBoxLayout *hlayoutForButtons;

	QPlainTextEdit *text;
	QTableView *registers;
	myQtTableModel *registersModel; // custom QtAbstractTableModel
	QTableView *memory;
	myQtTableModel *memoryModel;  // custom QtAbstractTableModel
	QLabel *statusLabel;
	QLineEdit *status;
	QPushButton *step;
	QPushButton *run;
	QPushButton *breakRun;

	QTextCursor cursor; // Will keep track of the cursor to display the highlighted line

	size_t initMainLine; // This will use the line number from the lexer to store where the main: label is located
	int32_t prevSumOfInitMain__pcReg; // I will use to tell to what instruction I need to jump too and highlight
	bool errorOpeningFile;
	bool hasBreaked; //This will be used to help the highlight() function to do a certain process while running. So if it has breaked then accomadate the highlighter to whereever program counter I stopped at

	QString fileName; // This is to open and store the file in the plain text edit
	Simulator *vmpointer; 
	
	void createPlainTextWidget();
	void constructModel();
	void errorModel();
	void highlightText();
	
	//Helper Functions
	void colorWhite();
	void colorYellow();
	void updateModelView();
	void enableAndDisableView(bool boolValue);

private slots:
	void stepSlot();
	void runSlot();
	void breakSlot();

public:
	VirtualMachineGUI();
	~VirtualMachineGUI();

	void load(QString filename);
	void testingHighlight_public();
	void guiShow(); // This function is to display the initially constructed gui contained in the mainWindow widget
};
#endif

