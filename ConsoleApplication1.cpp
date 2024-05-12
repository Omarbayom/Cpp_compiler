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
int counter = 0;
vector<T> Tokens;
vector<T>Symb;
string com = "";
int currentPos = 0;
T t;
bool sign = true;
bool lon = true;
bool sho = true;
int lonc = 0;
int shoc = 0;
int woh1;

T getToken() {
    if (currentPos < Tokens.size()) {
        return Tokens[currentPos++];
    }
    else {
        return { "END", "",0 };
    }
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
            T foundToken = findTokenById(token.id, token.level);
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
    currentPos = woh1;
    T next_token = Tokens[currentPos - 1];
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
    return (str[0] == '"' && str[str.size() - 1] == '"');
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
    return isExponential(str) || isDigit(str) || isString(str) || ischar(str);
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
        }
        else {
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
        else if (it->type == "sc") {
            it = tokens.erase(it);
            continue;
        }


        ++it;
    }
}


class TreeNode {
public:
    string data;
    vector <TreeNode*> next;
    TreeNode(string value) : data(value) {}
    void addChild(TreeNode* child) {
        next.push_back(child);
    }
};

void Start(TreeNode* node);
TreeNode* Declaration(TreeNode* node);
TreeNode* TypeSpec(TreeNode* node);
TreeNode* StructSpec(TreeNode* node, int x);
TreeNode* StructSuffix(TreeNode* node);
TreeNode* SList(TreeNode* node);
TreeNode* Eq(TreeNode* node);
TreeNode* EqAssign(TreeNode* node);
TreeNode* ArithExpr(TreeNode* node);
TreeNode* Variable(TreeNode* node);
TreeNode* SubArithExpr(TreeNode* node);
TreeNode* UnaryOp(TreeNode* node);
TreeNode* EnumSpec(TreeNode* node, int x);
TreeNode* EnumSpecSuffix(TreeNode* node);
TreeNode* EnumList(TreeNode* node);
TreeNode* EnumListOpt(TreeNode* node);
TreeNode* Enum(TreeNode* node);
TreeNode* EnumSuffix(TreeNode* node);
TreeNode* ArrayDecl(TreeNode* node, int x);
TreeNode* ArrayD(TreeNode* node);
TreeNode* pointerdecl(TreeNode* node, int x);
TreeNode* pointerlist(TreeNode* node);
TreeNode* VariableP(TreeNode* node);
TreeNode* Decl(TreeNode* node, int x);
TreeNode* Variablel(TreeNode* node);
TreeNode* Fundefs(TreeNode* node);
TreeNode* Fundef(TreeNode* node);
TreeNode* Header(TreeNode* node);
TreeNode* ParamList(TreeNode* node);
TreeNode* ParamDecl(TreeNode* node);
TreeNode* ParamListOpt(TreeNode* node);
TreeNode* Stmt(TreeNode* node);
TreeNode* Substmt(TreeNode* node);
TreeNode* Sinstmt(TreeNode* node);
TreeNode* Expr(TreeNode* node);
TreeNode* AssignExpr(TreeNode* node);
TreeNode* AssignOp(TreeNode* node);
TreeNode* BoolExpr(TreeNode* node,int x);
TreeNode* Funcall(TreeNode* node, int x);
TreeNode* Argu(TreeNode* node);
TreeNode* ArgueExpr(TreeNode* node);
TreeNode* ElseExpr(TreeNode* node);
TreeNode* CondExpr1(TreeNode* node);
TreeNode* IterationStmt(TreeNode* node);
TreeNode* Init(TreeNode* node);
TreeNode* Iterat(TreeNode* node);
TreeNode* ReturnStmt(TreeNode* node, int x);
TreeNode* ReturnExpr(TreeNode* node);
TreeNode* DirectDecl(TreeNode* node);
TreeNode* DirectDeclSuffix(TreeNode* node);
TreeNode* DirectDeclSuffixPrime(TreeNode* node);
TreeNode* Pointer(TreeNode* node);
TreeNode* PointerOpt(TreeNode* node);
TreeNode* TypeQualList(TreeNode* node);
TreeNode* TypeQualListOpt(TreeNode* node);
TreeNode* ParamTypeList(TreeNode* node);
TreeNode* ParamTypeListSuffix(TreeNode* node);
TreeNode* ParamListOpt(TreeNode* node);
TreeNode* DeclSpec(TreeNode* node);
TreeNode* Init(TreeNode* node);
TreeNode* InitList(TreeNode* node);
TreeNode* InitListOpt(TreeNode* node);
TreeNode* Designation(TreeNode* node);
TreeNode* DesignatorList(TreeNode* node);
TreeNode* DesignatorListOpt(TreeNode* node);
TreeNode* Designator(TreeNode* node);
TreeNode* Stmt(TreeNode* node);
TreeNode* LabeledStmt(TreeNode* node);
TreeNode* CompoundStmt(TreeNode* node);
TreeNode* StmtList(TreeNode* node);
TreeNode* StmtListOpt(TreeNode* node);
TreeNode* ExprStmt(TreeNode* node);
TreeNode* SelectionStmt(TreeNode* node);
TreeNode* SelectionStmtSuffix(TreeNode* node);
TreeNode* IterationStmt(TreeNode* node, int x);
TreeNode* ExprStmtOpt(TreeNode* node);
TreeNode* JumpStmt(TreeNode* node);
TreeNode* Expr(TreeNode* node, int x);
TreeNode* ExprOpt(TreeNode* node);
TreeNode* AssignExprSuffix(TreeNode* node);
TreeNode* AssignExprOpt(TreeNode* node);
TreeNode* CondExpr(TreeNode* node, int x);
TreeNode* CondExprSuffix(TreeNode* node);
TreeNode* LogOrExpr(TreeNode* node);
TreeNode* LogOrExprSuffix(TreeNode* node);
TreeNode* LogAndExpr(TreeNode* node);
TreeNode* LogAndExprSuffix(TreeNode* node);
TreeNode* InclOrExpr(TreeNode* node);
TreeNode* InclOrExprSuffix(TreeNode* node);
TreeNode* ExclOrExpr(TreeNode* node);
TreeNode* ExclOrExprSuffix(TreeNode* node);
TreeNode* AndExpr(TreeNode* node);
TreeNode* AndExprSuffix(TreeNode* node);
TreeNode* EqExpr(TreeNode* node);
TreeNode* EqExprSuffix(TreeNode* node);
TreeNode* RelExpr(TreeNode* node);
TreeNode* RelExprSuffix(TreeNode* node);
TreeNode* ShiftExpr(TreeNode* node);
TreeNode* ShiftExprSuffix(TreeNode* node);
TreeNode* AddExpr(TreeNode* node);
TreeNode* AddExprSuffix(TreeNode* node);
TreeNode* MultExpr(TreeNode* node);
TreeNode* MultExprSuffix(TreeNode* node);
TreeNode* CastExpr(TreeNode* node);
TreeNode* CastExprSuffix(TreeNode* node);
TreeNode* UnaryExpr(TreeNode* node);
TreeNode* PostfixExpr(TreeNode* node);
TreeNode* PostfixExprSuffix(TreeNode* node);
TreeNode* ArgExprListOpt(TreeNode* node);
TreeNode* PrimaryExpr(TreeNode* node);
TreeNode* Const(TreeNode* node);
TreeNode* IntConst(TreeNode* node);
TreeNode* CharConst(TreeNode* node);
TreeNode* FloatConst(TreeNode* node);
TreeNode* EnumConst(TreeNode* node);
TreeNode* IdentList(TreeNode* node);
TreeNode* IdentListOpt(TreeNode* node);
TreeNode* Type(TreeNode* node);
TreeNode* AbstractDecl(TreeNode* node);
TreeNode* AbstractDeclSuffix(TreeNode* node);
TreeNode* AbstractDeclSuffixPrime(TreeNode* node);
TreeNode* DirectAbsDecl(TreeNode* node);
TreeNode* DirectAbsDeclOpt(TreeNode* node);
TreeNode* ArgExprList(TreeNode* node);
TreeNode* ArgExprListOpt(TreeNode* node);
TreeNode* TypeQual(TreeNode* node);
TreeNode* ConstExpr(TreeNode* node);
TreeNode* ConditionalExpr(TreeNode* node);
TreeNode* BoolOp(TreeNode* node);
TreeNode* assignop(TreeNode* node);
TreeNode* iterat(TreeNode* node);

