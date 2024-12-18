/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CCTREECTRL
#define CCTREECTRL

#include <wx/treectrl.h>

#include <queue>

#include "parser/parser.h" // BrowserSortType
#include "parser/token.h"  // TokenKind

class wxPoint;
class wxSize;
class wxWindow;

class Token;

/** Specify which kind of node it is */
enum SpecialFolder
{
    sfToken   = 0x0001, // token node
    sfRoot    = 0x0002, // root node
    sfGFuncs  = 0x0004, // global funcs node
    sfGVars   = 0x0008, // global vars node
    sfPreproc = 0x0010, // preprocessor symbols node
    sfTypedef = 0x0020, // typedefs node
    sfBase    = 0x0040, // base classes node
    sfDerived = 0x0080, // derived classes node
    sfMacro   = 0x0100  // global macro node
};

class TokenInfo
{
private:

public:
    void operator=(const Token& token)
    {
        m_Name = token.m_Name;
        m_FileIdx = token.m_FileIdx;
        m_Line = token.m_Line;
        m_ImplFileIdx = token.m_ImplFileIdx;
        m_ImplLine = token.m_ImplLine;
        m_Scope = token.m_Scope;
        m_Index = token.m_Index;
        m_ParseManager = token.m_Parser->GetParseManager();
        assert(token.m_Parser == &token.m_Parser->GetParseManager()->GetParser());
    };

    bool operator==(const Token& token) const
    {
        return (
                   (m_Name == token.m_Name) &&
                   (m_FileIdx == token.m_FileIdx ) &&
                   (m_Line == token.m_Line ) &&
                   (m_ImplFileIdx == token.m_ImplFileIdx ) &&
                   (m_ImplLine == token.m_ImplLine ) &&
                   (m_Scope == token.m_Scope ) &&
                   (m_Index == token.m_Index )
               );
    };

    bool operator!=(const Token& token) const
    {
        return !(*this == token);
    };

    Token* GetToken() const
    {
        Token* token = nullptr;
        if (m_ParseManager)
        {
            TokenTree* tokenTree = m_ParseManager->GetParser().GetTokenTree();
            if (tokenTree)
            {
                token  = tokenTree->at(m_Index);
                if (token)
                {
                    if (!(*this == *token))
                    {
                        fprintf(stderr, "%s:%d : token %p does not match with this %p\n", __FUNCTION__, __LINE__, token, this );
                        token = nullptr;
                    }
                }
                else
                {
                    fprintf(stderr, "%s:%d : no token for tokeninfo this %p\n", __FUNCTION__, __LINE__, this );
                }
            }
            else
            {
                fprintf(stderr, "%s:%d : could not get token tree for this %p\n", __FUNCTION__, __LINE__, this );
            }
        }
        return token;
    }

    wxString DisplayName()
    {
        if (m_DisplayName.empty())
        {
            Token* token = GetToken();
            if (token)
            {
                m_DisplayName = token->DisplayName();
            }
        }
        return m_DisplayName;
    }

    wxString GetFilename() const
    {
        wxString ret;
        if (m_ParseManager)
        {
            const TokenTree* tokenTree = m_ParseManager->GetParser().GetTokenTree();
            if (tokenTree)
            {
                ret = tokenTree->GetFilename(m_FileIdx);
            }
        }
        return ret;
    }

    wxString GetImplFilename() const
    {
        const TokenTree* tokenTree = m_ParseManager->GetParser().GetTokenTree();
        if (!tokenTree)
            return wxString(_T(""));
        return tokenTree->GetFilename(m_ImplFileIdx);
    }

    char* GetTokenDetails() const
    {
        char * ret;
        asprintf(& ret, "Token info : name %s line %d implLine %d", m_Name.ToUTF8().data(), m_Line, m_ImplLine);
        return ret;
    }

    wxString                     m_Name;
    TokenScope                   m_Scope;
    unsigned int                 m_ImplLine;
    unsigned int                 m_FileIdx;
    unsigned int                 m_ImplFileIdx;
    unsigned int                 m_Line;
    int                          m_Index;
    wxString                     m_DisplayName;
    ParseManager* m_ParseManager{nullptr};
};

/** Actual data stored with each node in the symbol tree */
class CCTreeCtrlData : public wxTreeItemData
{
public:
    CCTreeCtrlData(SpecialFolder sf = sfToken, Token* token = 0,
                   short int kindMask = 0xffff, int parentIdx = -1);
    ~CCTreeCtrlData();

    CCTreeCtrlData() = delete;
    CCTreeCtrlData(const CCTreeCtrlData&) = default;
    CCTreeCtrlData& operator=(const CCTreeCtrlData&) = default;
    CCTreeCtrlData(CCTreeCtrlData&&) = delete;
    CCTreeCtrlData& operator=(CCTreeCtrlData&&) = delete;

    /** a pointer to the associated Token instance in the TokenTree */
    TokenInfo m_Token;

    /** a copy of Token::m_KindMask
     * @todo this variable is not used?
     */
    short int     m_KindMask;

    /** the node's kind, it could be "root", "normal token", @sa SpecialFolder */
    SpecialFolder m_SpecialFolder;

    /** a copy of Token::m_TokenIndex */
    int           m_TokenIndex;

    /** a copy of Token::m_TokenKind */
    TokenKind     m_TokenKind;

    /** short name of the Token,
     * it is a copy of Token::m_Name
     */
    wxString      m_TokenName;

    /** the parent Token index
     * it is a copy of Token::m_ParentIndex, @sa Token::m_ParentIndex
     * @todo this variable is not used?
     */
    int           m_ParentIndex;

    /** the Token's ticket in a TokenTree
     * This is actually a copy of Token::m_Ticket, @sa Token::m_Ticket  */
    unsigned long m_Ticket;

    /** pointer to the mirror node in the non-GUI tree */
    void*         m_MirrorNode;
};

class CCTreeCtrlExpandedItemData
{
public:
    CCTreeCtrlExpandedItemData(const CCTreeCtrlData* data, const int level);

    int   GetLevel() const          { return m_Level; }
    const CCTreeCtrlData& GetData() { return m_Data;  }
private:
    CCTreeCtrlData m_Data;  // copy of tree item data
    int            m_Level; // nesting level in the tree
};

typedef std::deque<CCTreeCtrlExpandedItemData> ExpandedItemVect;
typedef std::deque<CCTreeCtrlData>             SelectedItemPath;

class CCTreeCntrl : public wxTreeCtrl
{
public:
    CCTreeCntrl();
    CCTreeCntrl(wxWindow* parent, const wxWindowID id, const wxPoint& pos,
               const wxSize& size, long style);

    void SetCompareFunction(const BrowserSortType type);
    void RemoveDoubles(const wxTreeItemId& parent);

protected:
    static int CBAlphabetCompare(CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);
    static int CBKindCompare    (CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);
    static int CBScopeCompare   (CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);
    static int CBLineCompare    (CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);
    static int CBNoCompare      (CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);

    int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2) override;
    int (*Compare)(CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);

    DECLARE_DYNAMIC_CLASS(CCTreeCntrl)
};

#endif // CCTREECTRL
