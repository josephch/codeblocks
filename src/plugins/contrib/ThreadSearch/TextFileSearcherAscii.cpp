/***************************************************************
 * Name:      TextFileSearcher
 * Purpose:   TextFileSearcher is used to search text files
 *            for text or regular expressions.
 * Author:    Jerome ANTOINE
 * Created:   2007-04-07
 * Copyright: Jerome ANTOINE
 * License:   GPL
 **************************************************************/

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <globals.h>
#endif

#include "encodingdetector.h"

#include <wx/filename.h>
#include "TextFileSearcher.h"
#include "TextFileSearcherAscii.h"
#include "SearchFileAscii.h"

TextFileSearcherAscii::TextFileSearcherAscii(const wxString& searchText, bool matchCase, bool matchWordBegin,
                                           bool matchWord, bool matchInComments)
    : TextFileSearcher(searchText, matchCase, matchWordBegin, matchWord, matchInComments)
{
    if (!matchCase)
    {
        m_SearchText.LowerCase();
    }
}


TextFileSearcher::eFileSearcherReturn TextFileSearcherAscii::FindInFile(const wxString& filePath,
                                                                   wxArrayString &foundLines,
                                                                   std::vector<int> &matchedPositions)
{
    SearchOptions searchOptions;
    searchOptions.m_MatchCase = m_MatchCase;
    searchOptions.m_MatchInComments = m_MatchInComments;
    searchOptions.m_MatchWord = m_MatchWord;
    searchOptions.m_MatchWordBegin = m_MatchWordBegin;
    std::vector<wxString> foundLinesVec;
    auto ret = FindInFileAscii<wxString>(filePath.ToStdString(), m_SearchText.ToStdString(), searchOptions, foundLinesVec, matchedPositions);
    for (auto& foundLine : foundLinesVec)
    {
        foundLines.Add(foundLine);
    }
    return ret;
}