T currentToken;
TreeNode* Expr(TreeNode* node,int x) {
    TreeNode* exper = nullptr;
    TreeNode* BoolExpr1 = new TreeNode("BoolExpr");
    TreeNode* ArithExpr1 = new TreeNode("ArithExpr");
    TreeNode* AssignExpr1 = new TreeNode("AssignExpr");

    if (BoolExpr(BoolExpr1,x) != nullptr) {
        node->addChild(BoolExpr1);
        exper = BoolExpr1;
    }
    else if (ArithExpr(ArithExpr1) != nullptr) {
        node->addChild(ArithExpr1);
        exper = ArithExpr1;
    }
    else if (AssignExpr(AssignExpr1) != nullptr) {
        node->addChild(AssignExpr1);
        exper = AssignExpr1;
    }

    return exper;
}
bool match(const string& expectedId) {
    currentToken = getToken();
    if (currentToken.id == expectedId) {
        counter = 0;
        return true;
    }
    else if (expectedId == "id" && currentToken.type == "id") {
        counter = 0;
        return true;
    }
    else if (expectedId == "num" && currentToken.type == "li") {
        counter = 0;
        return true;
    }
    else if (expectedId == "string" && currentToken.type == "li") {
        counter = 0;
        return true;
    }
    else if (expectedId == "char" && currentToken.type == "li") {
        counter = 0;
        return true;
    }
    currentPos--;
    counter++;
    if (counter >= 100) {
        return true;
    }
    return false;
}

void printTree(TreeNode* root, int depth = 0, bool lastChild = false) {
    for (int i = 0; i < depth - 1; ++i) {
        cout << (lastChild ? "  " : "| ");
    }

    if (depth > 0) {
        cout << (lastChild ? "`-" : "|-");
    }
    cout << root->data << endl;
    int numChildren = root->next.size();
    for (int i = 0; i < numChildren; ++i) {
        printTree(root->next[i], depth + 1, i == numChildren - 1);
    }
}
TreeNode* root = new TreeNode("Start");
void Pareser() {

    if (currentPos >= Tokens.size()) {
        return;
    }
    while (currentPos < Tokens.size()) {
        if (counter < 10)
            Start(root);
        else {
            currentPos++;
            counter = 0;
        }
    }
    printTree(root);
}

void Start(TreeNode* node) {
    int x = currentPos;
    TreeNode* decl = new TreeNode("Declaration");
    if (Declaration(decl) != nullptr) {
        node->addChild(decl);
    }
    else {
        currentPos = x;

        TreeNode* fun = new TreeNode("Fundefs");
        if (Fundefs(fun) != nullptr)
            node->addChild(fun);
    }
}

TreeNode* Fundefs(TreeNode* node) {
    TreeNode* fundefsNode = nullptr;
    fundefsNode = new TreeNode("fundef");

    if (Fundef(fundefsNode) != nullptr) {
        node->addChild(fundefsNode);
        int x = currentPos;
        TreeNode* fundefNode = new TreeNode("Fundefs");
        if (Fundefs(fundefNode) != nullptr) {
            node->addChild(fundefNode);
        }
        else {
            currentPos = x;
        }

    }
    else {
        fundefsNode = nullptr;
    }
    return fundefsNode;
}


