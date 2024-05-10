#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <stdexcept>

using namespace std;

struct T {
    string id;
    string type;
    int level;
};
vector<T> Tokens;
vector<T>Symb;
string com = "";
int next_token_index = 0;
T t;
bool sign = true;
bool lon = true;
bool sho = true;
int lonc = 0;
int shoc = 0;
int woh1;

void get_next_token() {
    if (next_token_index < Tokens.size()) {
        T next_token = Tokens[next_token_index];
        next_token_index++;
        t= next_token;
    }
    else {
        t= { "", "", 0 }; 
    }
    cout << t.id << endl;

}

T findTokenById(const string& idToFind, int levelToFind) {
    vector<T> matchingTokens;
    for (const auto& token : Symb) {
        if (token.id == idToFind) {
            if (token.level == levelToFind) {
                return token;
            }
            else {
                matchingTokens.push_back(token);
            }
        }
    }

    if (!matchingTokens.empty()) {
        return matchingTokens.back();
    }

    return T{ "", "", 0 };
}

void SymbolTable(vector<T>& tokens) {
    for (const auto& token : tokens) {
        if (token.type == "id") {
            T foundToken = findTokenById(token.id,token.level);
            if (foundToken.id != "") {
                if (foundToken.level != token.level) {
                    Symb.push_back(token);
                }
            }
            else {
                Symb.push_back(token);
            }
        }
    }
}
void set_next_token() {
    next_token_index = woh1;
    T next_token = Tokens[next_token_index-1];
    t = next_token;
}

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
        "volatile|while|_Alignas|_Alignof|_Atomic|_Bool|_Complex|_Generic|_Imaginary|_Noreturn|_Static_assert|_Thread_local)"
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
    return (str[0] == '"' && str[str.size() - 1] == '"') ;
}

bool ischar(const string& str)
{
    regex pattern("'((.)|\\\\[nrtbvaf])'");
    return  (regex_match(str, pattern));
}

bool isExponential(const string& str) {
    regex exponentialRegex("(\\+|-)?(\\d*(\\.\\d+))([eE][-|\\+]?\\d+)?");
    regex exponentialRegex2("(\\+|-)?(\\d+(\\.\\d*)?)([eE][-|\\+]?\\d+)?");
    return regex_match(str, exponentialRegex) || regex_match(str, exponentialRegex2);
}

bool isLiteral(const string& str)
{
    return isExponential(str) || isDigit(str) || isString(str)||ischar(str);
}

bool isSeparator(const string& str)
{
    const vector<string> Separators{ "{", "}", ",", "(", ")",";","?",":" };
    for (const auto& separate : Separators)
        if (separate == str)
            return true;

    return false;
}

