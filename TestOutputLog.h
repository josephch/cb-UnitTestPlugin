#ifndef __TESTOUTPUTLOG_H__
#define __TESTOUTPUTLOG_H__

#include "loggers.h"
#include <wx/event.h>

class wxArrayString;
class wxArrayInt;
class wxCommandEvent;
class wxWindow;

class TestOutputLog : public ListCtrlLogger, public wxEvtHandler
{
	public:
		TestOutputLog(const wxArrayString &Titles, wxArrayInt &Widths);
		~TestOutputLog();

		wxWindow *CreateControl(wxWindow *Parent);
		void DestroyControls();

		void Fit();

	private:
		void OnDoubleClick(wxCommandEvent &Event);
		void SyncEditor(int selIndex);

		DECLARE_EVENT_TABLE()
};

#endif // __TESTOUTPUTLOG_H__