TreeNode* Fundef(TreeNode* node) {
    TreeNode* fundefNode = nullptr;
    fundefNode = new TreeNode("header");

    if (Header(fundefNode)) {
        node->addChild(fundefNode);
        if (match("{")) {
            fundefNode = new TreeNode("{");
            node->addChild(fundefNode);
            while (currentPos + 1 <= Tokens.size() && Tokens[currentPos].id != "}") {
                fundefNode = new TreeNode("Stmt");
                if (Stmt(fundefNode) != nullptr) {
                    node->addChild(fundefNode);
                }
                else {
                    currentPos++;
                }
            }
            if (match("}")) {
                fundefNode = new TreeNode("}");
                node->addChild(fundefNode);
            }
            else {
                fundefNode = nullptr;
            }
        }
        else {
            fundefNode = nullptr;
        }
    }
    else {
        fundefNode = nullptr;
    }
    return fundefNode;
}


TreeNode* Header(TreeNode* node) {
    TreeNode* headerNode = nullptr;
    TreeNode* woh = new TreeNode("void");
    TreeNode* type = new TreeNode("TypeSpec");
    if (match("void")) {

        node->addChild(woh);
    }
    else if (TypeSpec(type) != nullptr) {
        node->addChild(type);
    }
    else {
        return headerNode;
    }
    if (match("id")) {
        headerNode = new TreeNode(currentToken.id);
        node->addChild(headerNode);
        if (match("(")) {
            headerNode = new TreeNode("(");
            node->addChild(headerNode);
            while (currentPos + 1 <= Tokens.size() && Tokens[currentPos].id != ")") {
                headerNode = new TreeNode("ParamList");
                if (ParamList(headerNode) != nullptr) {
                    node->addChild(headerNode);
                }
            }
            if (match(")")) {
                headerNode = new TreeNode(")");
                node->addChild(headerNode);
            }
            else {
                headerNode = nullptr;
            }
        }
        else {
            headerNode = nullptr;
        }
    }
    else {
        headerNode = nullptr;
    }


    return headerNode;
}

TreeNode* ParamList(TreeNode* node) {

    TreeNode* paramListNode = nullptr;

    paramListNode = new TreeNode("ParamDecl");

    if (ParamDecl(paramListNode) != nullptr) {
        node->addChild(paramListNode);
    }
    TreeNode* paramListNode1 = new TreeNode("ParamListOpt");
    if (ParamListOpt(paramListNode1) != nullptr) {
        node->addChild(paramListNode1);
    }
    

    return paramListNode;
}

TreeNode* ParamDecl(TreeNode* node) {
    TreeNode* paramDeclNode = nullptr;
    paramDeclNode = new TreeNode("TypeSpec");
    if (TypeSpec(paramDeclNode) != nullptr) {
        node->addChild(paramDeclNode);
        if (match("id")) {
            paramDeclNode = new TreeNode(currentToken.id);
            node->addChild(paramDeclNode);
        }
        else {
            paramDeclNode = nullptr;
        }
    }
    else {
        paramDeclNode = nullptr;
    }

    return paramDeclNode;
}

TreeNode* ParamListOpt(TreeNode* node) {
    TreeNode* paramListOptNode = nullptr;

    if (match(",")) {
        paramListOptNode = new TreeNode(",");
        node->addChild(paramListOptNode);
        paramListOptNode = new TreeNode("ParamDecl");
        if (ParamDecl(paramListOptNode) != nullptr) {
            node->addChild(paramListOptNode);
        }
        paramListOptNode = new TreeNode("ParamListOpt");
        if (ParamListOpt(paramListOptNode) != nullptr) {
            node->addChild(paramListOptNode);
        }

    }

    return paramListOptNode;
}
TreeNode* Stmt(TreeNode* node) {
    TreeNode* stmtNode = nullptr;
    stmtNode = new TreeNode("Sinstmt");
    if (Sinstmt(stmtNode) != nullptr) {
        node->addChild(stmtNode);
        stmtNode = new TreeNode("Substmt");
        if (Substmt(stmtNode) != nullptr) {
            node->addChild(stmtNode);
        }
    }
    else {
        stmtNode = nullptr;
    }
    return stmtNode;
}

TreeNode* Substmt(TreeNode* node) {
    TreeNode* substmtNode = nullptr;
    substmtNode = new TreeNode("Stmt");
    if (Stmt(substmtNode) != nullptr) {
        node->addChild(substmtNode);
    }
    else {
        substmtNode = nullptr;
    }

    return substmtNode;
}

TreeNode* Sinstmt(TreeNode* node) {
    TreeNode* sinstmtNode = nullptr;
    int x = currentPos;
    TreeNode* expr1 = new TreeNode("expr");
    TreeNode* condexpr1 = new TreeNode("condexpr");
    TreeNode* IterationStmt1 = new TreeNode("IterationStmt");
    TreeNode* funcall1 = new TreeNode("funcall");
    TreeNode* returnstmt1 = new TreeNode("returnstmt");
    TreeNode* ArrayDecl1 = new TreeNode("ArrayDecl");
    TreeNode* pointerdecl1 = new TreeNode("pointerdecl");
    TreeNode* Decl1 = new TreeNode("Decl");

    if (Expr(expr1,x) != nullptr) {
        node->addChild(expr1);
        sinstmtNode = expr1;
    }
    /*else if (CondExpr(condexpr1,x) != nullptr) {
        node->addChild(condexpr1);
        sinstmtNode = condexpr1;
    }
    else */if (IterationStmt(IterationStmt1,x) != nullptr) {
        node->addChild(IterationStmt1);
        sinstmtNode = IterationStmt1;
    }
    else if (Funcall(funcall1, x) != nullptr) {
        node->addChild(funcall1);
        sinstmtNode = funcall1;
    }
    else if (ReturnStmt(returnstmt1, x) != nullptr) {
        node->addChild(returnstmt1);
        sinstmtNode = returnstmt1;
    }
    else if (ArrayDecl(ArrayDecl1, x) != nullptr) {
        node->addChild(ArrayDecl1);
        sinstmtNode = ArrayDecl1;
    }
    else if (pointerdecl(pointerdecl1, x) != nullptr) {
        node->addChild(pointerdecl1);
        sinstmtNode = pointerdecl1;
    }
    else if (Decl(Decl1, x) != nullptr) {
        node->addChild(Decl1);
        sinstmtNode = Decl1;
    }

    return sinstmtNode;
}

