#include <cbeditor.h>
#include <cbproject.h>
#include <cbstyledtextctrl.h>
#include <configmanager.h>
#include <configurationpanel.h>
#include <editormanager.h>
#include <logmanager.h>
#include <macrosmanager.h>
#include <pluginmanager.h>
#include <projectmanager.h>
#include <sdk.h> // Code::Blocks SDK
#include <sdk_events.h>

#include <wx/process.h>

#include "OutputReader.h"
#include "TestOutputLog.h"
#include "UnitTest.h"
#include "UnitTestProcess.h"
#include "Utility.h"

using namespace std;

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
PluginRegistrant<UnitTest> reg(_T("UnitTest"));
}

// events handling
BEGIN_EVENT_TABLE(UnitTest, cbPlugin)
// add any events you want to handle here
END_EVENT_TABLE()

// constructor
UnitTest::UnitTest() : m_Process(nullptr),
					   m_OutputReader(nullptr),
					   m_OutputLog(nullptr),
					   m_OutputListLog(nullptr),
					   m_OutputLogPageIndex(0),
					   m_OutputListLogPageIndex(0),
					   m_SwitchedToOutput(false),
					   m_SwitchedToOutputList(false),
					   m_CompilerPlugin(nullptr),
					   m_WaitingCompilerToFinish(false)
{
	// Make sure our resources are available.
	// In the generated boilerplate code we have no resources but when
	// we add some, it will be nice that this code is in place already ;)
	if (!Manager::LoadResource(_T("UnitTest.zip")))
	{
		NotifyMissingFile(_T("UnitTest.zip"));
	}
}

// destructor
UnitTest::~UnitTest()
{
}

void UnitTest::OnAttach()
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...
	Manager::Get()->RegisterEventSink(cbEVT_COMPILER_FINISHED, new cbEventFunctor<UnitTest, CodeBlocksEvent>(this, &UnitTest::OnCompilerFinished));

	Connect(ID_MENU_RUNTEST, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(UnitTest::OnRunTest));
	Connect(ID_MENU_RUNSUITE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(UnitTest::OnRunSuite));
	Connect(ID_MENU_RUNALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(UnitTest::OnRunAllTests));

	if (LogManager *logManager = Manager::Get()->GetLogManager())
	{
		// normal test output log
		m_OutputLog = new TextCtrlLogger();
		m_OutputLogPageIndex = logManager->SetLog(m_OutputLog);

		logManager->Slot(m_OutputLogPageIndex).title = _("UnitTest log");
		CodeBlocksLogEvent evtAdd1(cbEVT_ADD_LOG_WINDOW, m_OutputLog, logManager->Slot(m_OutputLogPageIndex).title);
		Manager::Get()->ProcessEvent(evtAdd1);

		// error list log
		wxArrayString Titles;
		Titles.Add(_("File"));
		Titles.Add(_("Line"));
		Titles.Add(_("Test"));
		Titles.Add(_("Message"));

		wxArrayInt Widths;
		Widths.Add(128);
		Widths.Add(48);
		Widths.Add(128);
		Widths.Add(640);

		m_OutputListLog = new TestOutputLog(Titles, Widths);
		m_OutputListLogPageIndex = logManager->SetLog(m_OutputListLog);

		logManager->Slot(m_OutputListLogPageIndex).title = _("UnitTest messages");
		CodeBlocksLogEvent addEvent(cbEVT_ADD_LOG_WINDOW, m_OutputListLog, logManager->Slot(m_OutputListLogPageIndex).title);
		Manager::Get()->ProcessEvent(addEvent);
	}

	m_OutputReader = new OutputReader(this);
}

void UnitTest::OnRelease(bool appShutDown)
{
	// do de-initialization for your plugin
	// if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
	// which means you must not use any of the SDK Managers
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
	if (appShutDown == false)
	{
		Manager::Get()->RemoveAllEventSinksFor(this);

		if (m_OutputLog != nullptr)
		{
			CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_OutputLog);
			Manager::Get()->ProcessEvent(evt);
		}

		if (m_OutputListLog != nullptr)
		{
			CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_OutputListLog);
			m_OutputListLog->DestroyControls();
			Manager::Get()->ProcessEvent(evt);
		}
	}

	m_OutputLog = nullptr;
	m_OutputListLog = nullptr;

	delete m_OutputReader;
	m_OutputReader = nullptr;
}

