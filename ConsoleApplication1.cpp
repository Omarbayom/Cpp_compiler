#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
// make a differance for * and & in the way that if the buffer holds int then the character is * or & it's a member access



//fix the space string issue
//add the logical statements
bool isID(const std::string& str)//regex
{
    if (std::isdigit(str[0]))
        return false;
    int counter = 0;
    if (str[0] == '_')
        counter++;

    for (; counter < str.size(); counter++)
        if (!(isalnum(str[counter])))
            return false;

    return true;
}

bool isComment(const std::string& str)//Don't forget to think about this
{
    return str == "/*" || str == "//";
}

bool isDigit(const std::string& str)//make it with regex
{
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool isString(const std::string& str)//same as the pevious 
{
    return str[0] == '"' && str[str.size() - 1] == '"';
}



bool isLiteral(const std::string& str)
{
    return isDigit(str) || isString(str) ;
}

bool isKeyword(const std::string& str)//more
{
    const vector<std::string> keywords{ "int", "float", "auto", "double", "do", "switch", "return" };
    for (const auto& keyword : keywords)
        if (keyword == str)
            return true;

    return false;
}

bool isStatement(const std::string& str)//we need more 
{
    const vector<std::string> statements{ "for", "while" };
    for (const auto& statement : statements)
        if (statement == str)
            return true;

    return false;
}

bool isOperator(const std::string& str)//woh it works just need to incress the number of operators 
{
    const vector<std::string> operators{ "<", ">", "<=", ">=", "*", "+", "-", "/", "=", "-=", "*=", "+=", "/=", "++", "--", "==" };
    for (const auto& op : operators) {
        if (op == str) {
            return true;
        }
    }

    return false;
}

bool isSeparator(const std::string& str)//maybe yes
{
    const vector<std::string> Separators{ "{", "}", ",", "(", ")", ";" }; // related to the special chars it is missing the following as they are not separators(".","?",":")
    for (const auto& separate : Separators)
        if (separate == str)
            return true;

    return false;
}

bool isNotLegal(const std::string& str)
{
    return str == " " || str == "\n";
}

void printRoleOfToken(const std::string& token)//good el good ma3ada el else
{
    if (isOperator(token))
        cout << "(operator, " << token << ")"<<endl;
    else if (isSeparator(token))
        cout << "(separator, " << token << ")" << endl;
    else if (isKeyword(token))
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
        throw std::runtime_error("Invalid token: " + token);
}

void lexicalAnalyze(const string& nameOfFile)//tmam bs 8ir 7war comments
{
    char ch;
    string buffer;
    fstream file(nameOfFile, std::fstream::in);

    if (!file.is_open())
    {
        cout << "error while opening the file\n";
        exit(0);
    }

    bool miltiCm = false, singleCm = false;
    while (file >> std::noskipws >> ch)
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
            std::string comm(1, ch);
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

        if (isNotLegal(std::string(1, ch)))
        {
            if (!buffer.empty())
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
            continue;
        }

        if (isOperator(std::string(1, ch)) && !isOperator(buffer))//try to merge 
        {
            if (!buffer.empty())
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
        }

        if (!isOperator(std::string(1, ch)) && isOperator(buffer))
        {
            printRoleOfToken(buffer);
            buffer = "";
        }

        if (isSeparator(std::string(1, ch)))
        {
            if (!buffer.empty())
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
            if (isSeparator(std::string(1, ch)))
            {
                printRoleOfToken(std::string(1, ch));
                continue;
            }
        }
        buffer += ch;
    }
    file.close();
}