TreeNode* ReturnStmt(TreeNode* node, int x) {
    TreeNode* returnStmtNode = nullptr;
    currentPos = x;
    if (match("return")) {
        returnStmtNode = new TreeNode("return");
        node->addChild(returnStmtNode);

        returnStmtNode = new TreeNode("returnExpr");
        if (ReturnExpr(returnStmtNode) != nullptr) {
            node->addChild(returnStmtNode);

        }if (match(";")) {
            returnStmtNode = new TreeNode(";");
            node->addChild(returnStmtNode);
        }
        else {
            returnStmtNode = nullptr;
        }
    }

    return returnStmtNode;
}

TreeNode* ReturnExpr(TreeNode* node) {
    TreeNode* returnExprNode = nullptr;
    TreeNode* BoolExpr1 = new TreeNode("boolexpr");
    TreeNode* ArithExpr1 = new TreeNode("arithexpr");
    TreeNode* Variable1 = new TreeNode("variable");
    int x = currentPos;
    if (match("1") || match("0")) {
        returnExprNode = new TreeNode(currentToken.id);
        node->addChild(returnExprNode);
    }
    else if (BoolExpr(BoolExpr1, x) != nullptr) {
        node->addChild(BoolExpr1);
        returnExprNode = BoolExpr1;

    }else if (Variable(Variable1) != nullptr) {
        node->addChild(Variable1);
        returnExprNode = Variable1;
    }
    else if (ArithExpr(ArithExpr1) != nullptr) {
        node->addChild(ArithExpr1);
        returnExprNode = ArithExpr1;
    }
    


    return returnExprNode;
}

TreeNode* Funcall(TreeNode* node, int x) {
    TreeNode* funcallNode = nullptr;
    currentPos = x;
    if (match("id")) {
        funcallNode = new TreeNode(currentToken.id);
        node->addChild(funcallNode);
        if (match("(")) {
            funcallNode = new TreeNode("(");
            node->addChild(funcallNode);

            funcallNode = new TreeNode("Argu");
            int x = currentPos;
            if (Argu(funcallNode) != nullptr) {

                node->addChild(funcallNode);
            }
            else {
                currentPos = x;
            }

            if (match(")")) {
                funcallNode = new TreeNode(")");
                node->addChild(funcallNode);
                if (match(";")) {
                    funcallNode = new TreeNode(";");
                    node->addChild(funcallNode);
                }
                else {
                    funcallNode = nullptr;
                }
            }
            else {
                funcallNode = nullptr;
            }
        }
        else {
            funcallNode = nullptr;
        }
    }


    return funcallNode;
}

TreeNode* Argu(TreeNode* node) {
    TreeNode* arguNode = nullptr;

    arguNode = new TreeNode("arguexpr");
    if (ArgueExpr(arguNode) != nullptr) {
        node->addChild(arguNode);


        if (match(",")) {
            arguNode = new TreeNode("Argu");
            if (Argu(arguNode) != nullptr) {
                node->addChild(arguNode);
            }

        }
    }
    else {
        arguNode = nullptr;
    }

    return arguNode;
}

TreeNode* ArgueExpr(TreeNode* node) {
    TreeNode* argueExprNode = nullptr;
    TreeNode* BoolExpr1 = new TreeNode("boolexpr");
    TreeNode* ArithExpr1 = new TreeNode("arithexpr");
    TreeNode* Variable1 = new TreeNode("variable");
    int x = currentPos;
    
     if (BoolExpr(BoolExpr1, x) != nullptr) {
        node->addChild(BoolExpr1);
        argueExprNode = BoolExpr1;
    }
    else if (Variable(Variable1) != nullptr) {
        node->addChild(Variable1);
        argueExprNode = Variable1;
    }else if (ArithExpr(node) != nullptr) {
        node->addChild(ArithExpr1);
        argueExprNode = ArithExpr1;
    }


    return argueExprNode;
}

TreeNode* BoolExpr(TreeNode* node, int x) {
    TreeNode* boolExprNode = nullptr;
    boolExprNode = new TreeNode("Variable");
    currentPos = x;
    if (Variable(boolExprNode) != nullptr) {
        node->addChild(boolExprNode);
    }
    boolExprNode = new TreeNode("boolExpr");
    if (BoolOp(boolExprNode) != nullptr) {
        node->addChild(boolExprNode);

        boolExprNode = new TreeNode("Variable");
        if (Variable(boolExprNode) != nullptr) {
            node->addChild(boolExprNode);
        }
        else {
            boolExprNode = nullptr;
        }
    }
    else {
        currentPos = x;
        boolExprNode = nullptr;
    }


    return boolExprNode;
}

TreeNode* BoolOp(TreeNode* node) {
    TreeNode* boolOpNode = nullptr;

    if (match("==") || match("!=") || match(">=") || match("<=") ||
        match(">") || match("<") || match("!") || match("&&") ||
        match("||")) {
        boolOpNode = new TreeNode(currentToken.id);
        node->addChild(boolOpNode);
    }
    else {

        boolOpNode = nullptr;
    }

    return boolOpNode;
}


