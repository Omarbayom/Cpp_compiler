#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

//adham and yasser tasks

//*****************************************************************************************
struct T {
    string id;
    string type;
};

vector<T> Tokens;

bool isOperator(const string& code) {
    regex opRegex("(=|\\+=|-=|\\*=|/=|%=|\\^=|&=|\\|=|<<=|>>=|==|!=|>|<|>=|<=|&&|\\|\\||!|\\++|--)");
    return regex_match(code, opRegex);

}

bool detectMemberAccess(const string& input) {

    regex pattern(R"(\[|\]|\*|&|\.|->)");
    return regex_search(input, pattern);
}

bool KeywordRegex(string pattern)
{
    if (pattern == "\\bauto\\b")
    {
        return true;
    }
    else if (pattern == "\\bbreak\\b")
    {
        return true;
    }
    else if (pattern == "\\bcase\\b")
    {
        return true;
    }
    else if (pattern == "\\bchar\\b")
    {
        return true;
    }
    else if (pattern == "\\bconst\\b")
    {
        return true;
    }
    else if (pattern == "\\bcontinue\\b")
    {
        return true;
    }
    else if (pattern == "\\bdefault\\b")
    {
        return true;
    }
    else if (pattern == "\\bdo\\b")
    {
        return true;
    }
    else if (pattern == "\\bdouble\\b")
    {
        return true;
    }
    else if (pattern == "\\belse\\b")
    {
        return true;
    }
    else if (pattern == "\\benum\\b")
    {
        return true;
    }
    else if (pattern == "\\bextern\\b")
    {
        return true;
    }
    else if (pattern == "\\bfloat\\b")
    {
        return true;
    }
    else if (pattern == "\\bfor\\b")
    {
        return true;
    }
    else if (pattern == "\\bgoto\\b")
    {
        return true;
    }
    else if (pattern == "\\bif\\b")
    {
        return true;
    }
    else if (pattern == "\\binline\\b")
    {
        return true;
    }
    else if (pattern == "\\bint\\b")
    {
        return true;
    }
    else if (pattern == "\\blong\\b")
    {
        return true;
    }
    else if (pattern == "\\bregister\\b")
    {
        return true;
    }
    else if (pattern == "\\bRestrict\\b")
    {
        return true;
    }
    else if (pattern == "\\breturn\\b")
    {
        return true;
    }
    else if (pattern == "\\bshort\\b")
    {
        return true;
    }
    else if (pattern == "\\bsigned\\b")
    {
        return true;
    }
    else if (pattern == "\\bsizeof\\b")
    {
        return true;
    }
    else if (pattern == "\\bstatic\\b")
    {
        return true;
    }
    else if (pattern == "\\bstruct\\b")
    {
        return true;
    }
    else if (pattern == "\\bswitch\\b")
    {
        return true;
    }
    else if (pattern == "\\btypedef\\b")
    {
        return true;
    }
    else if (pattern == "\\bunion\\b")
    {
        return true;
    }
    else if (pattern == "\\bunsigned\\b")
    {
        return true;
    }
    else if (pattern == "\\bvoid\\b")
    {
        return true;
    }
    else if (pattern == "\\bvolatile\\b")
    {
        return true;
    }
    else if (pattern == "\\bwhile\\b")
    {
        return true;
    }
    else
        return false;
}

bool KeywordPattern(const string& keyword)
{
    string pattern = "\\b" + keyword + "\\b";
    return KeywordRegex(pattern);
}

bool isID(const string& str) {
    regex idRegex("^[a-zA-Z_][a-zA-Z0-9_]*");
    return regex_match(str, idRegex);
}

bool isComment(const string& str) 
{
    return str == "/* ... */" || str == "//";
}

bool isDigit(const string& str) {
    regex digitRegex("^\\d+$");
    return regex_match(str, digitRegex);
}

bool isString(const string& str) 
{
    regex pattern("'\\s*(.?)\\s*'");
    return (str[0] == '"' && str[str.size() - 1] == '"') || (regex_match(str,pattern));
}

bool isLiteral(const string& str)
{
    return isDigit(str) || isString(str);
}


