#include <QTest>
#include <QPlainTextEdit>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>

#include "virtual_machine_gui.hpp"

#include "test_config.hpp"

class VirtualMachineGUITest : public QObject {
  Q_OBJECT

private slots:

  void initTestCase();
  // TODO: implement additional tests here
  void invalidFileCase();
  void firstTestCase();
  void testCase18();
  void testHighlight();
  
private:

  VirtualMachineGUI widget;
};

// this section just verifies the object names and API
void VirtualMachineGUITest::initTestCase(){

  widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test00.asm")); 

  auto textWidget = widget.findChild<QPlainTextEdit *>("text");
  QVERIFY2(textWidget, "Could not find QPLainText widget");

  auto registerViewWidget = widget.findChild<QTableView *>("registers");
  QVERIFY2(registerViewWidget, "Could not find QTableView widget for registers");

  auto memoryViewWidget = widget.findChild<QTableView *>("memory");
  QVERIFY2(memoryViewWidget, "Could not find QTableView widget for memory");

  auto statusViewWidget = widget.findChild<QLineEdit *>("status");
  QVERIFY2(statusViewWidget, "Could not find QLineEdit widget for status");

  auto stepButtonWidget = widget.findChild<QPushButton *>("step");
  QVERIFY2(stepButtonWidget, "Could not find QTableView widget for memory");
}

void VirtualMachineGUITest::invalidFileCase() {

	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test55.asm"));

	auto textWidget = widget.findChild<QPlainTextEdit *>("text");
	QVERIFY2(textWidget, "Could not find QPLainText widget");

	auto registerViewWidget = widget.findChild<QTableView *>("registers");
	QVERIFY2(registerViewWidget, "Could not find QTableView widget for registers");

	auto memoryViewWidget = widget.findChild<QTableView *>("memory");
	QVERIFY2(memoryViewWidget, "Could not find QTableView widget for memory");

	auto statusViewWidget = widget.findChild<QLineEdit *>("status");
	QVERIFY2(statusViewWidget, "Could not find QLineEdit widget for status");

	auto stepButtonWidget = widget.findChild<QPushButton *>("step");
	QVERIFY2(stepButtonWidget, "Could not find QTableView widget for memory");
}

void VirtualMachineGUITest::firstTestCase() {

	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test01.asm")); // This is test00.asm

	auto textWidget = widget.findChild<QPlainTextEdit *>("text");
	QVERIFY2(textWidget, "Could not find QPLainText widget");

	auto registerViewWidget = widget.findChild<QTableView *>("registers");
	QVERIFY2(registerViewWidget, "Could not find QTableView widget for registers");

	auto memoryViewWidget = widget.findChild<QTableView *>("memory");
	QVERIFY2(memoryViewWidget, "Could not find QTableView widget for memory");

	auto statusViewWidget = widget.findChild<QLineEdit *>("status");
	QVERIFY2(statusViewWidget, "Could not find QLineEdit widget for status");

	auto stepButtonWidget = widget.findChild<QPushButton *>("step");
	
	auto model = widget.findChild<QAbstractTableModel *>("regModel");
	
	stepButtonWidget->click();
	stepButtonWidget->click();
	stepButtonWidget->click();
	stepButtonWidget->click();
	stepButtonWidget->click();
	QVERIFY2(stepButtonWidget, "Could not find QTableView widget for memory");
}


void VirtualMachineGUITest::testCase18() {

	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test18.asm")); // This is test00.asm

	auto textWidget = widget.findChild<QPlainTextEdit *>("text");
	QVERIFY2(textWidget, "Could not find QPLainText widget");

	auto registerViewWidget = widget.findChild<QTableView *>("registers");
	QVERIFY2(registerViewWidget, "Could not find QTableView widget for registers");

	auto memoryViewWidget = widget.findChild<QTableView *>("memory");
	QVERIFY2(memoryViewWidget, "Could not find QTableView widget for memory");

	auto statusViewWidget = widget.findChild<QLineEdit *>("status");
	QVERIFY2(statusViewWidget, "Could not find QLineEdit widget for status");

	auto stepButtonWidget = widget.findChild<QPushButton *>("step"); // This allows me to go through the code where I require the highlighter to move up the file
	stepButtonWidget->click();
	stepButtonWidget->click();
	stepButtonWidget->click();
	stepButtonWidget->click();
	stepButtonWidget->click();
	stepButtonWidget->click();
	stepButtonWidget->click();
	QVERIFY2(stepButtonWidget, "Could not find QTableView widget for memory");
}

void VirtualMachineGUITest::testHighlight() {

	widget.load(QString::fromStdString(TEST_FILE_DIR + "/vm/test03.asm")); // This is test00.asm

	auto textWidget = widget.findChild<QPlainTextEdit *>("text");
	QVERIFY2(textWidget, "Could not find QPLainText widget");

	auto registerViewWidget = widget.findChild<QTableView *>("registers");
	QVERIFY2(registerViewWidget, "Could not find QTableView widget for registers");

	auto memoryViewWidget = widget.findChild<QTableView *>("memory");
	QVERIFY2(memoryViewWidget, "Could not find QTableView widget for memory");

	auto statusViewWidget = widget.findChild<QLineEdit *>("status");
	QVERIFY2(statusViewWidget, "Could not find QLineEdit widget for status");

	auto stepButtonWidget = widget.findChild<QPushButton *>("step"); // This allows me to go through the code where I require the highlighter to move up the file
	stepButtonWidget->click();
	widget.testingHighlight_public();
	
	QVERIFY2(stepButtonWidget, "Could not find QTableView widget for memory");
}

QTEST_MAIN(VirtualMachineGUITest)
#include "virtual_machine_gui_test.moc"