TreeNode* Declaration(TreeNode* node) {
    int x = currentPos;
    TreeNode* declarationNode = nullptr;
    TreeNode* EnumSpec1 = new TreeNode("EnumSpec");
    TreeNode* StructSpec1 = new TreeNode("StructSpec");
    TreeNode* ArrayDecl1 = new TreeNode("ArrayDecl");
    TreeNode* pointerdecl1 = new TreeNode("pointerdecl");
    TreeNode* Decl1 = new TreeNode("Decl");

    if (EnumSpec(EnumSpec1, x) != nullptr) {
        node->addChild(EnumSpec1);
        declarationNode = EnumSpec1;
    }
    else if (StructSpec(StructSpec1, x) != nullptr) {

        node->addChild(StructSpec1);
        declarationNode = StructSpec1;
    }
    else if (ArrayDecl(ArrayDecl1, x) != nullptr) {

        node->addChild(ArrayDecl1);
        declarationNode = ArrayDecl1;
    }
    else if (pointerdecl(pointerdecl1, x) != nullptr) {

        node->addChild(pointerdecl1);
        declarationNode = pointerdecl1;
    }
    else if (Decl(Decl1, x) != nullptr) {

        node->addChild(Decl1);
        declarationNode = Decl1;
    }

    return declarationNode;
}

TreeNode* EnumSpec(TreeNode* node, int x) {
    TreeNode* enumSpecNode = nullptr;
    currentPos = x;

    if (match("enum")) {
        enumSpecNode = new TreeNode("enum");
        node->addChild(enumSpecNode);
        if (match("id")) {
            enumSpecNode = new TreeNode(currentToken.id);
            node->addChild(enumSpecNode);
            enumSpecNode = new TreeNode("EnumSpec_suffix");
            if (EnumSpecSuffix(enumSpecNode) != nullptr) {
                node->addChild(enumSpecNode);
                if (match(";")) {
                    enumSpecNode = new TreeNode(";");
                    node->addChild(enumSpecNode);
                }
            }
            else {
                enumSpecNode = nullptr;
            }
        }

    }


    return enumSpecNode;
}



TreeNode* EnumSpecSuffix(TreeNode* node) {
    TreeNode* enumSpecSuffixNode = nullptr;

    if (match("{")) {
        enumSpecSuffixNode = new TreeNode("{");
        node->addChild(enumSpecSuffixNode);

        enumSpecSuffixNode = new TreeNode("EnumList");
        if (EnumList(enumSpecSuffixNode) != nullptr) {

            node->addChild(enumSpecSuffixNode);
        }
        if (match("}")) {
            enumSpecSuffixNode = new TreeNode("}");
            node->addChild(enumSpecSuffixNode);
        }

    }



    return enumSpecSuffixNode;
}

TreeNode* EnumList(TreeNode* node) {
    TreeNode* enumListNode = new TreeNode("Enum");
    if (Enum(enumListNode) != nullptr) {

        node->addChild(enumListNode);

        enumListNode = new TreeNode("EnumListopt");
        EnumListOpt(enumListNode);
        node->addChild(enumListNode);
    }
    else {
        enumListNode = nullptr;
    }

    return enumListNode;
}

TreeNode* EnumListOpt(TreeNode* node) {
    TreeNode* commaNode = new TreeNode(",");
    if (match(",")) {
        node->addChild(commaNode);

        TreeNode* commaNode = new TreeNode("Enum");
        if (Enum(commaNode) != nullptr) {
            node->addChild(commaNode);

            commaNode = new TreeNode("EnumListopt");
            if (EnumListOpt(commaNode) != nullptr)
                node->addChild(commaNode);

        }
    }


    return node;
}

TreeNode* Enum(TreeNode* node) {
    TreeNode* enumNode = nullptr;

    if (match("id")) {
        enumNode = new TreeNode(currentToken.id);
        node->addChild(enumNode);
        enumNode = new TreeNode("EnumSuffix");
        EnumSuffix(enumNode);
        node->addChild(enumNode);
    }


    return enumNode;
}

TreeNode* EnumSuffix(TreeNode* node) {
    TreeNode* enumSuffixNode = nullptr;

    if (match("=")) {
        TreeNode* assignNode = new TreeNode("=");
        node->addChild(assignNode);

        if (match("num")) {
            TreeNode* digitNode = new TreeNode(currentToken.id);
            node->addChild(digitNode);
        }

    }


    return enumSuffixNode;
}

TreeNode* StructSpec(TreeNode* node, int x) {
    TreeNode* structSpecNode = new TreeNode("Structe");
    currentPos = x;
    if (match("struct")) {
        node->addChild(structSpecNode);

        if (match("id")) {
            TreeNode* structSpecNode = new TreeNode(currentToken.id);
            node->addChild(structSpecNode);
        }
        else {
            structSpecNode = nullptr;
            return structSpecNode;
        }

        TreeNode* structSuffixNode = new TreeNode("structSpecNode");
        StructSuffix(structSuffixNode);
        node->addChild(structSuffixNode);

        if (match(";")) {
            structSpecNode = new TreeNode(";");
            node->addChild(structSpecNode);
        }
        else {
            structSpecNode = nullptr;
            return structSpecNode;
        }
    }
    else {
        structSpecNode = nullptr;
    }

    return structSpecNode;
}

TreeNode* StructSuffix(TreeNode* node) {
    TreeNode* structSuffixNode = new TreeNode("Struct_suffix");

    if (match("{")) {
        node->addChild(structSuffixNode);
        TreeNode* openBraceNode = new TreeNode("{");
        node->addChild(openBraceNode);

        TreeNode* sListNode = new TreeNode("sListNode");
        SList(structSuffixNode);
        node->addChild(sListNode);

        if (match("}")) {
            TreeNode* closeBraceNode = new TreeNode("}");
            node->addChild(closeBraceNode);
        }
    }
    else {
        structSuffixNode = nullptr;
        return structSuffixNode;
    }

    return structSuffixNode;
}

