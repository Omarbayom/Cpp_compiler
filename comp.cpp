#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>

using namespace std;

struct T {
    string id;
    string type;
    int level;
};
vector<T> Tokens;
string com = "";

bool isOperator(const string& code) {

    regex opRegex("(=|\\+=|-=|\\*=|/=|%=|\\^=|&=|\\|=|<<=|>>=|==|!=|>|<|>=|<=|&&|\\|\\||!|\\+\\+|--|~)");
    return regex_match(code, opRegex);

}
bool aOperator(const string& code) {
    regex opRegex("(\\+|-|\\*|/|%|\\^|&|\\||<<|>>)");
    return regex_match(code, opRegex);

}

bool detectMemberAccess(const string& input) {

    regex pattern(R"(\[|\]|\*|&|\.|->)");
    return regex_match(input, pattern);
}

bool KeywordRegex(const string& pattern) {
    static const regex keywords_regex(
        "(auto|break|case|char|const|continue|default|do|double|else|enum|"
        "extern|float|for|goto|if|inline|int|long|register|restrict|return|"
        "short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|"
        "volatile|while)"
    );
    return regex_match(pattern, keywords_regex);
}

bool isID(const string& str) {
    regex idRegex("^[a-zA-Z_][a-zA-Z0-9_]*");
    return regex_match(str, idRegex);
}

bool isComment(const string& str)
{
    return str == "/* ... */" || str == "//";
}
bool special(const string& str)
{
    regex pattern("\\\\[nrtbvaf]");
    return regex_match(str, pattern);
}

bool isDigit(const string& str) {
    //float x = .5;
    //float x = 1.5;
    regex fRegex("(\\+|-)?(\\d+)?(\\.\\d+)");
    //float x = 1.;
    //float x = 1.5;
    regex decimalRegex("(\\+|-)?\\d+(\\.\\d*)?");
    regex octalRegex("(\\+|-)?0[0-7]+");
    regex hexadecimalRegex("(\\+|-)?0[xX][0-9a-fA-F]+");
    regex binaryRegex("(\\+|-)?0[bB][01]+");

    return regex_match(str, decimalRegex) || regex_match(str, octalRegex) ||
        regex_match(str, hexadecimalRegex) || regex_match(str, binaryRegex) || regex_match(str, fRegex);
}

bool isString(const string& str)
{
    regex pattern("'((.)|\\\\[nrtbvaf])'");
    return (str[0] == '"' && str[str.size() - 1] == '"') || (regex_match(str, pattern));
}

bool isExponential(const string& str) {
    regex exponentialRegex("(\\+|-)?(\\d*(\\.\\d+))([eE][-|\\+]?\\d+)?");
    regex exponentialRegex2("(\\+|-)?(\\d+(\\.\\d*)?)([eE][-|\\+]?\\d+)?");
    return regex_match(str, exponentialRegex) || regex_match(str, exponentialRegex2);
}

bool isLiteral(const string& str)
{
    return isExponential(str) || isDigit(str) || isString(str);
}

bool isSeparator(const string& str)
{
    const vector<string> Separators{ "{", "}", ",", "(", ")",";","?",":"};
    for (const auto& separate : Separators)
        if (separate == str)
            return true;

    return false;
}

bool isNotLegal(const string& str)
{
    return str == " " || str == "\n"||str=="\t";
}

void printRoleOfToken(const string& token)
{
    T woh;
    if (isComment(token)) {
        if (com[com.size() - 1] == '\n')
            com.erase(com.size() - 1);
        woh = { com,"com" };

    }
    else if (isOperator(token)) {
        woh = { token,"op" };
    }
    else if (aOperator(token)) {
        woh = { token,"aop" };

    }
    else if (detectMemberAccess(token)) {
        woh = { token,"ma" };
    }
    else if (isSeparator(token)) {
        woh = { token,"sp" };
    }
    else if (KeywordRegex(token)) {
        woh = { token,"kw" };
    }
    else if (isLiteral(token)) {
        woh = { token,"li" };
    }
    else if (isID(token)) {
        woh = { token,"id" };
    }
    else {
        if (token[0] == '"')
            woh = { token + '"',"inv" };
        else
            woh = { token ,"inv" };

    }
    if (token != "") {
        Tokens.push_back(woh);
    }
}

