#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
using namespace std;

// make a differance for * and & in the way that if the buffer holds int then the character is * or & it's a member access

// add the logical statements                  don't know what does it means no more

//*****************************************************************************************
bool isOperator(const string& code) {
    regex opRegex("(=|\\+=|-=|\\*=|/=|%=|\\^=|&=|\\|=|<<=|>>=|==|!=|>|<|>=|<=|&&|\\|\\||! )");
    return regex_match(code, opRegex);

}

bool detectMemberAccess(const string& input) {

    regex pattern(R"(\[|\]|\*|&|\.|->)");
    return regex_search(input, pattern);
}

bool KeywordRegex(string pattern)
{
    if (pattern == "\\bauto \\b")
    {
        return true;
    }
    else if (pattern == "\\bbreak \\b")
    {
        return true;
    }
    else if (pattern == "\\bcase \\b")
    {
        return true;
    }
    else if (pattern == "\\bchar \\b")
    {
        return true;
    }
    else if (pattern == "\\bconst \\b")
    {
        return true;
    }
    else if (pattern == "\\bcontinue \\b")
    {
        return true;
    }
    else if (pattern == "\\bdefault \\b")
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
    return str == "/*" || str == "//";
}

bool isDigit(const string& str) {
    regex digitRegex("^\\d+$");
    return regex_match(str, digitRegex);
}

bool isString(const string& str) 
{
    return str[0] == '"' && str[str.size() - 1] == '"';
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

//bool isOperator(const std::string& str) // woh it works just need to incress the number of operators
//{
//    const vector<std::string> operators{ "<", ">", "<=", ">=", "*", "+", "-", "/", "=", "-=", "*=", "+=", "/=", "++", "--", "==" };
//    for (const auto& op : operators)
//    {
//        if (op == str)
//        {
//            return true;
//        }
//    }
//
//    return false;
//}

bool isSeparator(const string& str) // maybe yes
{
    const vector<string> Separators{ "{", "}", ",", "(", ")", ";" }; //related to the special chars it is missing the following(".","?",":")  Note: they are not considered as separators 
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
    if (isOperator(token))
        cout << "(operator, " << token << ")" << endl;
    else if (isSeparator(token))
        cout << "(separator, " << token << ")" << endl;
    else if (KeywordPattern(token))
        cout << "(keyword, " << token << ")" << endl;
    else if (isStatement(token))
        cout << "(statement, " << token << ")" << endl;
    else if (isLiteral(token))
        cout << "(literal, " << token << ")" << endl;
    else if (isID(token))
        cout << "(identifier, " << token << ")" << endl;
    else if (isComment(token))
        cout << "(comment, " << token << ")" << endl;
    else
        cout<<"Invalid token: " << token<<endl;
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
    bool miltiCm = false, singleCm = false;
    while (file >> noskipws >> ch)
    {
        if (singleCm || miltiCm)
        {
            if (singleCm && ch == '\n')
                singleCm = false;

            if (miltiCm && ch == '*')
            {
                file >> ch;
                if (ch == EOF)
                    break;

                if (ch == '/')
                    miltiCm = false;
            }
            continue;
        }

        if (ch == '/')
        {
            string comm(1, ch);
            file >> ch;
            if (ch == EOF)
            {
                printRoleOfToken(comm);
                break;
            }

            if (ch == '*')
            {
                miltiCm = true;
                comm += ch;
            }
            else if (ch == '/')
            {
                singleCm = true;
                comm += ch;
            }
            if (miltiCm || singleCm)
            {
                printRoleOfToken(comm);
                continue;
            }
        }

        if (isNotLegal(string(1, ch))&&buffer[0]!='"')
        {
            if (!buffer.empty())
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
            continue;
        }
        if (isOperator(string(1, ch)) && !isOperator(buffer)) // try to merge
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

        if (isSeparator(string(1, ch)))
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
    }
    file.close();
}

int main()
{

    lexicalAnalyze("C:\\Users\\dell\\Desktop\\Compiler\\project\\comp\\Cfile.txt");
    return 0;
}
