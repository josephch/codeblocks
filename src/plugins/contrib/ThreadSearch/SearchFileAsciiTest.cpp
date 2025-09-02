#include <fstream>
#include <iostream>

#include <gtest/gtest.h>

#include "SearchFileAscii.h"

constexpr const char* testFileName = "searchfiletest.txt";

TEST(FileNotFound, simple)
{
    SearchOptions searchOptions;
    std::vector<std::string> foundLines;
    std::vector<int> matchedPositions;
    TextFileSearcher::eFileSearcherReturn ret = FindInFileAscii("FileNotFound.txt", "test", searchOptions, foundLines, matchedPositions);
    EXPECT_EQ(ret, TextFileSearcher::idFileNotFound);
}

TEST(NoMatchCase, negative)
{
    SearchOptions searchOptions;
    searchOptions.m_MatchCase = false;
    std::ofstream outputFile(testFileName);
    ASSERT_EQ(outputFile.is_open(), true);
    std::string searchStr = "test";
    outputFile << "dust";
    outputFile.close();
    std::vector<std::string> foundLines;
    std::vector<int> matchedPositions;
    TextFileSearcher::eFileSearcherReturn ret = FindInFileAscii(testFileName, searchStr, searchOptions, foundLines, matchedPositions);
    EXPECT_EQ(ret, TextFileSearcher::idStringNotFound);
}

TEST(NoMatchCase, outputCase)
{
    SearchOptions searchOptions;
    searchOptions.m_MatchCase = false;
    std::ofstream outputFile(testFileName);
    ASSERT_EQ(outputFile.is_open(), true);
    std::string searchStr = "test";
    std::string fileString = "Test";
    outputFile << fileString;
    outputFile.close();
    std::vector<std::string> foundLines;
    std::vector<int> matchedPositions;
    TextFileSearcher::eFileSearcherReturn ret = FindInFileAscii(testFileName, searchStr, searchOptions, foundLines, matchedPositions);
    EXPECT_EQ(ret, TextFileSearcher::idStringFound);
    EXPECT_EQ(foundLines.size(), 2);
    EXPECT_EQ(foundLines[0], "1");
    EXPECT_EQ(foundLines[1], fileString);
    EXPECT_EQ(matchedPositions.size(), 1);
    EXPECT_EQ(matchedPositions[0], 0);
}

TEST(MatchCase, single)
{
    SearchOptions searchOptions;
    searchOptions.m_MatchCase = true;
    std::ofstream outputFile(testFileName);
    ASSERT_EQ(outputFile.is_open(), true);
    std::string searchStr = "test";
    outputFile << searchStr;
    outputFile.close();
    std::vector<std::string> foundLines;
    std::vector<int> matchedPositions;
    TextFileSearcher::eFileSearcherReturn ret = FindInFileAscii(testFileName, searchStr, searchOptions, foundLines, matchedPositions);
    EXPECT_EQ(ret, TextFileSearcher::idStringFound);
    EXPECT_EQ(foundLines.size(), 2);
    EXPECT_EQ(foundLines[0], "1");
    EXPECT_EQ(foundLines[1], searchStr);
    EXPECT_EQ(matchedPositions.size(), 1);
    EXPECT_EQ(matchedPositions[0], 0);
}

TEST(MatchCase, twoMatchSameLine)
{
    SearchOptions searchOptions;
    searchOptions.m_MatchCase = true;
    std::ofstream outputFile(testFileName);
    ASSERT_EQ(outputFile.is_open(), true);
    std::string searchStr = "test";
    std::stringstream fileContent;
    fileContent << searchStr << searchStr;
    outputFile << fileContent.str();
    outputFile.close();
    std::vector<std::string> foundLines;
    std::vector<int> matchedPositions;
    TextFileSearcher::eFileSearcherReturn ret = FindInFileAscii(testFileName, searchStr, searchOptions, foundLines, matchedPositions);
    EXPECT_EQ(ret, TextFileSearcher::idStringFound);
    EXPECT_EQ(foundLines.size(), 2);
    EXPECT_EQ(foundLines[0], "1");
    EXPECT_EQ(foundLines[1], fileContent.str());
    EXPECT_EQ(matchedPositions.size(), 2);
    EXPECT_EQ(matchedPositions[0], 0);
    EXPECT_EQ(matchedPositions[1], searchStr.size());
}

TEST(MatchCase, twoMatchSecondAndFourthLine)
{
    SearchOptions searchOptions;
    searchOptions.m_MatchCase = true;
    std::ofstream outputFile(testFileName);
    ASSERT_EQ(outputFile.is_open(), true);
    std::string searchStr = "searchcontent";
    std::stringstream fileContent;
    fileContent << "invalid\n";
    fileContent << searchStr;
    fileContent << "\n invalid second\n"
                << searchStr;
    outputFile << fileContent.str();
    outputFile.close();
    std::vector<std::string> foundLines;
    std::vector<int> matchedPositions;
    TextFileSearcher::eFileSearcherReturn ret = FindInFileAscii(testFileName, searchStr, searchOptions, foundLines, matchedPositions);
    EXPECT_EQ(ret, TextFileSearcher::idStringFound);
    EXPECT_EQ(foundLines.size(), 4);
    EXPECT_EQ(foundLines[0], "2");
    EXPECT_EQ(foundLines[1], searchStr);
    EXPECT_EQ(foundLines[2], "4");
    EXPECT_EQ(foundLines[3], searchStr);
    EXPECT_EQ(matchedPositions.size(), 2);
    EXPECT_EQ(matchedPositions[0], 0);
    EXPECT_EQ(matchedPositions[1], 0);
}

TEST(MatchCase, negative)
{
    SearchOptions searchOptions;
    searchOptions.m_MatchCase = true;
    std::ofstream outputFile(testFileName);
    ASSERT_EQ(outputFile.is_open(), true);
    std::string searchStr = "test";
    outputFile << "Test";
    outputFile.close();
    std::vector<std::string> foundLines;
    std::vector<int> matchedPositions;
    TextFileSearcher::eFileSearcherReturn ret = FindInFileAscii(testFileName, searchStr, searchOptions, foundLines, matchedPositions);
    EXPECT_EQ(ret, TextFileSearcher::idStringNotFound);
}

TEST(NoMatchCase, positive)
{
    SearchOptions searchOptions;
    searchOptions.m_MatchCase = false;
    std::ofstream outputFile(testFileName);
    ASSERT_EQ(outputFile.is_open(), true);
    std::string searchStr = "test";
    outputFile << searchStr << "\n";
    outputFile.close();
    std::vector<std::string> foundLines;
    std::vector<int> matchedPositions;
    TextFileSearcher::eFileSearcherReturn ret = FindInFileAscii(testFileName, searchStr, searchOptions, foundLines, matchedPositions);
    EXPECT_EQ(ret, TextFileSearcher::idStringFound);
    EXPECT_EQ(foundLines.size(), 2);
    EXPECT_EQ(foundLines[0], "1");
    EXPECT_EQ(foundLines[1], searchStr);
    EXPECT_EQ(matchedPositions.size(), 1);
    EXPECT_EQ(matchedPositions[0], 0);
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