TreeNode* TypeSpec(TreeNode* node) {
    TreeNode* typeSpecNode = nullptr;
    if (match("char")) {
        typeSpecNode = new TreeNode("char");
        node->addChild(typeSpecNode);
    }
    else if (match("signed") || match("unsigned")) {
        typeSpecNode = new TreeNode(currentToken.id);
        node->addChild(typeSpecNode);
        if (match("char")) {
            typeSpecNode = new TreeNode("char");
            node->addChild(typeSpecNode);

        }

    }
    else if (match("short") || match("long")) {
        cout << currentToken.id << endl;
        typeSpecNode = new TreeNode(currentToken.id);
        node->addChild(typeSpecNode);
        if (match("int")) {
            typeSpecNode = new TreeNode("int");
            node->addChild(typeSpecNode);
            if (match("short") || match("long")) {
                typeSpecNode = new TreeNode(currentToken.id);
                node->addChild(typeSpecNode);
            }
        }

    }
    else if (match("_Bool") || match("float") || match("double")) {
        typeSpecNode = new TreeNode(currentToken.id);
        node->addChild(typeSpecNode);
    }
    else if (match("int")) {
        typeSpecNode = new TreeNode("int");
        node->addChild(typeSpecNode);

    }
    return typeSpecNode;
}


TreeNode* SList(TreeNode* node) {
    TreeNode* sListNode = nullptr;

    sListNode = new TreeNode("TypeSpec");
    if (TypeSpec(sListNode) != nullptr) {
        node->addChild(sListNode);
        sListNode = new TreeNode("Eq");

        if (Eq(sListNode) != nullptr) {
            node->addChild(sListNode);
        }


        if (match("id")) {
            TreeNode* identifierNode = new TreeNode(currentToken.id);
            node->addChild(identifierNode);
        }
        else {
            sListNode = nullptr;
            return sListNode;
        }

        if (match(";")) {
            sListNode = new TreeNode(";");
            node->addChild(sListNode);
        }
        else {
            sListNode = nullptr;
            return sListNode;
        }
        sListNode = new TreeNode("sListNode");
        SList(sListNode);
        node->addChild(sListNode);

    }
    else {
        sListNode = nullptr;
        return sListNode;
    }

    return sListNode;
}

TreeNode* Eq(TreeNode* node) {
    TreeNode* eqNode = nullptr;

    if (match("=")) {
        eqNode = new TreeNode("=");
        node->addChild(eqNode);
        eqNode = new TreeNode("EgAssign");
        EqAssign(eqNode);
        if (eqNode != nullptr) {
            node->addChild(eqNode);
        }
    }


    return eqNode;
}

TreeNode* EqAssign(TreeNode* node) {
    TreeNode* eqAssignNode = nullptr;

    if (match("num") || match("id") || match("string") || match("char")) {
        eqAssignNode = new TreeNode(currentToken.id);
        node->addChild(eqAssignNode);
    }
    else if (match("arithexpr")) {
        eqAssignNode = new TreeNode("arithexpr");
        ArithExpr(eqAssignNode);
        node->addChild(eqAssignNode);

    }


    return eqAssignNode;
}

TreeNode* ArithExpr(TreeNode* node) {
    TreeNode* arithExprNode = nullptr;
    arithExprNode = new TreeNode("Variable");
    if (Variable(arithExprNode) != nullptr) {

        node->addChild(arithExprNode);

        arithExprNode = new TreeNode("SubArithExpr");
        if (SubArithExpr(arithExprNode) != nullptr)
            node->addChild(arithExprNode);

    }
    else {
        arithExprNode = nullptr;

        return arithExprNode;
    }

    return arithExprNode;
}

TreeNode* Variable(TreeNode* node) {
    TreeNode* variableNode = nullptr;

    if (match("num") || match("id")) {
        variableNode = new TreeNode(currentToken.id);
        node->addChild(variableNode);
    }


    return variableNode;
}

TreeNode* SubArithExpr(TreeNode* node) {
    TreeNode* subArithExprNode = new TreeNode("Unaryop");

    if (UnaryOp(subArithExprNode)) {

        node->addChild(subArithExprNode);

        subArithExprNode = new TreeNode("ArithExpr");
        if (ArithExpr(subArithExprNode) != nullptr)
            node->addChild(subArithExprNode);

    }
    else {
        subArithExprNode = nullptr;
        return subArithExprNode;
    }

    return subArithExprNode;
}

TreeNode* UnaryOp(TreeNode* node) {
    TreeNode* unaryOpNode = nullptr;

    if (match("&") || match("*") || match("+") || match("-") || match("~") || match("!") ||
        match("%") || match("/") || match("^") || match("<<") || match(">>") || match("|")) {
        unaryOpNode = new TreeNode(currentToken.id);
        node->addChild(unaryOpNode);
    }


    return unaryOpNode;
}


TreeNode* ArrayDecl(TreeNode* node, int x) {
    TreeNode* arrayDeclNode = nullptr;
    currentPos = x;
    arrayDeclNode = new TreeNode("TypeSpec");
    if (TypeSpec(arrayDeclNode) != nullptr) {
        node->addChild(arrayDeclNode);
        if (match("*")) {
            arrayDeclNode = new TreeNode("*");
            node->addChild(arrayDeclNode);
        }
        if (match("id")) {
            arrayDeclNode = new TreeNode(currentToken.id);
            node->addChild(arrayDeclNode);

            arrayDeclNode = new TreeNode("ArrayD");
            if (ArrayD(arrayDeclNode) != nullptr) {
                node->addChild(arrayDeclNode);
                if (match(";")) {
                    arrayDeclNode = new TreeNode(";");
                    node->addChild(arrayDeclNode);
                }
                else {
                    arrayDeclNode = nullptr;
                }
            }
            else {
                arrayDeclNode = nullptr;
            }
        }
        else {
            arrayDeclNode = nullptr;
        }
    }
    else {
        arrayDeclNode = nullptr;
    }

    return arrayDeclNode;
}

