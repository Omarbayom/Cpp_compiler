#ifndef comp
#define comp

#include <string>
#include <vector>
#include <string>
#include <vector>

struct T
{
    std::string id;
    std::string type;
    int level;
};

class LexicalAnalyzer { 
public:
    std::vector<T> Tokens;
    std::string com;
    bool isOperator(const std::string& code);
    bool aOperator(const std::string& code);
    bool detectMemberAccess(const std::string& input);
    bool KeywordRegex(const std::string& pattern);
    bool isID(const std::string& str);
    bool isComment(const std::string& str);
    bool special(const std::string& str);
    bool isDigit(const std::string& str);
    bool isString(const std::string& str);
    bool isExponential(const std::string& str);
    bool isLiteral(const std::string& str);
    bool isSeparator(const std::string& str);
    bool isNotLegal(const std::string& str);
    void printRoleOfToken(const std::string& token);
    void lexicalAnalyze(const std::string& nameOfFile);
    void processTokens(std::vector<T>& tokens);
    std::string readFileToString(const std::string& filename);
};


#endif // comp