// Callback/Event handlers
void UnitTest::OnRunTest(wxCommandEvent &WXUNUSED(event))
{
	auto suiteAndTest = ut::getCurrentSuiteAndTestName();
	if (!suiteAndTest)
		return;

	wxString testName = suiteAndTest.value().first + "." + suiteAndTest.value().second;
	Manager::Get()->GetLogManager()->Log(testName);

	RunTests(_("--gtest_filter=") + testName);
}

void UnitTest::OnRunSuite(wxCommandEvent &WXUNUSED(event))
{
	auto suiteAndTest = ut::getCurrentSuiteAndTestName();
	if (!suiteAndTest)
		return;

	wxString suiteName = suiteAndTest.value().first;

	Manager::Get()->GetLogManager()->Log(suiteName);

	RunTests(_("--gtest_filter=") + suiteName + _(".*"));
}

void UnitTest::OnRunAllTests(wxCommandEvent &WXUNUSED(event))
{
	RunTests(_(""));
}

void UnitTest::OnOutput(const wxString &line)
{
	wcout << line.c_str() << flush;

	// normal log
	m_OutputLog->Append(line);

	// switch over to output log if this is the first output
	if (m_SwitchedToOutput == false)
	{
		CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_OutputLog);
		Manager::Get()->ProcessEvent(evtSwitch);

		m_SwitchedToOutput = true;
	}

	wxRegEx errorExp;
	errorExp.Compile(_T("^(.*?):(\\d+):\\s*(.*)$"));

	if (errorExp.Matches(line))
	{
		wxString file = errorExp.GetMatch(line, 1);
		wxString lineNumber = errorExp.GetMatch(line, 2);
		wxString test = wxEmptyString;
		int lineNumberNumeric;
		if (lineNumber.ToInt(&lineNumberNumeric))
		{
			auto suiteAndTest = ut::getSuiteAndTestName(lineNumberNumeric);
			if (suiteAndTest)
			{
				test = suiteAndTest.value().first + "." + suiteAndTest.value().second;
			}
		}
		wxString message = errorExp.GetMatch(line, 3);

		// list log
		wxArrayString str;
		str.Add(file);
		str.Add(lineNumber);
		str.Add(test);
		str.Add(message);

		m_OutputListLog->Append(str);

		if (m_SwitchedToOutputList == false)
		{
			CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_OutputListLog);
			Manager::Get()->ProcessEvent(evtSwitch);

			m_SwitchedToOutputList = true;
		}
	}
}

void UnitTest::OnTestRunFinished()
{
	cout << "UnitTest knows that test process terminated" << endl;

	m_OutputReader->Stop();

	// read last output
	m_OutputReader->ReadOutput();

	// clean up
	delete m_Process;
	m_Process = nullptr;
}

void UnitTest::RunTests(const wxString &commandLineArguments)
{
	m_CommandLineArguments = commandLineArguments;

	EnsureBuildUpToDate();
}

void UnitTest::EnsureBuildUpToDate()
{
	m_WaitingCompilerToFinish = false;

	// make sure the target is compiled
	const PluginManager::CompilerPlugins &plugins = Manager::Get()->GetPluginManager()->GetCompilerPlugins();
	if (plugins.size())
	{
		m_CompilerPlugin = (cbCompilerPlugin *)plugins[0];
	}
	else
	{
		m_CompilerPlugin = nullptr;
	}

	if (m_CompilerPlugin != nullptr)
	{
		// is the compiler already running?
		if (m_CompilerPlugin->IsRunning() == false)
		{
			m_WaitingCompilerToFinish = true;
			m_CompilerPlugin->Build();
			// now, when the build is finished, DoDebug will be launched in OnCompilerFinished()
		}
		else
		{
			Manager::Get()->GetLogManager()->Log(_T("Compiler is already in use."));
		}
	}
	else
	{
		Manager::Get()->GetLogManager()->Log(_T("No suitable compiler plugin found."));
	}
}

