
#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <vector>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

struct TestSuite
{
		wxString name;
		int startLine;
		int endLine;
};

struct Test
{
		wxString name;
		int startLine;
		int endLine;
};

namespace ut
{

bool isWhiteSpace(wxChar c);
std::vector<wxString> split(wxString text, wxChar at);
wxString getCurrentSuiteName();
wxString getCurrentTestName();

} // namespace ut

#endif // __UTILITY_H__