bool isNotLegal(const string& str)
{
    return str == " " || str == "\n" || str == "\t";
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
        if (ch == '.' && buffer[0] != '"' && buffer[0] != '\'' && (isDigit(buffer) || isExponential(buffer) || buffer == "") && (Tokens[Tokens.size() - 1].type != "id") && (!isID(buffer))) {
            buffer += ch;
            continue;

        }
        if (tolower(ch) == 'e' && buffer[0] != '"' && buffer[0] != '\'' && !isID(buffer)) {
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

        if ((ch == '*' || ch == '&') && (Tokens[Tokens.size() - 1].type != "id" && (!isID(buffer) || KeywordRegex(buffer)) && !isLiteral(buffer) && Tokens[Tokens.size() - 1].type != "li") && buffer != "&" && !aOperator(buffer) && buffer[0] != '"' && buffer[0] != '\'') {
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

        if ((isSeparator(string(1, ch)) || detectMemberAccess(string(1, ch))) && buffer[0] != '"' && buffer[0] != '\'')
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

void level(vector<T>& tokens) {
    int level1 = 0;
    auto it = tokens.begin();
    while (it != tokens.end()) {
        if (it->id == "{") {
            level1 += 1;
            it->level = level1;
        }
        else if (it->id == "}") {
            it->level = level1;
            level1 -= 1;
        }else {
            it->level = level1;
        }
        it++;
    }
}
void processTokens(vector<T>& tokens) {
    
    auto it = tokens.begin();
    while (it != tokens.end()) {
        if (it->type == "inv" || it->type == "com") {
            it = tokens.erase(it);
            continue;
        }
        else if (it->type=="sc") {
            it = tokens.erase(it);
            continue;
        }
        

        ++it;
    }
}
bool start();
bool DeclarationList();
bool Declaration();
bool DeclarationList_opt();
bool TypeSpec();
bool Declaration_suffix();
bool Init();
bool InitList();
bool InitList_opt();
bool AssignExpr_suffix();
bool AssignExpr();
bool Expr();
bool Expr_opt();
bool CondExpr_suffix();
bool CondExpr();
bool LogOrExpr();
bool LogOrExpr_suffix();
bool LogAndExpr();
bool LogAndExpr_suffix();
bool InclOrExpr();
bool InclOrExpr_suffix();
bool ExclOrExpr();
bool ExclOrExpr_suffix();
bool AndExpr();
bool AndExpr_suffix();
bool EqExpr();
bool EqExpr_suffix();
bool RelExpr();
bool RelExpr_suffix();
bool ShiftExpr_suffix();
bool ShiftExpr();
bool AddExpr_suffix();
bool AddExpr();
bool MultExpr_suffix();
bool MultExpr();
bool CastExpr();
bool CastExpr_suffix();
bool UnaryExpr();
bool PrimaryExpr();
bool PostfixExpr_suffix();
bool ArgExprList_opt();
bool ArgExprList();
bool Type();
bool AbstractDecl();
bool Const();
bool PostfixExpr();
bool SpecQualList();
bool SpecQualList_opt();
bool TypeQual();
bool DirectAbsDecl();
bool DirectAbsDecl_opt();
bool ParamTypeList();
bool DirectDecl();
bool DirectDecl_suffix();
bool ParamList();
bool FuncDecl();
bool Declarator();
bool Pointer();
bool Pointer_opt();
bool TypeQualList_opt();
bool DirectDecl_suffix_prime();
bool LogicalOrExpr();
bool ConditionalExpr_suffix();
bool LogicalAndExpr();
bool LogicalOrExpr_suffix();
bool InclusiveOrExpr();
bool InclusiveOrExpr_suffix();
bool ExclusiveOrExpr();
bool ConditionalExpr();
bool LogicalAndExpr_suffix();
bool ExclusiveOrExpr_suffix();
bool Stmt();
bool LabeledStmt();
bool CompoundStmt();
bool StmtList();
bool StmtList_opt();
bool JumpStmt();
bool ExprStmt();
bool SelectionStmt();
bool IterationStmt();
bool SelectionStmt_suffix();
bool ExprStmt();
bool ExprStmt_opt();
bool ParamTypeList_suffix();
bool ParamDecl();
bool ParamList_opt();
bool ParamDecl();
bool DeclSpec();
bool AbstractDecl_suffix();
bool AbstractDecl_suffix_prime();
bool StructOrUnionSpec();
bool StructOrUnionSpec_suffix();
bool StructDeclList();
bool StructDeclList_opt();
bool StructDecl();
bool StructDeclaratorList();
bool StructDeclaratorList_opt();
bool StructDeclarator();
bool EnumSpec();
bool EnumSpec_suffix();
bool EnumList();
bool EnumList_opt();
bool Enum();
bool Enum_suffix();


void setTypeSpec() {
    sign = true;
    lon = true;
    sho = true;
    lonc = 0;
    shoc = 0;
}
void Pareser() {
    get_next_token();
    if (t.type == "") {
        return;
    }
    while (t.id != "") {
        start();
    }
}

bool start() {
     woh1 = next_token_index ;
    cout << 0 <<"   " << t.id << endl;
    if (EnumSpec()) {
        cout << 1  << endl;
            return true;
    }
    else if (Declaration()) {
        cout << 2 << endl;
        return true;
    }else if (StructOrUnionSpec()) {
        cout << 6 << endl;
        return true;
    }else if (FuncDecl()) {
        cout << 5 << endl;
        return true;
    }
    else if (Stmt()) {
        cout << 4 << endl;
        return true;
    }
    
    else {
        cout << "error in :" << t.id<<endl;
        get_next_token();
        return false;
    }
}

int cont = 0;
bool DeclarationList() {
    cont++;
    if (Declaration()) {
        DeclarationList_opt(); 
        return true;
    }
    else {
        return false;
    }

   
}


bool Declaration() {
    setTypeSpec();
    if (TypeSpec()) {
        int c = 0;
        while (true) {
            if (Declarator()) {
                c++;
                if (t.id == ",") {
                    get_next_token();
                }
                else if (t.id == ";") {
                    get_next_token();
                    if (c > 1) { // If multiple declarators were parsed, return true
                        return true;
                    }
                    else if (Declaration_suffix()) { // If single declarator, parse Declaration_suffix
                        if (t.id == ";") {
                            get_next_token();
                            return true; // If Declaration_suffix is parsed successfully, return true
                        }
                    }
                }
            }
            else {
                break; // Break the loop if no more declarators found
            }
        }
    }
    return false;
}
bool DeclarationList_opt() {
    if (Declaration()) {
        DeclarationList_opt(); 
    }
    
        return true;
}


bool TypeSpec() {
    string woh = t.id;
    if (woh == "char" || woh == "int" || woh == "float" || woh == "double") {
        get_next_token();
        return true;
    }
    else if (woh == "long" &&lon&&lonc<2) {
        lonc++;
        get_next_token();
        sho = false;
        TypeSpec();
        return true;
    }
    else if (woh == "short"&&sho && shoc < 2) {
        shoc++;
        get_next_token();
        lon = false;
        TypeSpec();
        return true;
    }
    else if ((woh == "signed" || woh == "unsigned") && sign) {
        get_next_token();
        sign = false;
        TypeSpec();
        return true;
    }
    return false;
}
bool Declaration_suffix() {
    if (t.id == "=") {
        get_next_token();
        if (Init()) {
            return true;
        }
        else {
            return false;
        }
    }

    return true; 
}

bool FuncDecl() {
    set_next_token();
    if (TypeSpec()) {
        if (t.type=="id") {
            get_next_token();
            if (t.id == "(") {
                get_next_token(); // Consume '('
                if (ParamList()) {
                    if (t.id == ")") {
                        get_next_token(); // Consume ')'
                        if (CompoundStmt()) {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}




bool ConstExpr() {
    return ConditionalExpr();
}

bool ConditionalExpr() {
    if (LogicalOrExpr()) {
        if (ConditionalExpr_suffix()) {
            return true;
        }
    }
    return false;
}

bool ConditionalExpr_suffix() {
    if (t.id == "?") {
        get_next_token();
        if (AssignExpr()) {
            if (t.id == ":") {
                get_next_token();
                if (ConditionalExpr()) {
                    return true;
                }
            }
        }
        return false;
    }
    return true; // Empty production
}

bool LogicalOrExpr() {
    if (LogicalAndExpr()) {
        if (LogicalOrExpr_suffix()) {
            return true;
        }
    }
    return false;
}

bool LogicalOrExpr_suffix() {
    if (t.id == "||") {
        get_next_token();
        if (LogicalAndExpr()) {
            if (LogicalOrExpr_suffix()) {
                return true;
            }
        }
        return false;
    }
    return true; // Empty production
}

bool LogicalAndExpr() {
    if (InclusiveOrExpr()) {
        if (LogicalAndExpr_suffix()) {
            return true;
        }
    }
    return false;
}

bool LogicalAndExpr_suffix() {
    if (t.id == "&&") {
        get_next_token();
        if (InclusiveOrExpr()) {
            if (LogicalAndExpr_suffix()) {
                return true;
            }
        }
        return false;
    }
    return true; // Empty production
}

bool InclusiveOrExpr() {
    if (ExclusiveOrExpr()) {
        if (InclusiveOrExpr_suffix()) {
            return true;
        }
    }
    return false;
}

bool InclusiveOrExpr_suffix() {
    if (t.id == "|") {
        get_next_token();
        if (ExclusiveOrExpr()) {
            if (InclusiveOrExpr_suffix()) {
                return true;
            }
        }
        return false;
    }
    return true; // Empty production
}

bool ExclusiveOrExpr() {
    if (AndExpr()) {
        if (ExclusiveOrExpr_suffix()) {
            return true;
        }
    }
    return false;
}

bool ExclusiveOrExpr_suffix() {
    if (t.id == "^") {
        get_next_token();
        if (AndExpr()) {
            if (ExclusiveOrExpr_suffix()) {
                return true;
            }
        }
        return false;
    }
    return true; // Empty production
}



// Function to parse a PrintStmt
bool PrintStmt() {
    // Check if the current token is "printf"
    if (t.id == "printf") {
        get_next_token(); // Get the next token
        // Check if the next token is "("
        if (t.id == "(") {
            get_next_token(); // Get the next token
            // Check if the next token is a string literal
            if (isString(t.id)) {
                get_next_token(); // Get the next token
                // Check if the next token is ")"
                if (t.id == ")") {
                    get_next_token(); // Get the next token

                    if (t.id == ";") {
                        get_next_token();
                        return true;
                    }

                }
            }
        }
    }
    return false; 
}





bool Init() {
    if (t.id == "{") {
         get_next_token();
        if (!InitList()) return false;
        if (t.id != "}") {
            return false;
        }
        get_next_token();
        return true;
    }
    else if (AssignExpr()) {
        return true;
    }
    else {
        return false;
    }
}

bool InitList() {
    if (!Init()) return false;
    if (!InitList_opt()) return false;
    return true;
}

bool InitList_opt() {
    if (t.id == ",") {
        get_next_token();
        if (!Init()) return false;
        if (!InitList_opt()) return false;
        return true;
    }
    else {
        return true;
    }
}
bool AssignExpr() {
    if (!CondExpr()) return false;
    if (!AssignExpr_suffix()) return false;
    return true;
}

bool AssignExpr_suffix() {
    if (t.id == "=" || t.id == "*=" || t.id == "/=" ||
        t.id == "%=" || t.id == "+=" || t.id == "-=" ||
        t.id == "<<=" || t.id == ">>=" || t.id == "&=" ||
        t.id == "^=" || t.id == "|=") {

        get_next_token();
        if (!AssignExpr()) return false;
        return true;
    }
    else {
        return true;
    }
}
bool Expr_opt() {
    if (t.id == ",") {
        get_next_token();
        if (!AssignExpr()) return false;
        if (!Expr_opt()) return false;
    }
    return true;
}

bool Expr() {
    if (!AssignExpr()) return false;
    if (!Expr_opt()) return false;
    return true;
}
bool CondExpr_suffix() {
    if (t.id == "?") {
         get_next_token();
        if (!Expr()) return false;
        if (t.id != ":") {
            return false;
        }
        get_next_token();
        if (!CondExpr()) return false;
    }
    return true;
}

bool CondExpr() {
    if (!LogOrExpr()) return false;
    if (!CondExpr_suffix()) return false;
    return true;
}


bool LogOrExpr() {
    if (!LogAndExpr()) return false;
    if (!LogOrExpr_suffix()) return false;
    return true;
}


bool LogOrExpr_suffix() {
    if (t.id == "||") {
        get_next_token();
        if (!LogAndExpr()) return false;
        if (!LogOrExpr_suffix()) return false;
    }
    return true;
}

bool LogAndExpr() {
    if (!InclOrExpr()) return false;
    if (!LogAndExpr_suffix()) return false;
    return true;
}

bool LogAndExpr_suffix() {
    if (t.id == "&&") {
        get_next_token();
        if (!InclOrExpr()) return false;
        if (!LogAndExpr_suffix()) return false;
    }
    return true;
}

bool InclOrExpr() {
    if (!ExclOrExpr()) return false;
    if (!InclOrExpr_suffix()) return false;
    return true;
}

bool InclOrExpr_suffix() {
    if (t.id == "|") {
        get_next_token();
        if (!ExclOrExpr()) return false;
        if (!InclOrExpr_suffix()) return false;
    }
    return true;
}

bool ExclOrExpr() {
    if (!AndExpr()) return false;
    if (!ExclOrExpr_suffix()) return false;
    return true;
}

bool ExclOrExpr_suffix() {
    if (t.id == "^") {
        get_next_token();
        if (!AndExpr()) return false;
        if (!ExclOrExpr_suffix()) return false;
    }
    return true;
}

bool AndExpr() {
    if (!EqExpr()) return false;
    if (!AndExpr_suffix()) return false;
    return true;
}

bool AndExpr_suffix() {
    if (t.id == "&") {
        get_next_token();
        if (!EqExpr()) return false;
        if (!AndExpr_suffix()) return false;
    }
    return true;
}

bool EqExpr() {
    if (!RelExpr()) return false;
    if (!EqExpr_suffix()) return false;
    return true;
}

bool EqExpr_suffix() {
    if (t.id == "==" || t.id == "!=") {
        get_next_token();
        if (!RelExpr()) return false;
        if (!EqExpr_suffix()) return false;
    }
    return true;
}

bool RelExpr() {
    if (!ShiftExpr()) return false;
    if (!RelExpr_suffix()) return false;
    return true;
}

bool RelExpr_suffix() {
    if (t.id == "<" || t.id == ">" || t.id == "<=" || t.id == ">=") {
        get_next_token();
        if (!ShiftExpr()) return false;
        if (!RelExpr_suffix()) return false;
    }
    return true;
}

bool ShiftExpr_suffix() {
    if (t.id == "<<" || t.id == ">>") {
        get_next_token();
        if (!AddExpr()) return false;
        if (!ShiftExpr_suffix()) return false;
    }
    return true;
}

bool ShiftExpr() {
    if (!AddExpr()) return false;
    if (!ShiftExpr_suffix()) return false;
    return true;
}

bool AddExpr_suffix() {
    if (t.id == "+" || t.id == "-") {
        get_next_token();
        if (!MultExpr()) return false;
        if (!AddExpr_suffix()) return false;
    }
    return true;
}

bool AddExpr() {
    if (!MultExpr()) return false;
    if (!AddExpr_suffix()) return false;
    return true;
}

bool MultExpr_suffix() {
    if ((t.id == "*" || t.id == "/" || t.id == "%")&&t.type=="aop") {
        get_next_token();
        if (!CastExpr()) return false;
        if (!MultExpr_suffix()) return false;
    }
    return true;
}

bool MultExpr() {
    if (!CastExpr()) return false;
    if (!MultExpr_suffix()) return false;
    return true;
}

bool CastExpr() {
    if (!UnaryExpr()) return false; 
    if (!CastExpr_suffix()) return false;
    return true;
}

bool CastExpr_suffix() {
    if (t.id == "(") {
        get_next_token();
        if (!Type()) return false; 
        if (t.id != ")") {
            return false;
        }
        get_next_token(); 
        if (!CastExpr()) return false; 
    }
    return true;
}

bool UnaryExpr() {
    if (t.id == "++" || t.id == "--") {
        get_next_token();
        if (!UnaryExpr()) return false;
    }
    else if (t.id == "&" || ((t.id == "*")&&(t.type=="aop")) || t.id == "+" || t.id == "-" || t.id == "~" || t.id == "!") {
        get_next_token();
        if (!CastExpr()) return false;
    }
    else if (t.id == "sizeof") {
        get_next_token();
        if (t.id == "(") {
            get_next_token();
            if (!Type()) return false;
            if (t.id != ")") {
                // Error handling: Expected ')'
                return false;
            }
            get_next_token(); // Move to the next token after ')'
        }
        else {
            if (!UnaryExpr()) return false;
        }
    }
    else if (t.id == "_Alignof") {
        get_next_token();
        if (t.id != "(") {
            // Error handling: Expected '('
            return false;
        }
        get_next_token();
        if (!Type()) return false;
        if (t.id != ")") {
            // Error handling: Expected ')'
            return false;
        }
        get_next_token(); // Move to the next token after ')'
    }
    else {
        if (!PostfixExpr()) return false;
    }
    return true;
}

bool PostfixExpr() {
    if (!PrimaryExpr()) return false;
    if (!PostfixExpr_suffix()) return false;
    return true;
}

bool PostfixExpr_suffix() {
    if (t.id == "[") {
        get_next_token();
        if (!Expr()) return false;
        if (t.id != "]") return false;
        get_next_token();
        if (!PostfixExpr_suffix()) return false;
    }
    else if (t.id == "(") {
        get_next_token();
        if (t.id == ")") get_next_token();
        else {
            if (!ArgExprList_opt()) return false;
            if (t.id != ")") return false;
            get_next_token();
        }
        if (!PostfixExpr_suffix()) return false;
    }
    else if (t.id == ".") {
        get_next_token();
        if (t.type != "id") return false;
        get_next_token();
        if (!PostfixExpr_suffix()) return false;
    }
    else if (t.id == "->") {
        get_next_token();
        if (t.type != "id") return false;
        get_next_token();
        if (!PostfixExpr_suffix()) return false;
    }
    else if (t.id == "++" || t.id == "--") {
        get_next_token();
        if (!PostfixExpr()) return false;
        if (!PostfixExpr_suffix()) return false;
    }
    return true;
}

bool ArgExprList_opt() {
    if (t.id == ",") {
        get_next_token();
        if (!AssignExpr()) return false;
        if (!ArgExprList_opt()) return false;
    }else if (!ArgExprList()) {
         return false;
    }
    
    return true; 
}

bool PrimaryExpr() {
    if (t.type == "id" || Const() || isString(t.id)) {
        get_next_token();
        return true;
    }
    else if (t.id == "(") {
        get_next_token();
        if (!Expr()) return false;
        if (t.id != ")") return false;
        get_next_token();
        return true;
    }
    return false;
}

bool Const() {
    if (isDigit(t.id)||isExponential(t.id) || ischar(t.id) ) {
        return true;
    }
    return false;
}

bool ArgExprList() {
    if (!AssignExpr()) return false;  
    if (!ArgExprList_opt()) return false;  
    return true;
}

bool Type() {
    if (SpecQualList()) {
        if (AbstractDecl()) {
            return true;
        }
        else {
            return true;
        }
    }
    return false;
}


bool TypeQual() {

    if (t.id=="const" || t.id=="volatile" || t.id=="restrict") {
        return true; 
    }
    return false; 
}

bool AbstractDecl() {
    if (DirectAbsDecl()) {
        if (AbstractDecl_suffix()) {
            return true;
        }
    }
    return false;
}

bool AbstractDecl_suffix() {
    if (Pointer()) {
        if (AbstractDecl_suffix_prime()) {
            return true;
        }
    }
    return true; // Empty production
}

bool AbstractDecl_suffix_prime() {
    if (DirectAbsDecl()) {
        if (AbstractDecl_suffix_prime()) {
            return true;
        }
    }
    return true; // Empty production
}


bool DirectAbsDecl() {
    if (t.id == "(") {
        get_next_token();
        if (!AbstractDecl()) return false;
        if (t.id != ")") {
            return false; 
        }
        get_next_token();
    }
    else if (t.id == "[") {
        get_next_token();
        if (t.id == "]") {
            get_next_token();
        }
        else {
            return false; 
        }
    }
    else {
        if (!DirectAbsDecl_opt()) return false;
    }
    return true;
}

bool DirectAbsDecl_opt() {
    if (t.id == "[") {
        get_next_token();
        if (t.id == "]") {
            get_next_token();
        }
        else {
            return false; 
        }
    }
    else if (t.id == "(") {
        get_next_token();
        if (t.id == ")") {
            get_next_token();
        }
        else if (ParamTypeList()) {
            if (t.id != ")") {
                return false; 
            }
            get_next_token();
        }
        else {
            return false; 
        }
    }
    else {
        if (!DirectAbsDecl()) return false;
    }
    return true;
}

bool ParamTypeList() {
   
    if (ParamList()) { 
        if (ParamTypeList_suffix()) {
            return true;
        }
    }
    return false;
}

bool ParamTypeList_suffix() {
    if (t.id == ",") {
        get_next_token();
        if (t.id == "...") {
            get_next_token();
            return true;
        }
        return ParamList();
    }
    return true; // Empty production
}

bool ParamList() {
    if (ParamDecl()) {
        if (ParamList_opt()) {
            return true;
        }
    }
    return true;
}

bool ParamList_opt() {
    if (t.id == ",") {
        get_next_token();
        return ParamDecl();
    }
    return true; // Empty production
}

bool ParamDecl() {
    if (DeclSpec()) {
        if (Declarator()) {
            return true;
        }
    }
    return false;
}

bool DeclSpec() {
    if (TypeSpec()) {
        return true;
    }
    else {
        return SpecQualList();
    }
}


bool Declarator() {
    if (Pointer()) {
        if (DirectDecl()) {
            return true;
        }
    }
    else if (DirectDecl()) {
        return true;
    }
    return false;
}

bool DirectDecl() {
    if (t.type == "id") {
        get_next_token();
        return DirectDecl_suffix();
    }
    return false;
}

bool DirectDecl_suffix() {
    return DirectDecl_suffix_prime();
}

bool DirectDecl_suffix_prime() {
    if (t.id == "[") {

        get_next_token();
        
        if (t.id == "]") {
            get_next_token();
            return DirectDecl_suffix_prime(); // Continue parsing DirectDecl_suffix
        }
        else if (ConstExpr()) {
            if (t.id == "]") {
                get_next_token();
                return DirectDecl_suffix_prime(); // Continue parsing DirectDecl_suffix
            }
        }

        return false;
    }
    else if (t.id == "(") {
        get_next_token();
        if (t.id == ")") {
            get_next_token();
            return DirectDecl_suffix_prime(); // Continue parsing DirectDecl_suffix
        }
        else if (ParamTypeList()) {
            if (t.id == ")") {
                get_next_token();
                return DirectDecl_suffix_prime(); // Continue parsing DirectDecl_suffix
            }
        }
        return false;
    }
    return true; // Empty production
}


bool Pointer() {
    if (t.id == "*" && t.type == "ma") {
        get_next_token();
        TypeQualList_opt();
        Pointer_opt();
        return true;
    }
    return false;
}

bool Pointer_opt() {
    if (t.id == "*"&&t.type=="ma") {
        get_next_token();
        Pointer_opt();
        return true;
    }
    return true; // empty
}

bool TypeQualList() {
    if (TypeQual()) {
        TypeQualList_opt();
        return true;
    }
    return false;
}

bool TypeQualList_opt() {
    if (TypeQualList()) {
        return true;
    }
    return true; // empty
}

bool Stmt() {
    /*set_next_token();*/
    return LabeledStmt() || CompoundStmt() || ExprStmt() || SelectionStmt() || IterationStmt() || JumpStmt()|| PrintStmt();
}

bool LabeledStmt() {
    if (t.type == "id") {
        get_next_token();
        if (t.id == ":") {
            get_next_token();
            if (Stmt()) {
                return true;
            }
        }
    }
    else if (t.id == "case") {
        get_next_token();
        if (ConstExpr()) {
            if (t.id == ":") {
                get_next_token();
                if (Stmt()) {
                    return true;
                }
            }
        }
    }
    else if (t.id == "default") {
        get_next_token();
        if (t.id == ":") {
            get_next_token();
            if (Stmt()) {
                return true;
            }
        }
    }
    return false;
}
bool CompoundStmt() {
    if (t.id == "{") {
        
        get_next_token();
        if (t.id == "}") {
            get_next_token();
            return true; // Empty compound statement {}
        }
        // Check for declaration list followed by statement list

        if (DeclarationList_opt()) {
            if (StmtList()) {
                if (t.id == "}") {
                    get_next_token();
                    return true;
                }
            }
            return false; // Missing closing '}'
        }
        // Check for statement list only
        else if (StmtList()) {
            if (t.id == "}") {
                get_next_token();
                return true;
            }
            return false; // Missing closing '}'
        }
    }
    return false; // Invalid syntax or missing '{'
}


bool StmtList() {
    if (Stmt()) {
        if (StmtList_opt()) {
            return true;
        }
    }
    return false;
}

bool StmtList_opt() {
    if (Stmt()) {
        if (StmtList_opt()) {
            return true;
        }
    }
    return true; // empty
}

bool ExprStmt() {
    if (t.id == ";") {
        get_next_token();
        return true;
    }
    else if (Expr()) {
        if (t.id == ";") {
            get_next_token();
            return true;
        }
    }
    return false;
}

bool SelectionStmt() {
    if (t.id == "if") {
        get_next_token();
        if (t.id == "(") {
            get_next_token();
            if (Expr()) {
                if (t.id == ")") {
                    get_next_token();
                    if (Stmt()) {
                        if (SelectionStmt_suffix()) {
                            return true;
                        }
                        return true; // No 'else' part
                    }
                }
            }
        }
    }
    return false;
}

bool SelectionStmt_suffix() {
    if (t.id == "else") {
        get_next_token();
        if (Stmt()) {
            return true;
        }
    }
    return true; // Empty production
}

bool IterationStmt() {
    if (t.id == "while") {
        get_next_token();
        if (t.id == "(") {
            get_next_token();
            if (Expr()) {
                if (t.id == ")") {
                    get_next_token();
                    if (Stmt()) {
                        return true;
                    }
                }
            }
        }
    }
    else if (t.id == "do") {
        get_next_token();
        if (Stmt()) {
            if (t.id == "while") {
                get_next_token();
                if (t.id == "(") {
                    get_next_token();
                    if (Expr()) {
                        if (t.id == ")") {
                            get_next_token();
                            if (t.id == ";") {
                                get_next_token();
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    else if (t.id == "for") {
        get_next_token();
        if (t.id == "(") {
            get_next_token();
            if (ExprStmt()) {
                if (ExprStmt_opt()) {
                    if (t.id == ";") {
                        get_next_token();
                        if (Expr()) {
                            if (t.id == ")") {
                                get_next_token();
                                if (Stmt()) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool ExprStmt_opt() {
    if (Expr()) {
        return true;
    }
    return true; // Empty production
}

bool JumpStmt() {
    if (t.id == "goto") {
        get_next_token();
        if (t.id == "Identifier") {
            get_next_token();
            if (t.id == ";") {
                get_next_token();
                return true;
            }
        }
    }
    else if (t.id == "continue" || t.id == "break") {
        get_next_token();
        if (t.id == ";") {
            get_next_token();
            return true;
        }
    }
    else if (t.id == "return") {
        get_next_token();
        if (Expr()) {
            if (t.id == ";") {
                get_next_token();
                return true;
            }
        }
        else if (t.id == ";") {
            get_next_token();
            return true;
        }
    }
    return false;
}

bool StructOrUnionSpec() {
    set_next_token();
    if (t.id == "struct" || t.id == "union") {
        get_next_token();
        if (t.type == "id") {
            get_next_token();
            if (StructOrUnionSpec_suffix()) {
                return true;
            }
        }
    }
    return false;
}

bool StructOrUnionSpec_suffix() {
    if (t.id == "{") {
        get_next_token();
        if (StructDeclList()) {
            if (t.id == "}") {
                get_next_token();
                if (t.id == ";") {
                    get_next_token();
                    return true;
                }
            }
        }
        else if (t.id == "}") {
            get_next_token();
            if (t.id == ";") {
                get_next_token();
                return true;
            }
        }
    }
    return false;
}

bool StructDeclList() {
    if (StructDecl()) {
        if (StructDeclList_opt()) {
            return true;
        }
    }
    return false;
}

bool StructDeclList_opt() {
    if (StructDecl()) {
        if (StructDeclList_opt()) {
            return true;
        }
    }
    return true; // empty
}

bool StructDecl() {
    if (SpecQualList()) {
        if (StructDeclaratorList()) {
            if (t.id == ";") {
                get_next_token();
                return true;
            }
        }
    }
    return false;
}

bool SpecQualList() {
    if (TypeSpec()) {
        if (SpecQualList_opt()) {
            return true;
        }
    }
    else if (TypeQual()) {
        if (SpecQualList_opt()) {
            return true;
        }
    }
    return false;
}

bool SpecQualList_opt() {
    if (TypeSpec()) {
        if (SpecQualList_opt()) {
            return true;
        }
    }
    else if (TypeQual()) {
        if (SpecQualList_opt()) {
            return true;
        }
    }
    return true; // empty
}

bool StructDeclaratorList() {
    if (StructDeclarator()) {
        if (StructDeclaratorList_opt()) {
            return true;
        }
    }
    return false;
}

bool StructDeclaratorList_opt() {
    if (t.id == ",") {
        get_next_token();
        if (StructDeclarator()) {
            if (StructDeclaratorList_opt()) {
                return true;
            }
        }
        return false;
    }
    return true; // empty
}

bool StructDeclarator() {
    if (Declarator()) {
        return true;
    }
    else if (t.id == ":") {
        get_next_token();
        if (ConstExpr()) {
            return true;
        }
    }
    return false;
}

bool EnumSpec() {
    if (t.id == "enum") {
        get_next_token();
        if (t.type == "id") {
            get_next_token();
            if (EnumSpec_suffix()) {
                return true;
            }
        }
    }
    return false;
}


bool EnumSpec_suffix() {
    if (t.id == "{") {
        get_next_token();
        
        if (EnumList()) {
            
            if (t.id == "}") {
                get_next_token();
                if (t.id == ";") {
                    get_next_token();
                    return true;
                }
            }
        }
        else if (t.id == "}") {
            if (t.id == ";") {
                get_next_token();
                return true;
            }
        }
    }
    return false;
}


bool EnumList() {
    if (Enum()) {
        if (EnumList_opt()) {
            return true;
        }
    }
    return false;
}


bool EnumList_opt() {
    if (t.id == ",") {
        get_next_token();
        if (Enum()) {
            if (EnumList_opt()) {
                return true;
            }
        }
        
    }
    return true; // empty
}


bool Enum() {
    if (t.type=="id") {
        get_next_token();
        if (Enum_suffix()) {
            return true;
        }
    }
    return false;
}


bool Enum_suffix() {
    if (t.id == "=") {
        get_next_token();
        if (ConstExpr()) {
            return true;
        }
        return false;
    }
    return true; // empty
}






int main()
{
    string file1 = "C:\\Users\\dell\\Desktop\\Compiler\\project\\comp\\Testfile1.txt";
    string fileContents = readFileToString(file1);
    lexicalAnalyze(fileContents);
    level(Tokens);
   // processTokens(Tokens);
    SymbolTable(Tokens);
    //Pareser();
    for (const auto& token : Symb)
        cout << "(" << token.id << "," << token.type << "," << token.level << ")" << endl;
    return 0;
}