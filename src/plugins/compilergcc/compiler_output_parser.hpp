#ifndef COMPILER_OUTPUT_PARSER_HPP_INCLUDED
#define COMPILER_OUTPUT_PARSER_HPP_INCLUDED

#include <iostream>
#include "ctre.hpp"

enum class CompilerOutputLineType
{
    normal,
    warning,
    error,
    info
};

struct CompilerOutputLineInfo
{
    CompilerOutputLineType type;
    std::string fileName;
    std::string line;
    std::string message;
    auto operator<=>(const CompilerOutputLineInfo&) const = default;
};

struct CompilerRegexInfo
{
    const char* name{nullptr};
    CompilerOutputLineType type;
    size_t fileNameIdx;
    size_t lineIdx;
    size_t messageIdx;
};

#define POPULATE_INFO(info, match, regexInfo)                                                                               \
    {                                                                                                                       \
        info.type = regexInfo.type;                                                                                         \
        /*std::cout << "Matched : name : [" << regexInfo.name << "] type [" << static_cast<int>(regexInfo.type) << "]\n";*/ \
        if (compilerRegexInfo.fileNameIdx) info.fileName = match.get<compilerRegexInfo.fileNameIdx>().to_string();          \
        if (compilerRegexInfo.lineIdx) info.line = match.get<compilerRegexInfo.lineIdx>().to_string();                      \
        if (compilerRegexInfo.messageIdx) info.message = match.get<compilerRegexInfo.messageIdx>().to_string();             \
    }

