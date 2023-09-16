/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <wx/event.h>
#include <wx/string.h>

#include "tokentree.h"

#include "token.h"
#include "parser_base.h"

#ifdef TRACE
#define TRACE_PRINTF fprintf
#else
#define TRACE_PRINTF(x...)
#endif

#ifdef TRACE
#define TRACE_PRINTF fprintf
#else
#define TRACE_PRINTF(x...)
#endif

#define CC_TOKEN_DEBUG_OUTPUT 0

#if defined(CC_GLOBAL_DEBUG_OUTPUT)
    #if CC_GLOBAL_DEBUG_OUTPUT == 1
        #undef CC_TOKEN_DEBUG_OUTPUT
        #define CC_TOKEN_DEBUG_OUTPUT 1
    #elif CC_GLOBAL_DEBUG_OUTPUT == 2
        #undef CC_TOKEN_DEBUG_OUTPUT
        #define CC_TOKEN_DEBUG_OUTPUT 2
    #endif
#endif

#if CC_TOKEN_DEBUG_OUTPUT == 1
    #define TRACE(format, args...) \
        CCLogger::Get()->DebugLog(wxString::Format(format, ##args))
    #define TRACE2(format, args...)
#elif CC_TOKEN_DEBUG_OUTPUT == 2
    #define TRACE(format, args...)                                              \
        do                                                                      \
        {                                                                       \
            if (g_EnableDebugTrace)                                             \
                CCLogger::Get()->DebugLog(wxString::Format(format, ##args));                   \
        }                                                                       \
        while (false)
    #define TRACE2(format, args...) \
        CCLogger::Get()->DebugLog(wxString::Format(format, ##args))
#else
    #define TRACE(format, args...)
    #define TRACE2(format, args...)
#endif

Token::Token(const wxString& name, unsigned int file, unsigned int line, size_t ticket, ParserBase* parser) :
    m_Name(name),
    m_FileIdx(file),
    m_Line(line),
    m_ImplFileIdx(0),
    m_ImplLine(0),
    m_ImplLineStart(0),
    m_ImplLineEnd(0),
    m_Scope(tsUndefined),
    m_TokenKind(tkUndefined),
    m_IsOperator(false),
    m_IsLocal(false),
    m_IsTemp(false),
    m_IsConst(false),
    m_IsNoExcept(false),
    m_IsAnonymous(false),
    m_Index(-1),
    m_ParentIndex(-1),
    m_UserData(0),
    m_Parser(parser),
    m_Ticket(ticket)
{
    TRACE_PRINTF(stderr, "Token::%s:%d enter %p name %s\n", __FUNCTION__, __LINE__, this, m_Name.ToUTF8().data());
    //ctor
}

bool Token::operator==(Token const& other) const
{
	return (
		(m_Name == other.m_Name) &&
		(m_FileIdx == other.m_FileIdx ) &&
		(m_Line == other.m_Line ) &&
		(m_ImplFileIdx == other.m_ImplFileIdx ) &&
		(m_ImplLine == other.m_ImplLine ) &&
		(m_ImplLineStart == other.m_ImplLineStart ) &&
		(m_ImplLineEnd == other.m_ImplLineEnd ) &&
		(m_Scope == other.m_Scope ) &&
		(m_TokenKind == other.m_TokenKind ) &&
		(m_IsOperator == other.m_IsOperator ) &&
		(m_IsLocal == other.m_IsLocal ) &&
		(m_IsTemp == other.m_IsTemp ) &&
		(m_IsConst == other.m_IsConst ) &&
		(m_IsNoExcept == other.m_IsNoExcept ) &&
		(m_IsAnonymous == other.m_IsAnonymous ) &&
		(m_Index == other.m_Index ) &&
		(m_ParentIndex == other.m_ParentIndex ) &&
		(m_UserData == other.m_UserData ) &&
		(m_Parser == other.m_Parser ) &&
		(m_Ticket == other.m_Ticket )
	);
}

bool Token::operator!=(Token const& other) const
{
    return !(*this == other);
}

Token::~Token()
{
    TRACE_PRINTF(stderr, "Token::%s:%d enter %p name %s\n", __FUNCTION__, __LINE__, this, m_Name.ToUTF8().data());
    //dtor
    m_TemplateMap.clear();
    m_TemplateType.clear();
}

wxString Token::DisplayName() const
{
    wxString result;
    if      (m_TokenKind == tkClass)
        return result << _T("class ")     << m_Name << m_BaseArgs << _T(" {...}");
    else if (m_TokenKind == tkNamespace)
        return result << _T("namespace ") << m_Name << _T(" {...}");
    else if (m_TokenKind == tkEnum)
        return result << _T("enum ")      << m_Name << _T(" {...}");
    else if (m_TokenKind == tkTypedef)
    {
        result << _T("typedef");

        if (!m_FullType.IsEmpty())
            result << _T(" ") << m_FullType;

        // we support 2 cases of typedef'd function pointers, and in each case the type is stored
        // as below:
        // typedef void (*dMessageFunction)(int errnum, const char *msg, va_list ap);
        // --> type is stored as: (*)
        // typedef void (MyClass::*Function)(int);
        // --> type is stored as: (MyClass::*)
        // so, ensure we really have ')' as the last char.
        if (result.Find('*', true) != wxNOT_FOUND && result.Last() == ')')
        {
            result.RemoveLast();
            return result << m_Name << _T(")") <<  GetFormattedArgs();
        }

        if (!m_TemplateArgument.IsEmpty())
            result << m_TemplateArgument;

        return result << _T(" ") << m_Name;
    }
    else if (m_TokenKind == tkMacroDef)
    {
        result << _T("#define ") << m_Name << GetFormattedArgs();
        if (!m_FullType.IsEmpty())
            result << _T(" ") << m_FullType;

        return result;
    }

    // else
    if (!m_FullType.IsEmpty())
        result << m_FullType << m_TemplateArgument << _T(" ");

    if (m_TokenKind == tkEnumerator)
        return result << GetNamespace() << m_Name << _T("=") << GetFormattedArgs();

    return result << GetNamespace() << m_Name << GetStrippedArgs();
}

bool Token::IsValidAncestor(const wxString& ancestor)
{
    switch (ancestor.Len())
    {
    case 3:
        if (ancestor == _T("int"))
            return false;
        break;

    case 4:
        if (   ancestor == _T("void")
            || ancestor == _T("bool")
            || ancestor == _T("long")
            || ancestor == _T("char") )
        {
            return false;
        }
        break;

    case 5:
        if (   ancestor == _T("short")
            || ancestor == _T("float") )
        {
            return false;
        }
        break;

    case 6:
        if (   ancestor == _T("size_t")
            || ancestor == _T("double") )
        {
            return false;
        }
        break;

    case 10:
        if (ancestor == _T("value_type"))
            return false;
        break;

    default:
        if (   ancestor.StartsWith(_T("unsigned"))
            || ancestor.StartsWith(_T("signed")) )
        {
            return false;
        }
        break;
    }

    return true;
}

wxString Token::GetFilename() const
{
    wxString ret;
    if (m_Parser)
    {
        const TokenTree* tokenTree = m_Parser->GetTokenTree();
        if (tokenTree)
        {
            ret = tokenTree->GetFilename(m_FileIdx);
        }
    }
    return ret;
}

wxString Token::GetImplFilename() const
{
    const TokenTree* tokenTree = m_Parser->GetTokenTree();
    if (!tokenTree)
        return wxString(_T(""));
    return tokenTree->GetFilename(m_ImplFileIdx);
}

// ----------------------------------------------------------------------------
wxString Token::GetFormattedArgs() const
// ----------------------------------------------------------------------------
{
    wxString args(m_Args);
    args.Replace(_T("\n"), wxEmptyString);
    return args;
}

// ----------------------------------------------------------------------------
wxString Token::GetStrippedArgs() const
// ----------------------------------------------------------------------------
{
    // the argument should have the format (xxxx = y, ....) or just an empty string
    // if it is empty, we just return an empty string
    if (m_Args.IsEmpty())
        return wxEmptyString;

    wxString args;
    args.Alloc(m_Args.Len() + 1);
    bool skipDefaultValue = false;
    for (size_t i = 0; i < m_Args.Len(); ++i)
    {
        const wxChar ch = m_Args[i];
        if (ch == _T('\n'))
            continue;
        else if (ch == _T('='))
        {
            skipDefaultValue = true;
            args.Trim();
        }
        else if (ch == _T(','))
            skipDefaultValue = false;

        if (!skipDefaultValue)
            args << ch;
    }

    if (args.Last() != _T(')'))
        args << _T(')');
    if (not args.StartsWith('('))
        args.Prepend('(');

    return args;
}

bool Token::MatchesFiles(const TokenFileSet& files)
{
    if (!files.size())
        return true;

    if (!m_FileIdx && !m_ImplFileIdx)
        return true;

    if ((m_FileIdx && files.count(m_FileIdx)) || (m_ImplFileIdx && files.count(m_ImplFileIdx)))
        return true;

    return false;
}

wxString Token::GetNamespace() const
{
    const wxString dcolon(_T("::"));
    wxString res;
    const TokenTree* tokenTree = m_Parser->GetTokenTree();
    if (!tokenTree)
    {
        return res;
    }
    const Token* parentToken = tokenTree->at(m_ParentIndex);
    while (parentToken)
    {
        res.Prepend(dcolon);
        res.Prepend(parentToken->m_Name);
        parentToken = tokenTree->at(parentToken->m_ParentIndex);
    }
    return res;
}

bool Token::AddChild(int childIdx)
{
    if (childIdx < 0)
        return false;
    m_Children.insert(childIdx);
    return true;
}

bool Token::DeleteAllChildren()
{
    TokenTree* tokenTree = m_Parser->GetTokenTree();
    if (!tokenTree)
        return false;
    for (;;)
    {
        TokenIdxSet::const_iterator it = m_Children.begin();
        if (it == m_Children.end())
            break;
        tokenTree->erase(*it);
    }
    return true;
}

bool Token::InheritsFrom(int idx) const
{
    const TokenTree* tokenTree = m_Parser->GetTokenTree();
    if (idx < 0 || !tokenTree)
        return false;

    const Token* token = tokenTree->at(idx);
    if (!token)
        return false;

    for (TokenIdxSet::const_iterator it = m_DirectAncestors.begin(); it != m_DirectAncestors.end(); it++)
    {
        int idx2 = *it;
        const Token* ancestor = tokenTree->at(idx2);
        if (!ancestor)
            continue;

        if (ancestor == token || ancestor->InheritsFrom(idx)) // ##### is this intended?
            return true;
    }
    return false;
}

wxString Token::GetTokenKindString() const
{
    switch (m_TokenKind)
    {
        case tkClass:           return _T("class");
        case tkNamespace:       return _T("namespace");
        case tkTypedef:         return _T("typedef");
        case tkEnum:            return _T("enum");
        case tkEnumerator:      return _T("enumerator");
        case tkFunction:        return _T("function");
        case tkConstructor:     return _T("constructor");
        case tkDestructor:      return _T("destructor");
        case tkMacroDef:        return _T("macro definition");
        case tkMacroUse:        return _T("macro usage");
        case tkVariable:        return _T("variable");
        case tkAnyContainer:    return _T("any container");
        case tkAnyFunction:     return _T("any function");
        case tkUndefined:       return _T("undefined");
        default:                return wxEmptyString; // tkUndefined
    }
}

wxString Token::GetTokenScopeString() const
{
    switch (m_Scope)
    {
        case tsPrivate:   return _T("private");
        case tsProtected: return _T("protected");
        case tsPublic:    return _T("public");
        case tsUndefined: return _T("undefined");
        default:          return wxEmptyString;
    }
}