bool isStatement(const string& str) // we need more
{
    const vector<string> statements{ "for", "while" };
    for (const auto& statement : statements)
        if (statement == str)
            return true;

    return false;
}

bool isSeparator(const string& str) // maybe yes
{
    const vector<string> Separators{ "{", "}", ",", "(", ")", ";",".","?",":" }; //related to the special chars it is missing the following(".","?",":")  Note: they are not considered as separators 
    for (const auto& separate : Separators)
        if (separate == str)
            return true;

    return false;
}

bool isNotLegal(const string& str)
{
    return str == " " || str == "\n";
}

void printRoleOfToken(const string& token) // good el good ma3ada el else
{
    bool valid=true;
    T woh;
    if (isComment(token)) {
        cout << "(comment, " << token << ")" << endl;
        valid = false;
    }
    else if (isOperator(token)) {
        woh = { token,"op" };
    }
    else if (isSeparator(token)) {
        woh = { token,"sp" };
    }
    else if (KeywordPattern(token)) {
        woh = { token,"kw" };
    }
    else if (isLiteral(token)) {
        woh = { token,"li" };
    }
    else if (isID(token)) {
        woh = { token,"id" };
    }else if (detectMemberAccess(token)) {
        woh = { token,"ma" };
    }
    
    else {
        if (token[0]=='"')
        cout << "Invalid token: " << token+'"' << endl;
        else
            cout << "Invalid token: " << token << endl;
        valid = false;
    }
    if (valid)
        Tokens.push_back(woh);
}

void lexicalAnalyze(const string& nameOfFile) // tmam bs 8ir 7war comments
{
    char ch;
    string buffer;
    fstream file(nameOfFile, fstream::in);


    if (!file.is_open())
    {
        cout << "error while opening the file\n";
        exit(0);
    }
    bool multiCm = false, singleCm = false;
    string com;
    while (file >> noskipws >> ch)
    {
        if (multiCm) {
            com +=ch;
            if (ch == '*') {
                char next_ch;
                file >> next_ch;
                if (next_ch == '/') {
                    multiCm = false;
                    com += next_ch;
                    printRoleOfToken("/* ... */");
                    com = "";
                    file >> next_ch;
                    com += next_ch;
                }
                else if (file.eof()) {
                    cout << "Invalid token: " << com << endl;
                    com = "";
                    break;
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
                file.unget(); 
            }
            
        }

        if (isNotLegal(string(1, ch))&&buffer[0]!='"' && buffer[0] != '\'')
        {
            if (!buffer.empty())
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
            continue;
        }
        if (isOperator(string(1, ch)) && !isOperator(buffer) && buffer[0] != '"' && buffer[0] != '\'') // try to merge
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

        if (isSeparator(string(1, ch)) && buffer[0] != '"' && buffer[0] != '\'')
        {
            if (!buffer.empty())
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
            if (isSeparator(string(1, ch)))
            {
                printRoleOfToken(string(1, ch));
                continue;
            }
        }
        
        buffer += ch;
        if (buffer[0] == '\'' && buffer.size() != 1&&ch=='\'') {
            buffer.erase(remove(buffer.begin(), buffer.end(), ' '), buffer.end());
            printRoleOfToken(buffer);
            buffer = "";
        }
        if (buffer[0] == '"' && ch =='\n') {

            while (file >> noskipws >> ch){  
                if (ch == '"')
                    break;
                buffer += ch;
            
            }
            printRoleOfToken(buffer);
                buffer = "";
        }
        if (ch == '"' && buffer[0] == '"' && buffer.size() != 1 && buffer[buffer.size() - 2] != '\\' ) {
            printRoleOfToken(buffer);
            buffer = "";
        }  
       
        
        
    }

    file.close();
}

int main()
{

    lexicalAnalyze("C:\\Users\\dell\\Desktop\\Compiler\\project\\comp\\Cfile.txt");
    for (const auto& token : Tokens)
        cout << "(" << token.id << "," << token.type << ")"<<endl;
    char x = '   g';
    return 0;
}
