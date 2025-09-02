#ifndef SEARCH_FILE_ASCII_H
#define SEARCH_FILE_ASCII_H

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifndef TEXT_FILE_SEARCHER_H
#define TEXT_FILE_SEARCHER_H
class TextFileSearcher
{
  public:
    enum eFileSearcherReturn
    {
        idStringFound = 0,
        idStringNotFound,
        idFileNotFound,
        idFileOpenError
    };
};
#endif

struct SearchOptions
{
    bool m_MatchCase;
    bool m_MatchInComments;
    bool m_MatchWordBegin;
    bool m_MatchWord;
    SearchOptions() : m_MatchCase{false}, m_MatchInComments{false}, m_MatchWordBegin{false}, m_MatchWord{false} {};
};

template <typename T>
TextFileSearcher::eFileSearcherReturn FindInFileAscii(const std::string& filePath, const std::string& searchStr, const SearchOptions& searchOptions,
                                                      std::vector<T>& foundLines,
                                                      std::vector<int>& matchedPositions)
{
    if (!std::filesystem::exists(filePath))
    {
        std::cerr << __FUNCTION__ << ":" << __LINE__ << " : file : " << filePath << " not exist\n";
        return TextFileSearcher::idFileNotFound;
    }
    TextFileSearcher::eFileSearcherReturn success = TextFileSearcher::idStringNotFound;
    // skip empty fles
    std::uintmax_t fileSize = std::filesystem::file_size(filePath);
    if (fileSize != 0)
    {
        std::ifstream file(filePath.c_str());
        if (!file.is_open())
        {
            std::cerr << __FUNCTION__ << ":" << __LINE__ << " : file : " << filePath << " not found\n";
            return TextFileSearcher::idFileOpenError;
        }
        else
        {
            // std::cerr << __FUNCTION__ << ":" << __LINE__ << " : file : " << filePath << " open. size " << fileSize << "\n";
            std::string lineOrig;
            std::string lineLowered;
            std::string* pline = &lineOrig;
            // Tests all file lines
            for (size_t i = 0; std::getline(file, lineOrig); i++)
            {
                // std::cerr << __FUNCTION__ << ":" << __LINE__ << " : file : " << filePath << " open. lineOrig : [" << lineOrig << "] i : " << i << "\n";
                if (!searchOptions.m_MatchCase)
                {
                    lineLowered.clear();
                    lineLowered.reserve(lineOrig.size());
                    std::transform(lineOrig.begin(), lineOrig.end(), std::back_inserter(lineLowered),
                                   [](unsigned char c)
                                   { return std::tolower(c); });
                    pline = &lineLowered;
                }
                std::string& searchLine = *pline;
                if (!searchOptions.m_MatchInComments)
                {
                    size_t pos = searchLine.find_first_not_of(" \t\r\n");
                    if (pos == std::string::npos)
                    {
                        continue;
                    }
                    if (searchLine.compare(pos, 2, "//") == 0)
                    {
                        continue;
                    }
                }
                size_t pos = 0;
                bool found = false;
                while ((pos = searchLine.find(searchStr, pos)) != std::string::npos)
                {
                    if ((searchOptions.m_MatchWordBegin || searchOptions.m_MatchWord) && pos > 0)
                    {
                        // Try to see if this is the start of the word.
                        const char prevChar = searchLine[pos - 1];
                        if (isalnum(prevChar) || prevChar == '_')
                        {
                            pos++;
                            continue;
                        }
                    }

                    if (searchOptions.m_MatchWord && (pos + searchStr.length() < searchLine.length()))
                    {
                        // Try to see if this is the end of the word.
                        const char nextChar = searchLine[pos + searchStr.length()];
                        if (isalnum(nextChar) || nextChar == '_')
                        {
                            pos++;
                            continue;
                        }
                    }
                    // std::cerr << "found pos : " << pos << "\n";
                    matchedPositions.push_back(pos);
                    pos += searchStr.length(); // Move past the last found occurrence
                    found = true;
                }
                if (!found)
                {
                    continue;
                }

                std::string& line = lineOrig;

                std::vector<int>::size_type idxMatchedCount = matchedPositions.size();
                success = TextFileSearcher::idStringFound;
                // An interesting line is found. We clean and add it to the provided array
                std::replace(line.begin(), line.end(), '\t', ' ');
                std::replace(line.begin(), line.end(), '\r', ' ');
                std::replace(line.begin(), line.end(), '\n', ' ');

                int compensateStart = line.length();
                line.erase(0, line.find_first_not_of(' '));
                compensateStart = compensateStart - line.length();
                line.erase(line.find_last_not_of(' ') + 1);

                foundLines.push_back(std::to_string(i + 1));
                foundLines.push_back(line);
                // std::cerr << __FUNCTION__ << ":" << __LINE__ << " found line : " << line << " line no : " << i + 1 << "\n";

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
    // std::cerr << __FUNCTION__ << ":" << __LINE__ << " : file : " << filePath << " searchStr : " << searchStr << " size " << fileSize << " foundLines size : " << foundLines.size() << " matchedPositions size : " << matchedPositions.size() << "\n";
    return success;
}

#endif // SEARCH_FILE_ASCII_H