void UnitTest::OnCompilerFinished(cb_unused CodeBlocksEvent &event)
{
	Manager::Get()->GetLogManager()->Log(_T("Compiler Finished Callback"));

	if (m_WaitingCompilerToFinish == true)
	{
		m_WaitingCompilerToFinish = false;

		// only proceed if build succeeeded
		if (m_CompilerPlugin && m_CompilerPlugin->GetExitCode() == 0)
		{
			Manager::Get()->GetLogManager()->Log(_T("Launching Tests."));
			LaunchTestProcess();

			ProjectManager *manager = Manager::Get()->GetProjectManager();
			if (manager->GetIsRunning() && manager->GetIsRunning() == this)
				manager->SetIsRunning(nullptr);
		}
		else
		{
			Manager::Get()->GetLogManager()->Log(_T("Compilation failed, not running tests."));
		}
	}
	else
	{
		Manager::Get()->GetLogManager()->Log(_T("Not waiting for CompilerFinished."));
	}
}

int UnitTest::LaunchTestProcess()
{
	if (IsTestProcessRunning() == true) // don't run the tests twice
	{
		Manager::Get()->GetLogManager()->Log(_T("There is already a test process running."));
		return 1;
	}

	// get executable
	wxString executablePath, workingDirectory, linkerPath;

	cbProject *project = Manager::Get()->GetProjectManager()->GetActiveProject();

	// if no project open, exit
	if (!project)
	{
		wxString msg = _("You need to open a project\nbefore you can run any tests!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return 1;
	}

	// check the project s active target -> it should be executable !!
	wxString strTarget = project->GetActiveBuildTarget();
	if (strTarget.empty())
	{
		wxString msg = _("You need to have an (executable) target in your open project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return 1;
	}

	// let's get the target
	ProjectBuildTarget *target = nullptr;
	if (!project->BuildTargetValid(strTarget, false))
	{
		const int tgtIdx = project->SelectTarget();
		if (tgtIdx == -1)
		{
			return 1;
		}
		target = project->GetBuildTarget(tgtIdx);
		strTarget = target->GetTitle();
	}
	else
	{
		target = project->GetBuildTarget(strTarget);
	}

	if (target == nullptr)
	{
		wxString msg = _("You need to have an (executable) target in your open project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return 1;
	}

	// check the type of the target
	const TargetType targetType = target->GetTargetType();

	MacrosManager *macrosManager = Manager::Get()->GetMacrosManager();
	if (targetType == ttDynamicLib || targetType == ttStaticLib)
	{
		if (target->GetHostApplication().IsEmpty())
		{
			wxString msg = _("You need to have an ***executable*** target in your open project\nbefore using the plugin!");
			cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
			Manager::Get()->GetLogManager()->DebugLog(msg);
			return 1;
		}

		executablePath = project->GetBasePath() + target->GetHostApplication();
		macrosManager->ReplaceMacros(executablePath, target);
		workingDirectory = target->GetWorkingDir();
	}
	else if (targetType == ttExecutable || targetType == ttConsoleOnly)
	{
		executablePath = project->GetBasePath() + target->GetOutputFilename();
		macrosManager->ReplaceMacros(executablePath, target);
		workingDirectory = target->GetWorkingDir();
	}
	else
	{
		wxString msg = _("You need to have an ***executable*** target in your open project\nbefore using the plugin!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK, Manager::Get()->GetAppWindow());
		Manager::Get()->GetLogManager()->DebugLog(msg);
		return 1;
	}

	linkerPath = cbGetDynamicLinkerPathForTarget(project, target);

	// now we can actually launch the program
	// save old & set new working directory
	wxString oldWorkingDirectory = wxGetCwd();
	wxSetWorkingDirectory(workingDirectory);

	// save old & set new linker path
	wxString OldLinkerPath;
	wxGetEnv(CB_LIBRARY_ENVVAR, &OldLinkerPath);
	linkerPath = cbMergeLibPaths(OldLinkerPath, linkerPath);
	wxSetEnv(CB_LIBRARY_ENVVAR, linkerPath);

	// build command line
	wxString commandLine;
	commandLine << executablePath << _(" ") << m_CommandLineArguments;

	cout << "executing: ";
	wcout << commandLine.c_str() << endl;

	Manager::Get()->GetLogManager()->Log(_("Executing Unit Test: ") + commandLine);

	// create process
	m_Process = new UnitTestProcess(this);
	m_Process->Redirect();

	m_OutputListLog->Clear();
	m_OutputLog->Clear();

	m_SwitchedToOutput = false;
	m_SwitchedToOutputList = false;

	// start it
	if (wxExecute(commandLine, wxEXEC_ASYNC, m_Process) == 0)
	{
		cbMessageBox(_("Command Launch Failed: ") + commandLine);
	}

	// restore
	wxSetWorkingDirectory(oldWorkingDirectory);
	wxSetEnv(CB_LIBRARY_ENVVAR, OldLinkerPath);

	cout << "process started" << endl;

	// start reading output
	m_OutputReader->Start();

	return 0;
}

void UnitTest::KillTestProcess()
{
	if (m_Process != nullptr)
	{
		wxProcess::Kill(m_Process->GetPid(), wxSIGKILL);

		m_OutputReader->Stop();
		m_OutputReader->ReadOutput();

		delete m_Process;
		m_Process = nullptr;
	}
}

bool UnitTest::IsTestProcessRunning() const
{
	return (m_Process != nullptr);
}

void UnitTest::BuildMenu(wxMenuBar *menuBar)
{
	//The application is offering its menubar for your plugin,
	//to add any menu items you want...
	//Append any items you need in the menu...
	//NOTE: Be careful in here... The application's menubar is at your disposal.
	NotImplemented(_T("UnitTest::BuildMenu()"));
}

void UnitTest::BuildModuleMenu(const ModuleType type, wxMenu *menu, const FileTreeData *data)
{
	//Some library module is ready to display a pop-up menu.
	//Check the parameter \"type\" and see which module it is
	//and append any items you need in the menu...
	//TIP: for consistency, add a separator as the first item...
	//    NotImplemented(_T("UnitTest::BuildModuleMenu()"));

	// Add the comment functions to the editor's context-sensitive menu.
	if (type == mtEditorManager)
	{
		wxMenu *submenu = new wxMenu;

		menu->AppendSeparator();

		auto suiteAndTest = ut::getCurrentSuiteAndTestName();
		if (!suiteAndTest)
			return;

		wxString currentTestName = suiteAndTest.value().first + "." + suiteAndTest.value().second;
		if (currentTestName != _T(""))
		{
			wxString runTestCaption = _T("Run Test ") + currentTestName;

			wxMenuItem *runTestEntry = new wxMenuItem(submenu, ID_MENU_RUNTEST, runTestCaption, _("Run the currently selected test."));
			submenu->Append(runTestEntry);
		}

		wxString currentSuiteName = suiteAndTest.value().first;
		if (currentSuiteName != _T(""))
		{
			wxString runSuiteCaption = _T("Run Suite ") + currentSuiteName;

			wxMenuItem *runSuiteEntry = new wxMenuItem(submenu, ID_MENU_RUNSUITE, runSuiteCaption, _("Run the current test suite."));
			submenu->Append(runSuiteEntry);
		}

		wxMenuItem *runAllTestsEntry = new wxMenuItem(submenu, ID_MENU_RUNALL, _("Run All Tests"), _("Run all tests."));
		submenu->Append(runAllTestsEntry);

		menu->AppendSubMenu(submenu, wxT("Unit Testing"));
		// Events are already attached to these IDs in BuildMenu().
	}
}

bool UnitTest::BuildToolBar(wxToolBar *toolBar)
{
	//The application is offering its toolbar for your plugin,
	//to add any toolbar items you want...
	//Append any items you need on the toolbar...
	NotImplemented(_T("UnitTest::BuildToolBar()"));

	// return true if you add toolbar items
	return false;
}
