/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CCDEBUGINFO_H
#define CCDEBUGINFO_H

#include <wx/wxprec.h>

//(*Headers(CCDebugInfo)
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/panel.h>
#include "scrollingdialog.h"
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
//*)

class ParserBase;
class Token;

// When instance this class, *MUST* entered a critical section, and should call ShowModal() rather than the other
// e.g. wxMutexLocker locker(s_TokenTreeMutex);
class CCDebugInfo: public wxScrollingDialog
{
public:
    CCDebugInfo(wxWindow* parent, ParserBase* parser, const Token* token);
    virtual ~CCDebugInfo();

    void FillFiles();
    void FillDirs();
    void FillMacros();
    void DisplayTokenInfo();
    void FillChildren();
    void FillAncestors();
    void FillDescendants();

    //(*Identifiers(CCDebugInfo)
    static const long ID_TEXTCTRL1;
    static const long ID_BUTTON1;
    static const long ID_STATICTEXT18;
    static const long ID_STATICTEXT2;
    static const long ID_STATICTEXT10;
    static const long ID_STATICTEXT12;
    static const long ID_STATICTEXT4;
    static const long ID_STATICTEXT6;
    static const long ID_STATICTEXT8;
    static const long ID_STATICTEXT37;
    static const long ID_STATICTEXT41;
    static const long ID_STATICTEXT14;
    static const long ID_STATICTEXT16;
    static const long ID_STATICTEXT33;
    static const long ID_STATICTEXT39;
    static const long ID_STATICTEXT1;
    static const long ID_STATICTEXT20;
    static const long ID_STATICTEXT24;
    static const long ID_BUTTON4;
    static const long ID_COMBOBOX3;
    static const long ID_BUTTON5;
    static const long ID_COMBOBOX2;
    static const long ID_BUTTON3;
    static const long ID_COMBOBOX1;
    static const long ID_BUTTON2;
    static const long ID_STATICTEXT26;
    static const long ID_BUTTON7;
    static const long ID_STATICTEXT28;
    static const long ID_BUTTON8;
    static const long ID_STATICTEXT35;
    static const long ID_PANEL1;
    static const long ID_LISTBOX1;
    static const long ID_PANEL2;
    static const long ID_LISTBOX2;
    static const long ID_PANEL3;
    static const long ID_LISTBOX3;
    static const long ID_PANEL4;
    static const long ID_NOTEBOOK1;
    static const long ID_BUTTON6;
    //*)

protected:
    //(*Handlers(CCDebugInfo)
    void OnInit(wxInitDialogEvent& event);
    void OnFindClick(wxCommandEvent& event);
    void OnGoAscClick(wxCommandEvent& event);
    void OnGoDescClick(wxCommandEvent& event);
    void OnGoParentClick(wxCommandEvent& event);
    void OnGoChildrenClick(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnGoDeclClick(wxCommandEvent& event);
    void OnGoImplClick(wxCommandEvent& event);
    //*)

    //(*Declarations(CCDebugInfo)
    wxComboBox* cmbChildren;
    wxStaticText* txtFullType;
    wxComboBox* cmbAncestors;
    wxTextCtrl* txtFilter;
    wxStaticText* txtArgsStripped;
    wxListBox* lstMacros;
    wxStaticText* txtID;
    wxStaticText* txtParent;
    wxStaticText* txtArgs;
    wxStaticText* txtNamespace;
    wxStaticText* txtBaseType;
    wxStaticText* txtIsOp;
    wxBoxSizer* BoxSizer11;
    wxStaticText* txtScope;
    wxPanel* Panel4;
    wxStaticText* txtKind;
    wxStaticText* txtDeclFile;
    wxListBox* lstFiles;
    wxListBox* lstDirs;
    wxStaticText* txtTemplateArg;
    wxStaticText* txtIsNoExcept;
    wxStaticText* txtInfo;
    wxStaticText* txtName;
    wxStaticText* txtIsLocal;
    wxStaticText* txtUserData;
    wxStaticText* txtIsTemp;
    wxBoxSizer* BoxSizer12;
    wxStaticText* txtImplFile;
    wxComboBox* cmbDescendants;
    wxButton* btnSave;
    wxBoxSizer* BoxSizer5;
    wxStaticText* txtIsConst;
    //*)

private:
    ParserBase* m_Parser;
    const Token*      m_Token;

    DECLARE_EVENT_TABLE()
};

#endif
