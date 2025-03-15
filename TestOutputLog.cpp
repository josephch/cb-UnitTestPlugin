
#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/arrstr.h>
    #include <wx/filename.h>
    #include <wx/listctrl.h>
    #include "manager.h"
    #include "editormanager.h"
    #include "cbeditor.h"
#endif
#include "cbstyledtextctrl.h"

#include "TestOutputLog.h"

namespace
{
	const int ID_List = wxNewId();
};

BEGIN_EVENT_TABLE(TestOutputLog, wxEvtHandler)
//
END_EVENT_TABLE()

TestOutputLog::TestOutputLog(const wxArrayString& titles, wxArrayInt& widths)
    : ListCtrlLogger(titles, widths)
{
	//ctor
}

TestOutputLog::~TestOutputLog()
{
	//dtor
	Disconnect(ID_List, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
               (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
               &TestOutputLog::OnDoubleClick);
}


// TODO : use Getter instead of protected 'control'

wxWindow *TestOutputLog::CreateControl(wxWindow *parent)
{
	ListCtrlLogger::CreateControl(parent);
    control->SetId(ID_List);
    Connect(ID_List, -1, wxEVT_COMMAND_LIST_ITEM_ACTIVATED,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &TestOutputLog::OnDoubleClick);
    Manager::Get()->GetAppWindow()->PushEventHandler(this);
	return control;
}

void TestOutputLog::DestroyControls()
{
    if (!Manager::Get()->IsAppShuttingDown())
    {
        Manager::Get()->GetAppWindow()->RemoveEventHandler(this);
    }
}

void TestOutputLog::OnDoubleClick(wxCommandEvent& /*event*/)
{
	// go to the relevant file/line
	if (control->GetSelectedItemCount() == 0)
	{
		return;
	}
	// find selected item index
	const int index = control->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	SyncEditor(index);
} // end of OnDoubleClick

void TestOutputLog::SyncEditor(int selected)
{
	wxFileName Filename(control->GetItemText(selected));
	wxString filePath = Filename.GetFullPath();

	wxListItem li;
	li.m_itemId = selected;
	li.m_col = 1;
	li.m_mask = wxLIST_MASK_TEXT;
	control->GetItem(li);

	long line = 0;
	li.m_text.ToLong(&line);
	cbEditor* editor = Manager::Get()->GetEditorManager()->Open(filePath);
	if (!line || !editor)
	{
		return;
	}

	line -= 1;
	editor->Activate();
	editor->GotoLine(line);

	if (cbStyledTextCtrl* Control = editor->GetControl())
	{
		Control->EnsureVisible(line);
	}
}

void TestOutputLog::Fit()
{
    int columns = control->GetColumnCount();
    for (int ii = 0; ii < columns; ++ii)
        control->SetColumnWidth(ii, wxLIST_AUTOSIZE);
}

