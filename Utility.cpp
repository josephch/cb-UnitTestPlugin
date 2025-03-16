#include "Utility.h"

#include <cbeditor.h>
#include <cbstyledtextctrl.h>
#include <configmanager.h>
#include <configurationpanel.h>
#include <editormanager.h>
#include <logmanager.h>
#include <sdk.h> // Code::Blocks SDK
#include <wx/regex.h>

using namespace std;

namespace ut
{

bool isWhiteSpace(wxChar c)
{
	return (c == '\n') || (c == ' ') || (c == '\t') || (c == '\r');
}

vector<wxString> split(wxString text, wxChar at)
{
	vector<wxString> result;

	wxString temp;
	for (wxChar c : text)
	{
		if (c == at)
		{
			result.push_back(temp);
			temp = _T("");
		}
		else
		{
			temp += c;
		}
	}

	return result;
}

wxString getCurrentSuiteName()
{
	cbEditor *editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (editor == nullptr)
		return _T("");

	cbStyledTextCtrl *control = editor->GetControl();
	if (control == nullptr)
		return _T("");

	int currentLine = control->GetCurrentLine();

	wxString editorText = control->GetText();

	// split text into lines
	vector<wxString> lines = split(editorText, '\n');

	// walk up checking for a pattern like
	// SUITE(TestSuite)
	wxRegEx reSuite;
	reSuite.Compile(_T("SUITE[[:space:]]*\\((.+)\\)"));

	while (currentLine >= 0 && currentLine < lines.size())
	{
		wxString &line = lines[currentLine];
		if (reSuite.Matches(line))
		{
			wxString suiteName = reSuite.GetMatch(line, 1);

			return suiteName;
		}

		currentLine--;
	}

	// nothing found, just take the first suite in the file
	// start from line 0 and walk down
	currentLine = 0;
	while (currentLine < lines.size())
	{
		wxString &line = lines[currentLine];
		if (reSuite.Matches(line))
		{
			wxString suiteName = reSuite.GetMatch(line, 1);

			return suiteName;
		}

		currentLine++;
	}

	return _T("");
}

std::optional<std::pair<wxString, wxString>> getCurrentSuiteAndTestName()
{
	cbEditor *editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
	if (editor == nullptr)
		return std::nullopt;

	cbStyledTextCtrl *control = editor->GetControl();
	if (control == nullptr)
		return std::nullopt;

	int currentLine = control->GetCurrentLine();

	wxString editorText = control->GetText();

	// split text into lines
	vector<wxString> lines = split(editorText, '\n');

	// walk up checking for a pattern like
	// TEST(TestName)
	wxRegEx reSuite;
	reSuite.Compile(_T("TEST[[:space:]]*\\((.+,.+)\\)"));

	while (currentLine >= 0 && currentLine < lines.size())
	{
		wxString &line = lines[currentLine];
		if (reSuite.Matches(line))
		{
			wxString match = reSuite.GetMatch(line, 1);
			wxArrayString arrayString = wxSplit(std::move(match), wxChar(','));
			fprintf(stderr, "%s:%d suite %s test %s\n", __FUNCTION__, __LINE__, arrayString[0].ToUTF8().data(), arrayString[1].ToUTF8().data());
			return make_pair(arrayString[0], arrayString[1].Trim(false));
		}
		currentLine--;
	}

	// nothing found, just take the first suite in the file
	// start from line 0 and walk down
	currentLine = 0;
	while (currentLine < lines.size())
	{
		wxString &line = lines[currentLine];
		if (reSuite.Matches(line))
		{
			wxString match = reSuite.GetMatch(line, 1);
			wxArrayString arrayString = wxSplit(std::move(match), wxChar(','));
			return make_pair(arrayString[0], arrayString[1]);
		}

		currentLine++;
	}
	return std::nullopt;
}

} // namespace ut