int main()
{

    lexicalAnalyze("C:\\Users\\dell\\Desktop\\aamar\\ConsoleApplication1\\New Text Document.txt");
    return 0;
}
//#include <fstream>
//#include <iostream>
//#include <stdlib.h>
//#include <string.h>
//#include <ctype.h>
//
//
//using namespace std;
//
//bool isPunctuator(char ch)					//check if the given character is a punctuator or not
//{
//    if (ch == ' ' || ch == '+' || ch == '-' || ch == '*' ||
//        ch == '/' || ch == ',' || ch == ';' || ch == '>' ||
//        ch == '<' || ch == '=' || ch == '(' || ch == ')' ||
//        ch == '[' || ch == ']' || ch == '{' || ch == '}' ||
//        ch == '&' || ch == '|')
//    {
//        return true;
//    }
//    return false;
//}
//
//bool validIdentifier(char* str)						//check if the given identifier is valid or not
//{
//    if (str[0] == '0' || str[0] == '1' || str[0] == '2' ||
//        str[0] == '3' || str[0] == '4' || str[0] == '5' ||
//        str[0] == '6' || str[0] == '7' || str[0] == '8' ||
//        str[0] == '9' || isPunctuator(str[0]) == true)
//    {
//        return false;
//    }									//if first character of string is a digit or a special character, identifier is not valid
//    int i, len = strlen(str);
//    if (len == 1)
//    {
//        return true;
//    }										//if length is one, validation is already completed, hence return true
//    else
//    {
//        for (i = 1; i < len; i++)						//identifier cannot contain special characters
//        {
//            if (isPunctuator(str[i]) == true)
//            {
//                return false;
//            }
//        }
//    }
//    return true;
//}
//
//bool isOperator(char ch)							//check if the given character is an operator or not
//{
//    if (ch == '+' || ch == '-' || ch == '*' ||
//        ch == '/' || ch == '>' || ch == '<' ||
//        ch == '=' || ch == '|' || ch == '&')
//    {
//        return true;
//    }
//    return false;
//}
//
//bool isKeyword(char* str)						//check if the given substring is a keyword or not
//{
//    if (!strcmp(str, "if") || !strcmp(str, "else") ||
//        !strcmp(str, "while") || !strcmp(str, "do") ||
//        !strcmp(str, "break") || !strcmp(str, "continue")
//        || !strcmp(str, "int") || !strcmp(str, "double")
//        || !strcmp(str, "float") || !strcmp(str, "return")
//        || !strcmp(str, "char") || !strcmp(str, "case")
//        || !strcmp(str, "long") || !strcmp(str, "short")
//        || !strcmp(str, "typedef") || !strcmp(str, "switch")
//        || !strcmp(str, "unsigned") || !strcmp(str, "void")
//        || !strcmp(str, "static") || !strcmp(str, "struct")
//        || !strcmp(str, "sizeof") || !strcmp(str, "long")
//        || !strcmp(str, "volatile") || !strcmp(str, "typedef")
//        || !strcmp(str, "enum") || !strcmp(str, "const")
//        || !strcmp(str, "union") || !strcmp(str, "extern")
//        || !strcmp(str, "bool"))
//    {
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
//
//bool isNumber(char* str)							//check if the given substring is a number or not
//{
//    int i, len = strlen(str), numOfDecimal = 0;
//    if (len == 0)
//    {
//        return false;
//    }
//    for (i = 0; i < len; i++)
//    {
//        if (numOfDecimal > 1 && str[i] == '.')
//        {
//            return false;
//        }
//        else if (numOfDecimal <= 1)
//        {
//            numOfDecimal++;
//        }
//        if (str[i] != '0' && str[i] != '1' && str[i] != '2'
//            && str[i] != '3' && str[i] != '4' && str[i] != '5'
//            && str[i] != '6' && str[i] != '7' && str[i] != '8'
//            && str[i] != '9' || (str[i] == '-' && i > 0))
//        {
//            return false;
//        }
//    }
//    return true;
//}
//
//char* subString(char* realStr, int l, int r)				//extract the required substring from the main string
//{
//    int i;
//
//    char* str = (char*)malloc(sizeof(char) * (r - l + 2));
//
//    for (i = l; i <= r; i++)
//    {
//        str[i - l] = realStr[i];
//        str[r - l + 1] = '\0';
//    }
//    return str;
//}
//
//
//void parse(char* str)						//parse the expression
//{
//    int left = 0, right = 0;
//    int len = strlen(str);
//    while (right <= len && left <= right) {
//        if (isPunctuator(str[right]) == false)			//if character is a digit or an alphabet
//        {
//            right++;
//        }
//
//        if (isPunctuator(str[right]) == true && left == right)		//if character is a punctuator
//        {
//            if (isOperator(str[right]) == true)
//            {
//                std::cout << str[right] << " IS AN OPERATOR\n";
//            }
//            right++;
//            left = right;
//        }
//        else if (isPunctuator(str[right]) == true && left != right
//            || (right == len && left != right)) 			//check if parsed substring is a keyword or identifier or number
//        {
//            char* sub = subString(str, left, right - 1);   //extract substring
//
//            if (isKeyword(sub) == true)
//            {
//                cout << sub << " IS A KEYWORD\n";
//            }
//            else if (isNumber(sub) == true)
//            {
//                cout << sub << " IS A NUMBER\n";
//            }
//            else if (validIdentifier(sub) == true
//                && isPunctuator(str[right - 1]) == false)
//            {
//                cout << sub << " IS A VALID IDENTIFIER\n";
//            }
//            else if (validIdentifier(sub) == false
//                && isPunctuator(str[right - 1]) == false)
//            {
//                cout << sub << " IS NOT A VALID IDENTIFIER\n";
//            }
//
//            left = right;
//        }
//    }
//    return;
//}
//
//int main()
//{
//    char c[100] = "int m = n + 3p;\n m=20;";
//    parse(c);
//    return 0;
//}