inline CompilerOutputLineInfo GetCompilerOutputLineInfo(std::string_view line)
{
    CompilerOutputLineInfo ret;
    //<![CDATA[FATAL:[[:blank:]]*(.*)]]>
    if (auto m = ctre::match<"FATAL:[[:blank:]]*(.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Fatal error", .type = CompilerOutputLineType::error, .fileNameIdx = 0, .lineIdx = 0, .messageIdx = 1};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):[[:blank:]]+([iI]n
    //([cC]lass|[cC]onstructor|[dD]estructor|[fF]unction|[mM]ember [fF]unction).*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):[[:blank:]]+([iI]n "
                                  "([cC]lass|[cC]onstructor|[dD]estructor|[fF]unction|[mM]ember [fF]unction).*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "'In function...' info", .type = CompilerOutputLineType::info, .fileNameIdx = 1, .lineIdx = 0, .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[0-9]+:[[:blank:]]+(\[[[:blank:]]+[Ss]kipping [0-9]+ instantiation
    // contexts[[:blank:]]+\])]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[0-9]+:[[:blank:]]+(\\[[[:blank:]]+[Ss]kipping "
                                  "[0-9]+ instantiation contexts[[:blank:]]+\\])">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {.name = "'Skipping N instantiation contexts' info (2)",
                                                                .type = CompilerOutputLineType::info,
                                                                .fileNameIdx = 1,
                                                                .lineIdx = 2,
                                                                .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[[:blank:]]+(\[[[:blank:]]+[Ss]kipping [0-9]+ instantiation
    // contexts[[:blank:]]+\])]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[[:blank:]]+(\\[[[:blank:]]+[Ss]kipping "
                                  "[0-9]+ instantiation contexts[[:blank:]]+\\])">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {.name = "'Skipping N instantiation contexts' info",
                                                                .type = CompilerOutputLineType::info,
                                                                .fileNameIdx = 1,
                                                                .lineIdx = 2,
                                                                .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):[[:blank:]]+([Ii]n [Ii]nstantiation.*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):[[:blank:]]+([Ii]n [Ii]nstantiation.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "'In instantiation' warning", .type = CompilerOutputLineType::warning, .fileNameIdx = 1, .lineIdx = 0, .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[0-9]+:[[:blank:]]+([Rr]equired from.*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[0-9]+:[[:blank:]]+([Rr]equired from.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "'Required from' warning", .type = CompilerOutputLineType::warning, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[0-9]+:[[:blank:]]+([Ii]nstantiated from .*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[0-9]+:[[:blank:]]+([Ii]nstantiated from .*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "'Instantiated from' info (2)", .type = CompilerOutputLineType::info, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[[:blank:]]+([Ii]nstantiated from .*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[[:blank:]]+([Ii]nstantiated from .*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "'Instantiated from' info", .type = CompilerOutputLineType::info, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[windres.exe:[[:blank:]]([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[[:blank:]](.*)]]>
    else if (auto m = ctre::match<"windres.exe:[[:blank:]]([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[[:blank:]](.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Resource compiler error", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[windres.exe:[[:blank:]](.*)]]>
    else if (auto m = ctre::match<"windres.exe:[[:blank:]](.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Resource compiler error (2)", .type = CompilerOutputLineType::error, .fileNameIdx = 0, .lineIdx = 0, .messageIdx = 1};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):([0-9]+):[[:blank:]]([Ww]arning:[[:blank:]].*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):([0-9]+):[[:blank:]]([Ww]arning:[[:blank:]].*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Preprocessor warning", .type = CompilerOutputLineType::warning, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 4};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[0-9]+:[[:blank:]]([Nn]ote:[[:blank:]].*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[0-9]+:[[:blank:]]([Nn]ote:[[:blank:]].*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Compiler note (2)", .type = CompilerOutputLineType::info, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[[:blank:]]([Nn]ote:[[:blank:]].*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[[:blank:]]([Nn]ote:[[:blank:]].*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Compiler note", .type = CompilerOutputLineType::info, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([Nn]ote:[[:blank:]].*)]]>
    else if (auto m = ctre::match<".{0,1023}([Nn]ote:[[:blank:]].*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "General note", .type = CompilerOutputLineType::info, .fileNameIdx = 0, .lineIdx = 0, .messageIdx = 1};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[0-9]+:[[:blank:]](.*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[0-9]+:[[:blank:]](.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Preprocessor error", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[0-9]+:[[:blank:]]([Ww]arning:[[:blank:]].*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[0-9]+:[[:blank:]]([Ww]arning:[[:blank:]].*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Compiler warning (2)", .type = CompilerOutputLineType::warning, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[[:blank:]]([Ww]arning:[[:blank:]].*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[[:blank:]]([Ww]arning:[[:blank:]].*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Compiler warning", .type = CompilerOutputLineType::warning, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[[][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+\.o:([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[[:blank:]](undefined
    // reference.*)]]>
    else if (auto m = ctre::match<"[{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+\\.o:([{}()[:blank:]#%$~[:alnum:]!&_:+/"
                                  "\\\\\\.\\-]+):([0-9]+):[[:blank:]](undefined reference.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Undefined reference (2)", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[0-9]+:[[:blank:]](.*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[0-9]+:[[:blank:]](.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Compiler error (2)", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[[:blank:]](.*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[[:blank:]](.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Compiler error", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):\(\.text\+[0-9a-fA-FxX]+\):[[:blank:]]([Ww]arning:[[:blank:]].*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/"
                                  "\\\\\\.\\-]+):\\(\\.text\\+[0-9a-fA-FxX]+\\):[[:blank:]]([Ww]arning:[[:blank:]].*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Linker warning", .type = CompilerOutputLineType::warning, .fileNameIdx = 1, .lineIdx = 0, .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):([0-9]+):[0-9]+:[[:blank:]](.*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):([0-9]+):[0-9]+:[[:blank:]](.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Linker error", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 2, .messageIdx = 3};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[[][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+\(.text\+[0-9A-Za-z]+\):([[:blank:]A-Za-z0-9_:+/\.-]+):[[:blank:]](.*)]]>
    else if (auto m = ctre::match<"[{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+\\(.text\\+[0-9A-Za-z]+\\):([[:blank:]A-Za-z0-9_:+/"
                                  "\\.\\-]+):[[:blank:]](.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Linker error (2)", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 0, .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):\(\.text\+[0-9a-fA-FxX]+\):(.*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):\\(\\.text\\+[0-9a-fA-FxX]+\\):(.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Linker error (3)", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 0, .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[.*(ld.*):[[:blank:]](cannot find.*)]]>
    else if (auto m = ctre::match<".{0,1023}(ld.{0,1023}):[[:blank:]](cannot find.*)">(line))  // Error
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Linker error (lib not found)", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 0, .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[.*(ld.*):[[:blank:]](cannot open output file.*):[[:blank:]](.*)]]>
    else if (auto m = ctre::match<".{0,1023}(ld.{0,1023}):[[:blank:]](cannot open output file.*):[[:blank:]](.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Linker error (cannot open output file)", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 0, .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
        // TODO msg2
    }
    //<![CDATA[.*(ld.*):[[:blank:]](unrecognized option.*)]]>
    else if (auto m = ctre::match<".{0,1023}(ld.{0,1023}):[[:blank:]](unrecognized option.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Linker error (unrecognized option)", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 0, .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[.*cc.*:[[:blank:]]([Uu]nrecognized.*option.*)]]>
    else if (auto m = ctre::match<".{0,1023}cc.{0,1023}:[[:blank:]]([Uu]nrecognized.*option.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Compiler error (unrecognized option)", .type = CompilerOutputLineType::error, .fileNameIdx = 0, .lineIdx = 0, .messageIdx = 1};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[.*:(.*):[[:blank:]](No such file or directory.*)]]>
    else if (auto m = ctre::match<".{0,1023}:(.{0,1023}):[[:blank:]](No such file or directory.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "No such file or directory", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 0, .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):[[:blank:]](undefined reference.*)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):[[:blank:]](undefined reference.*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Undefined reference", .type = CompilerOutputLineType::error, .fileNameIdx = 1, .lineIdx = 0, .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([Ee]rror:[[:blank:]].*)]]>
    else if (auto m = ctre::match<".{0,1023}([Ee]rror:[[:blank:]].*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "General error", .type = CompilerOutputLineType::error, .fileNameIdx = 0, .lineIdx = 0, .messageIdx = 1};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([Ww]arning:[[:blank:]].*)]]>
    else if (auto m = ctre::match<".{0,1023}([Ww]arning:[[:blank:]].*)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "General warning", .type = CompilerOutputLineType::warning, .fileNameIdx = 0, .lineIdx = 0, .messageIdx = 1};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([Ii]nfo:[[:blank:]].*)\(auto-import\)]]>
    else if (auto m = ctre::match<"(.{0,1023}[Ii]nfo:[[:blank:]].*)\\(auto-import\\)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {
            .name = "Auto-import info", .type = CompilerOutputLineType::info, .fileNameIdx = 0, .lineIdx = 0, .messageIdx = 1};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    //<![CDATA[([][{}()[:blank:]#%$~[:alnum:]!&_:+/\\\.-]+):[[:blank:]]+(duplicate section.*has different size)]]>
    else if (auto m = ctre::match<"([{}()[:blank:]#%$~[:alnum:]!&_:+/\\\\\\.\\-]+):[[:blank:]]+(duplicate section.*has different size)">(line))
    {
        static constexpr CompilerRegexInfo compilerRegexInfo = {.name = "Linker warning (different sized sections)",
                                                                .type = CompilerOutputLineType::warning,
                                                                .fileNameIdx = 1,
                                                                .lineIdx = 0,
                                                                .messageIdx = 2};
        POPULATE_INFO(ret, m, compilerRegexInfo);
    }
    else
    {
        ret.type = CompilerOutputLineType::normal;
        /*std::cout << "Match failed!\n";*/
    }
    return ret;
}

#endif  // COMPILER_OUTPUT_PARSER_HPP_INCLUDED