void lexicalAnalyze(const string& nameOfFile)
{
    char ch;
    string buffer;
    istringstream file(nameOfFile);



    bool multiCm = false, singleCm = false;

    while (file >> noskipws >> ch)
    {
        if (multiCm) {
            com += ch;
            if (ch == '*') {
                char next_ch;
                file >> next_ch;
                if (next_ch == '/') {
                    multiCm = false;
                    com += next_ch;
                    printRoleOfToken("/* ... */");
                    com = "";

                }
                else {
                    file.unget();
                }
            }

            continue;
        }
        if (singleCm) {

            com += ch;
            if (ch == '\n') {
                singleCm = false;
                printRoleOfToken("//");
                com = "";
            }
            continue;
        }
        
        if (ch == '/' && buffer[0] != '"') {
            char next_ch;
            com += ch;
            file >> next_ch;

            if (next_ch == '*') {
                com += next_ch;
                multiCm = true;
                continue;
            }
            else if (next_ch == '/') {

                singleCm = true;
                com += next_ch;
                continue;
            }
            else {
                com = "";
                file.unget();
            }
        }
        
        if (ch == '\\' && (buffer[0] == '"' || buffer[0] == '\'')) {
            string buf;
            char next_ch;
            buf += ch;
            file >> next_ch;
            if (special(buf + next_ch)) {
                    buf += next_ch;
                    T woh = { buf,"sc" };
                    Tokens.push_back(woh);
                    buffer += buf;
                    continue;

            }
            else {
                file.unget();
            }
        }
        if ((ch == '-') && buffer[0] != '"' && buffer[0] != '\'') {
            string buf;
            char next_ch;
            buf += ch;
            file >> next_ch;
            if (detectMemberAccess(buf + next_ch)) {
                buf += next_ch;
                T woh = { buf,"ma" };
                printRoleOfToken(buffer);
                buffer = "";
                Tokens.push_back(woh);
                continue;
            }
            else {
                file.unget();
            }

        }
        if (ch == '.' && buffer[0] != '"' && buffer[0] != '\''&&(isDigit(buffer)||isExponential(buffer)||buffer=="")&&(Tokens[Tokens.size() - 1].type != "id")&&(!isID(buffer))) {
            buffer += ch;
            continue;
           
        }
        if (tolower(ch) == 'e' && buffer[0] != '"' && buffer[0] != '\''&&!isID(buffer)) {
            char next_ch;
            file >> next_ch;
            if ((next_ch == '+') || (next_ch == '-') || (isDigit(string(1, next_ch)))) { 
                buffer += ch;
                buffer += next_ch;
                continue;
            }
            else {
                file.unget();
            }
        }

        if ((ch == '*' || ch == '&') && (Tokens[Tokens.size() - 1].type != "id" &&(!isID(buffer)||KeywordRegex(buffer)) && !isLiteral(buffer) && Tokens[Tokens.size() - 1].type != "li") && buffer != "&" && !aOperator(buffer) && buffer[0] != '"' && buffer[0] != '\'') {
            string buf;
            char next_ch;
            buf += ch;
            file >> next_ch;
            if (isOperator(buf + next_ch)) {
                buf += next_ch;
                T woh = { buf,"op" };
                printRoleOfToken(buffer);
                buffer = "";
                Tokens.push_back(woh);
                continue;
            }
            else {
                file.unget();
                printRoleOfToken(buffer);
                buffer = "";
                T woh = { string(1,ch),"ma" };
                Tokens.push_back(woh);
                continue;


            }

        }
        if (!aOperator(string(1, ch)) && aOperator(buffer) && isOperator(buffer + ch) && buffer[0] != '"' && buffer[0] != '\'')
        {
            buffer += ch;
            printRoleOfToken(buffer);
            buffer = "";
            continue;
        }
        if (aOperator(string(1, ch)) && aOperator(buffer) && isOperator(buffer + ch) && buffer[0] != '"' && buffer[0] != '\'')
        {
            buffer += ch;
            printRoleOfToken(buffer);
            buffer = "";
            continue;
        }
        if (isOperator(string(1, ch)) && isOperator(buffer) && aOperator(buffer + ch) && buffer[0] != '"' && buffer[0] != '\'')
        {
            buffer += ch;
            char next_ch;
            file >> next_ch;
            if (isOperator(buffer + next_ch)) {
                buffer += next_ch;
                printRoleOfToken(buffer);
                buffer = "";
                continue;
            }
            else {
                file.unget();
                printRoleOfToken(buffer);
                buffer = "";
                continue;
            } 
        }
        if (aOperator(string(1, ch)) && aOperator(buffer) && buffer[0] != '"' && buffer[0] != '\'')
        {
                printRoleOfToken(buffer);
                buffer = "";
        }
        if (aOperator(string(1, ch)) && !aOperator(buffer) && buffer[0] != '"' && buffer[0] != '\'')
        {

            if (!buffer.empty())
            {

                printRoleOfToken(buffer);
                buffer = "";
            }
        }


        if (!aOperator(string(1, ch)) && aOperator(buffer) && buffer[0] != '"' && buffer[0] != '\'')
        {
            printRoleOfToken(buffer);
            buffer = "";
        }

        if (isNotLegal(string(1, ch)) && buffer[0] != '"' && buffer[0] != '\'')
        {
            if (!buffer.empty())
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
            continue;
        }
        if (isOperator(string(1, ch)) && !isOperator(buffer) && buffer[0] != '"' && buffer[0] != '\'')
        {
            if (!buffer.empty())
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
        }
        if (isOperator(string(1, ch)) && isOperator(buffer) && !isOperator(buffer + ch) && buffer[0] != '"' && buffer[0] != '\'')
        {
            if (!buffer.empty())
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
        }

        if (!isOperator(string(1, ch)) && isOperator(buffer))
        {
            printRoleOfToken(buffer);
            buffer = "";
        }

        if ((isSeparator(string(1, ch))||detectMemberAccess(string(1, ch))) && buffer[0] != '"' && buffer[0] != '\'')
        {
            if (!buffer.empty())
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
            printRoleOfToken(string(1, ch));
            continue;    
        }
        buffer += ch;
        if (buffer[0] == '\'' && buffer.size() != 1 && ch == '\'') {
            printRoleOfToken(buffer);
            buffer = "";
        }
        if (buffer[0] == '"' && ch == '\n') { 

            while (file >> noskipws >> ch) {
                if (ch == '"')
                    break;
                buffer += ch;

            }
            printRoleOfToken(buffer);
            buffer = "";
        }
        if (ch == '"' && buffer[0] == '"' && buffer.size() != 1 && buffer[buffer.size() - 2] != '\\') {
            printRoleOfToken(buffer);
            buffer = "";
        }
    }
    if (!buffer.empty()) {
        printRoleOfToken(buffer);
    }
    if (!com.empty()) {
        printRoleOfToken(com);
    }
}

string readFileToString(const string& filename) {
    ifstream file(filename);
    stringstream buffr;
    string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            buffr << line << endl;
        }
        file.close();
        return buffr.str();
    }
    else {
        cerr << "Error: Unable to open file " << filename << endl;
        return "";
    }
}
void processTokens(vector<T>& tokens) {
    int level1 = 0;
    auto it = tokens.begin();
    while (it != tokens.end()) {
        if (it->id == "{") {
            level1+=1;
            it->level = level1;
        }
        else if (it->id == "}") {
            it->level = level1;
            level1 -= 1;
        }
        else if (it->type == "inv"||it->type == "com") {
            it = tokens.erase(it); 
            continue; 
        }
        else {
            it->level = level1;
        }
        
        ++it;
    }
}

int main()
{
    string file1 = "C:\\Users\\dell\\Desktop\\Compiler\\project\\comp\\Testfile.txt";
    string fileContents = readFileToString(file1);
    lexicalAnalyze(fileContents);
    processTokens(Tokens);
    for (const auto& token : Tokens)
        cout << "(" << token.id << "," << token.type <<","<<token.level << ")" << endl;
    return 0;
}