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
#include <fstream>


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
    eFileSearcherReturn success=idStringNotFound;
    try{
    // skip empty fles
    wxULongLong fileSize = wxFileName::GetSize(filePath);
    std::cerr << "TextFileSearcherAscii::FindInFile : file : " << filePath << " size " << fileSize << " <<\n";
    if (fileSize != 0)
    {
        if (fileSize == wxInvalidSize)
        {
            // We skip missing files without alerting user.
            // If a file has disappeared, it is not our problem.
            // cbMessageBox( filePath + _T(" does not exist."), _T("Error"), wxICON_ERROR);
            return idFileNotFound;
        }
        std::ifstream file(filePath.c_str());
        if (!file.is_open())
        {
            std::cerr << "TextFileSearcherAscii::FindInFile : file  : " << filePath << " not found\n";
            return idFileOpenError;
        }
        else
        {
            std::cerr << "TextFileSearcherAscii::FindInFile : file : " << filePath << " open. size " << fileSize << "\n";
            std::string lineOrig;
            std::string lineLowered;
            std::string &line = lineOrig;
            std::string searchStr = GetSearchText();
            // Tests all file lines
            for (size_t i = 0; std::getline(file, lineOrig); i++)
            {
                if (!m_MatchCase)
                {
                    lineLowered.reserve(lineOrig.size());
                    std::transform(lineOrig.begin(), lineOrig.end(), std::back_inserter(lineLowered),
                                   [](unsigned char c)
                                   { return std::tolower(c); });
                    line = lineLowered;
                }
                else
                {
                    line = lineOrig;
                }
                if (!m_MatchInComments)
                {
                    size_t pos = line.find_first_not_of(" \t\r\n");
                    if (pos != std::string::npos)
                    {
                        continue;
                    }
                    if (line.compare(pos, 2, "//") == 0)
                    {
                        continue;
                    }
                }
                size_t pos = 0;
                bool found = false;
                while ((pos = line.find(searchStr, pos)) != std::string::npos)
                {
                    if ((m_MatchWordBegin || m_MatchWord) && pos > 0)
                    {
                        // Try to see if this is the start of the word.
                        const char prevChar = line[pos - 1];
                        if (wxIsalnum(prevChar) || prevChar == '_')
                        {
                            pos++;
                            continue;
                        }
                    }

                    if (m_MatchWord && (pos + m_SearchText.length() < line.length()))
                    {
                        // Try to see if this is the end of the word.
                        const char nextChar = line[pos + m_SearchText.length()];
                        if (wxIsalnum(nextChar) || nextChar == '_')
                        {
                            pos++;
                            continue;
                        }
                    }
                    matchedPositions.push_back(pos);
                    pos += searchStr.length(); // Move past the last found occurrence
                    found = true;
                }
                if (!found)
                {
                    continue;
                }
                line = lineOrig;

                std::vector<int>::size_type idxMatchedCount = matchedPositions.size();
                success=idStringFound;
                // An interesting line is found. We clean and add it to the provided array
                std::replace(line.begin(), line.end(), '\t', ' ');
                std::replace(line.begin(), line.end(), '\r', ' ');
                std::replace(line.begin(), line.end(), '\n', ' ');

                int compensateStart = line.length();
                line.erase(0, line.find_first_not_of(' '));
                compensateStart = compensateStart - line.length();
                line.erase(line.find_last_not_of(' ') + 1);

                foundLines.Add(wxString::Format("%zu", i + 1));
                foundLines.Add(line);

                // We have to compensate any trimming on the left. If we don't do it the matched
                // positions would be incorrect.
                if (idxMatchedCount < matchedPositions.size())
                {
                    const int count = matchedPositions[idxMatchedCount];
                    for (int matchedIdx = 0; matchedIdx < count; ++matchedIdx)
                    {
                        const std::vector<int>::size_type index = idxMatchedCount + 1 + matchedIdx * 2;
                        const int oldStart = matchedPositions[index];
                        matchedPositions[index] = std::max(oldStart - compensateStart, 0);
                    }
                }
            }
        }
    }
    }
    catch (...)
    {
        abort();
    }
    return success;
}