TreeNode* ArrayD(TreeNode* node) {
    TreeNode* arraydNode = nullptr;

    if (match("[")) {
        arraydNode = new TreeNode("[");
        node->addChild(arraydNode);
        arraydNode = new TreeNode("variable");
        if (Variable(arraydNode)) {
            node->addChild(arraydNode);
        }   arraydNode = new TreeNode("]");
        if (match("]")) {
            node->addChild(arraydNode);
            arraydNode = new TreeNode("ArrayD");
            if (ArrayD(arraydNode) != nullptr) {
                node->addChild(arraydNode);
            }
        }

    }
    else {
        arraydNode = nullptr;
    }
    return arraydNode;
}


TreeNode* pointerlist(TreeNode* node) {
    TreeNode* pointerlistNode = nullptr;
    if (match("*")) {
        pointerlistNode = new TreeNode("*");
        node->addChild(pointerlistNode);
        pointerlistNode = new TreeNode("variablep");
        if (VariableP(pointerlistNode) != nullptr) {

            node->addChild(pointerlistNode);
            if (match(",")) {
                pointerlistNode = new TreeNode(",");
                node->addChild(pointerlistNode);
                pointerlistNode = new TreeNode("pointerlist");
                if (pointerlist(pointerlistNode) != nullptr) {
                    node->addChild(pointerlistNode);
                }
                else {
                    return nullptr;
                }
            }
        }
    }
    return pointerlistNode;
}

TreeNode* VariableP(TreeNode* node) {
    TreeNode* variablePNode = nullptr;

    if (match("id")) {
        variablePNode = new TreeNode(currentToken.id);
        node->addChild(variablePNode);

        if (match("=")) {
            variablePNode = new TreeNode("=");
            node->addChild(variablePNode);

            if (match("&")) {
                variablePNode = new TreeNode("&");
                node->addChild(variablePNode);

                if (match("id")) {
                    variablePNode = new TreeNode(currentToken.id);
                    node->addChild(variablePNode);
                }
                else {
                    return nullptr;
                }

            }
            else {
                return nullptr;
            }

        }
    }


    return variablePNode;
}


TreeNode* pointerdecl(TreeNode* node, int x) {
    currentPos = x;
    TreeNode* pointerdeclNode = nullptr;
    pointerdeclNode = new TreeNode("TypeSpec");
    if (TypeSpec(pointerdeclNode) != nullptr) {
        node->addChild(pointerdeclNode);
        pointerdeclNode = new TreeNode("pointerlist");
        if (pointerlist(pointerdeclNode) != nullptr) {
            node->addChild(pointerdeclNode);
            if (match(";")) {
                pointerdeclNode = new TreeNode(";");
                node->addChild(pointerdeclNode);
            }
            else {
                pointerdeclNode = nullptr;
            }
        }
        else {
            pointerdeclNode = nullptr;
        }
    }
    else {
        pointerdeclNode = nullptr;
    }
    return pointerdeclNode;
}

TreeNode* Decl(TreeNode* node, int x) {
    currentPos = x;
    TreeNode* declNode = nullptr;
    declNode = new TreeNode("TypeSpec");
    if (TypeSpec(declNode) != nullptr) {
        node->addChild(declNode);
        declNode = new TreeNode("Variablel");
        if (Variablel(declNode) != nullptr) {
            node->addChild(declNode);
            if (match(";")) {
                declNode = new TreeNode(";");
                node->addChild(declNode);
            }
            else {
                declNode = nullptr;
            }
        }
        else {
            declNode = nullptr;
        }
    }
    else {
        declNode = nullptr;
    }
    return declNode;
}

TreeNode* Variablel(TreeNode* node) {
    TreeNode* variablelNode = nullptr;
    if (match("id")) {
        variablelNode = new TreeNode(currentToken.id);
        node->addChild(variablelNode);
        variablelNode = new TreeNode("Eq");
        if (Eq(variablelNode) != nullptr) {
            node->addChild(variablelNode);
        }
            if (match(",")) {
                variablelNode = new TreeNode(",");
                node->addChild(variablelNode);
                variablelNode = new TreeNode("Variablel");
                if (Variablel(variablelNode) != nullptr) {
                    node->addChild(variablelNode);
                }
                
            }
        
    }
    return variablelNode;
}



void findLeaves(TreeNode* node, vector<string>& leaves) {
    if (node->next.empty()) {
        // Node is a leaf, add its data to the list
        leaves.push_back(node->data);
    }
    else {
        // Node has children, recursively traverse them
        for (TreeNode* child : node->next) {
            findLeaves(child, leaves);
        }
    }
}

TreeNode* IterationStmt(TreeNode* node, int x) {
    TreeNode* iterationStmtNode = nullptr;
    currentPos = x;
    if (match("for")) {
        iterationStmtNode = new TreeNode("for");
        node->addChild(iterationStmtNode);

        if (match("(")) {
            iterationStmtNode = new TreeNode("(");
            node->addChild(iterationStmtNode);


            iterationStmtNode = new TreeNode("Init");
            if (Init(iterationStmtNode) != nullptr) {
                node->addChild(iterationStmtNode);
                currentPos--;
            }

            if (match(";")) {
                iterationStmtNode = new TreeNode(";");
                node->addChild(iterationStmtNode);

                iterationStmtNode = new TreeNode("boolexpr");
                int woh = currentPos;
                if (BoolExpr(iterationStmtNode,woh) != nullptr) {
                    node->addChild(iterationStmtNode);
                }
                cout << Tokens[currentPos].id << endl;
                if (match(";")) {
                    iterationStmtNode = new TreeNode(";");
                    node->addChild(iterationStmtNode);
                    while (currentPos + 1 <= Tokens.size() && Tokens[currentPos].id != ")") {
                       
                        iterationStmtNode = new TreeNode("iterat");
                        if (iterat(iterationStmtNode) != nullptr) {
                            node->addChild(iterationStmtNode);
                        }
                        else {
                            currentPos++;
                        }
                    }
                    if (match(")")) {
                        iterationStmtNode = new TreeNode(")");
                        node->addChild(iterationStmtNode);

                        if (match("{")) {
                            iterationStmtNode = new TreeNode("{");
                            node->addChild(iterationStmtNode);
                            while (currentPos + 1 <= Tokens.size() && Tokens[currentPos].id != "}") {
                                iterationStmtNode =  new TreeNode("Stmt");
                                if (Stmt(iterationStmtNode) != nullptr) {
                                    node->addChild(iterationStmtNode);
                                }
                                else {
                                    currentPos++;
                                }
                            }
                            if (match("}")) {
                                iterationStmtNode = new TreeNode("}");
                                node->addChild(iterationStmtNode);
                            }
                            else {
                                iterationStmtNode = nullptr;
                            }
                        }
                        else {
                            iterationStmtNode = nullptr;
                        }
                    }
                    else {
                        iterationStmtNode = nullptr;
                    }
                }
                else {
                    iterationStmtNode = nullptr;
                }
            }
            else {
                iterationStmtNode = nullptr;
            }
        }
        else {
            iterationStmtNode = nullptr;
        }
    }

    return iterationStmtNode;
}

TreeNode* Init(TreeNode* node) {
    TreeNode* initNode = nullptr;
    TreeNode* AssignExpr1 = new TreeNode("arguexpr");
    TreeNode* Decl1 = new TreeNode("Decl1");
    int x = currentPos;
    if (AssignExpr(initNode) != nullptr) {
        node->addChild(AssignExpr1);
        initNode = AssignExpr1;
    }
    else if (Decl(Decl1, x) != nullptr) {
        node->addChild(Decl1);
        initNode = Decl1;
    }

    return initNode;
}

TreeNode* iterat(TreeNode* node) {
    TreeNode* iteratNode = nullptr;
    iteratNode = new TreeNode("Variable");

    if (match("id")) {
        iteratNode = new TreeNode(currentToken.id);
        node->addChild(iteratNode);
        iteratNode = new TreeNode("AssignOp");
        if (assignop(iteratNode)) {
            node->addChild(iteratNode);
            iteratNode = new TreeNode("Variable");
            if (Variable(iteratNode)) {
                node->addChild(iteratNode);
            }
            else {
                iteratNode = nullptr;
            }


        }
        else {
            iteratNode = nullptr;
        }
    }
    else if (Variable(iteratNode)) {
        node->addChild(iteratNode);
        iteratNode = new TreeNode("AssignOp");
        if (assignop(iteratNode)) {
            node->addChild(iteratNode);
            iteratNode = new TreeNode("Arithexpr");
            if (ArithExpr(iteratNode)) {
                node->addChild(iteratNode);
            }
            if (match(";")) {
                iteratNode = new TreeNode(";");
                node->addChild(iteratNode);
            }
            else {
                iteratNode = nullptr;
            }

        }
        else {
            iteratNode = nullptr;
        }

    }
    else {
        iteratNode = nullptr;
    }

    return iteratNode;
}
TreeNode* assignop(TreeNode* node) {
    TreeNode* assignopNode = nullptr;

    if (match("=") || match("*=") || match("/=") || match("%=") || match("+=") ||
        match("-=") || match("<<=") || match(">>=") || match("&=") || match("^=") || match("|=")) {
        assignopNode = new TreeNode(currentToken.id);
        node->addChild(assignopNode);
    }
    

    return assignopNode;
}

void findNotFoundTokens(const vector<T>& Tokens, const vector<string>& leaves) {
    bool inSuccession = false;  
    for (int i = 0; i < Tokens.size(); ++i) {
        bool tokenFound = false;

        int x = 0;
        for (int j = i + x; j < leaves.size(); ++j) {
            if (Tokens[i].id == leaves[j]) {
                tokenFound = true;
                break;
            }
        }
        if (!tokenFound) {
            if (!inSuccession) {

                cout << "Token(s) not found in leaf nodes:" << endl;
                cout << Tokens[i].id << endl;
                inSuccession = true;  
            }
        }
        else {
            x = -1;
            inSuccession = false;  
        }
    }
}

vector<string> findLeafNodes(TreeNode* root) {
    vector<string> leaves;
    if (root)
        findLeaves(root, leaves);
    findNotFoundTokens(Tokens, leaves);
    return leaves;
}

TreeNode* AssignExpr(TreeNode* node) {
    TreeNode* assignExprNode = nullptr;

    if (match("id")) {
        assignExprNode = new TreeNode(currentToken.id);
        node->addChild(assignExprNode);

        if (assignop(assignExprNode)!=nullptr) {
            assignExprNode = new TreeNode(currentToken.id);
            node->addChild(assignExprNode);

            assignExprNode = new TreeNode("variable ");
            if (Variable(assignExprNode)!=nullptr) {
                node->addChild(assignExprNode);
            }
            else {
                assignExprNode = new TreeNode("arithexpr ");
                if (ArithExpr(assignExprNode) != nullptr) {
                node->addChild(assignExprNode);
            }
            }

            
        }
        else {
            assignExprNode = nullptr;
        }
    }
    else {
        assignExprNode = nullptr;
    }

    return assignExprNode;
}












int main()
{
    string file1 = "C:\\Users\\dell\\Desktop\\Compiler\\project\\comp\\Testfile1.txt";
    string fileContents = readFileToString(file1);
    lexicalAnalyze(fileContents);
    level(Tokens);
    processTokens(Tokens);
    SymbolTable(Tokens);
    Pareser();
   // findLeafNodes(root);
    /*for (const auto& token : Symb)
        cout << "(" << token.id << "," << token.type << "," << token.level << ")" << endl;*/
    return 0;
}






