#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include "comp.h"

using namespace std;


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
    T currentToken;
    T LexicalAnalyzer::getToken() {
        if (currentPos < Tokens.size()) {
            if(counter==0)
                cout << Tokens[currentPos].id << endl;
            return Tokens[currentPos++];
        }
        else {
            return { "END", "",0 };
        }
    }

    T LexicalAnalyzer::findTokenById(const string& idToFind, int levelToFind) {
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

    void LexicalAnalyzer::SymbolTable(vector<T>& tokens) {
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

    void LexicalAnalyzer::set_next_token() {
        next_token_index = woh1;
        T next_token = Tokens[next_token_index-1];
        t = next_token;
    }

    bool LexicalAnalyzer::isOperator(const string& code)
    {

        regex opRegex("(=|\\+=|-=|\\*=|/=|%=|\\^=|&=|\\|=|<<=|>>=|==|!=|>|<|>=|<=|&&|\\|\\||!|\\+\\+|--|~)");
        return regex_match(code, opRegex);
    }

    bool LexicalAnalyzer::aOperator(const string& code)
    {
        regex opRegex("(\\+|-|\\*|/|%|\\^|&|\\||<<|>>)");
        return regex_match(code, opRegex);
    }

    bool LexicalAnalyzer::detectMemberAccess(const string& input)
    {

        regex pattern(R"(\[|\]|\*|&|\.|->)");
        return regex_match(input, pattern);
    }

    bool LexicalAnalyzer:: KeywordRegex(const string& pattern)
    {
        static const regex keywords_regex(
            "(auto|break|case|char|const|continue|default|do|double|else|enum|"
            "extern|float|for|goto|if|inline|int|long|register|restrict|return|"
            "short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|"
            "volatile|while)");
        return regex_match(pattern, keywords_regex);
    }

    bool LexicalAnalyzer::isID(const string& str)
    {
        regex idRegex("^[a-zA-Z_][a-zA-Z0-9_]*");
        return regex_match(str, idRegex);
    }

    bool LexicalAnalyzer::isComment(const string& str)
    {
        return str == "/* ... */" || str == "//";
    }
    bool LexicalAnalyzer::special(const string& str)
    {
        regex pattern("\\\\[nrtbvaf]");
        return regex_match(str, pattern);
    }

    bool LexicalAnalyzer::isDigit(const string& str)
    {
        // float x = .5;
        // float x = 1.5;
        regex fRegex("(\\+|-)?(\\d+)?(\\.\\d+)");
        // float x = 1.;
        // float x = 1.5;
        regex decimalRegex("(\\+|-)?\\d+(\\.\\d*)?");
        regex octalRegex("(\\+|-)?0[0-7]+");
        regex hexadecimalRegex("(\\+|-)?0[xX][0-9a-fA-F]+");
        regex binaryRegex("(\\+|-)?0[bB][01]+");

        return regex_match(str, decimalRegex) || regex_match(str, octalRegex) ||
            regex_match(str, hexadecimalRegex) || regex_match(str, binaryRegex) || regex_match(str, fRegex);
    }

    bool LexicalAnalyzer::isString(const string& str)
    {
        regex pattern("'((.)|\\\\[nrtbvaf])'");
        return (str[0] == '"' && str[str.size() - 1] == '"') || (regex_match(str, pattern));
    }

    bool LexicalAnalyzer::ischar(const string& str)
    {
        regex pattern("'((.)|\\\\[nrtbvaf])'");
        return  (regex_match(str, pattern));
    }

    bool LexicalAnalyzer::isExponential(const string& str)
    {
        regex exponentialRegex("(\\+|-)?(\\d*(\\.\\d+))([eE][-|\\+]?\\d+)?");
        regex exponentialRegex2("(\\+|-)?(\\d+(\\.\\d*)?)([eE][-|\\+]?\\d+)?");
        return regex_match(str, exponentialRegex) || regex_match(str, exponentialRegex2);
    }

    bool LexicalAnalyzer::isLiteral(const string& str)
    {
        return isExponential(str) || isDigit(str) || isString(str);
    }

    bool LexicalAnalyzer::isSeparator(const string& str)
    {
        const vector<string> Separators{ "{", "}", ",", "(", ")", ";", "?", ":" };
        for (const auto& separate : Separators)
            if (separate == str)
                return true;

        return false;
    }

    bool LexicalAnalyzer::isNotLegal(const string& str)
    {
        return str == " " || str == "\n" || str == "\t";
    }

    void LexicalAnalyzer::printRoleOfToken(const string& token)
    {
        T woh;
        if (isComment(token))
        {
            if (com[com.size() - 1] == '\n')
                com.erase(com.size() - 1);
            woh = { com, "com" };
        }
        else if (isOperator(token))
        {
            woh = { token, "op" };
        }
        else if (aOperator(token))
        {
            woh = { token, "aop" };
        }
        else if (detectMemberAccess(token))
        {
            woh = { token, "ma" };
        }
        else if (isSeparator(token))
        {
            woh = { token, "sp" };
        }
        else if (KeywordRegex(token))
        {
            woh = { token, "kw" };
        }
        else if (isLiteral(token))
        {
            woh = { token, "li" };
        }
        else if (isID(token))
        {
            woh = { token, "id" };
        }
        else
        {
            if (token[0] == '"')
                woh = { token + '"', "inv" };
            else
                woh = { token, "inv" };
        }
        if (token != "")
        {
            Tokens.push_back(woh);
        }
    }

    void LexicalAnalyzer::lexicalAnalyze(const string& nameOfFile)
    {
        char ch;
        string buffer;
        istringstream file(nameOfFile);

        bool multiCm = false, singleCm = false;

        while (file >> noskipws >> ch)
        {
            if (multiCm)
            {
                com += ch;
                if (ch == '*')
                {
                    char next_ch;
                    file >> next_ch;
                    if (next_ch == '/')
                    {
                        multiCm = false;
                        com += next_ch;
                        printRoleOfToken("/* ... */");
                        com = "";
                    }
                    else
                    {
                        file.unget();
                    }
                }

                continue;
            }
            if (singleCm)
            {

                com += ch;
                if (ch == '\n')
                {
                    singleCm = false;
                    printRoleOfToken("//");
                    com = "";
                }
                continue;
            }

            if (ch == '/' && buffer[0] != '"')
            {
                char next_ch;
                com += ch;
                file >> next_ch;

                if (next_ch == '*')
                {
                    com += next_ch;
                    multiCm = true;
                    continue;
                }
                else if (next_ch == '/')
                {

                    singleCm = true;
                    com += next_ch;
                    continue;
                }
                else
                {
                    com = "";
                    file.unget();
                }
            }

            if (ch == '\\' && (buffer[0] == '"' || buffer[0] == '\''))
            {
                string buf;
                char next_ch;
                buf += ch;
                file >> next_ch;
                if (special(buf + next_ch))
                {
                    buf += next_ch;
                    T woh = { buf, "sc" };
                    Tokens.push_back(woh);
                    buffer += buf;
                    continue;
                }
                else
                {
                    file.unget();
                }
            }
            if ((ch == '-') && buffer[0] != '"' && buffer[0] != '\'')
            {
                string buf;
                char next_ch;
                buf += ch;
                file >> next_ch;
                if (detectMemberAccess(buf + next_ch))
                {
                    buf += next_ch;
                    T woh = { buf, "ma" };
                    printRoleOfToken(buffer);
                    buffer = "";
                    Tokens.push_back(woh);
                    continue;
                }
                else
                {
                    file.unget();
                }
            }
            if (ch == '.' && buffer[0] != '"' && buffer[0] != '\'' && (isDigit(buffer) || isExponential(buffer) || buffer == "") && (Tokens[Tokens.size() - 1].type != "id") && (!isID(buffer)))
            {
                buffer += ch;
                continue;
            }
            if (tolower(ch) == 'e' && buffer[0] != '"' && buffer[0] != '\'' && !isID(buffer))
            {
                char next_ch;
                file >> next_ch;
                if ((next_ch == '+') || (next_ch == '-') || (isDigit(string(1, next_ch))))
                {
                    buffer += ch;
                    buffer += next_ch;
                    continue;
                }
                else
                {
                    file.unget();
                }
            }

            if ((ch == '*' || ch == '&') && (Tokens[Tokens.size() - 1].type != "id" && (!isID(buffer) || KeywordRegex(buffer)) && !isLiteral(buffer) && Tokens[Tokens.size() - 1].type != "li") && buffer != "&" && !aOperator(buffer) && buffer[0] != '"' && buffer[0] != '\'')
            {
                string buf;
                char next_ch;
                buf += ch;
                file >> next_ch;
                if (isOperator(buf + next_ch))
                {
                    buf += next_ch;
                    T woh = { buf, "op" };
                    printRoleOfToken(buffer);
                    buffer = "";
                    Tokens.push_back(woh);
                    continue;
                }
                else
                {
                    file.unget();
                    printRoleOfToken(buffer);
                    buffer = "";
                    T woh = { string(1, ch), "ma" };
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
                if (isOperator(buffer + next_ch))
                {
                    buffer += next_ch;
                    printRoleOfToken(buffer);
                    buffer = "";
                    continue;
                }
                else
                {
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
            if (buffer[0] == '\'' && buffer.size() != 1 && ch == '\'')
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
            if (buffer[0] == '"' && ch == '\n')
            {

                while (file >> noskipws >> ch)
                {
                    if (ch == '"')
                        break;
                    buffer += ch;
                }
                printRoleOfToken(buffer);
                buffer = "";
            }
            if (ch == '"' && buffer[0] == '"' && buffer.size() != 1 && buffer[buffer.size() - 2] != '\\')
            {
                printRoleOfToken(buffer);
                buffer = "";
            }
        }
        if (!buffer.empty())
        {
            printRoleOfToken(buffer);
        }
        if (!com.empty())
        {
            printRoleOfToken(com);
        }
    }

    string LexicalAnalyzer::readFileToString(const string& filename)
    {
        ifstream file(filename);
        stringstream buffr;
        string line;

        if (file.is_open())
        {
            while (getline(file, line))
            {
                buffr << line << endl;
            }
            file.close();
            return buffr.str();
        }
        else
        {
            cerr << "Error: Unable to open file " << filename << endl;
            return "";
        }
    }

    void LexicalAnalyzer::level(vector<T>& tokens) {
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


    void LexicalAnalyzer::processTokens(vector<T>& tokens) {

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

    TreeNode::TreeNode(std::string value) : data(value) {}

    void TreeNode::addChild(TreeNode* child) {
        next.push_back(child);
    }

    bool LexicalAnalyzer::match(const string& expectedId) {
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
        if (counter >= 10) {
            return true;
        }
        return false;
    }

    void LexicalAnalyzer::printTree(TreeNode* root, int depth , bool lastChild) {
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

    void LexicalAnalyzer::visualize_parse_tree_aux(TreeNode* node, ostream& out) {
        // Need to uniquely identify each node in graphviz
        out << "  \"" << node << "\" [label=\"" << node->data << "\""
            << (node->next.empty() ? "][fontcolor=\"red\"][color=\"red\"];\n" : "];\n");

        for (auto child : node->next) {
            // Need to uniquely identify each child node too
            out << "  \"" << child << "\" [label=\"" << child->data << "\"];\n";

            // Create edge between node and child unique addresses
            out << "  \"" << node << "\" -> \"" << child << "\";\n";

            // Recursively build the rest of the tree
            visualize_parse_tree_aux(child, out);
        }
    }

    void LexicalAnalyzer::visualize_parse_tree(TreeNode* root) {
        ofstream dot_file_out("parse_tree.dot", ios::trunc);

        // TODO: Decorate your parse tree using graphviz dot notation
        dot_file_out << "digraph G {\n";
        visualize_parse_tree_aux(root, dot_file_out);
        dot_file_out << "}";
        dot_file_out.close();

        // Convert the dot file into an SVG vector image
        system("dot -Tsvg ./parse_tree.dot -oparse_tree.svg");

        // Open the vector file
        // TODO: Integrate this with your GUI
        // You'll need to find a way to open an .svg file inside your GUI framework
        system("parse_tree.svg");
    }

    TreeNode* root = new TreeNode("Start");
    void LexicalAnalyzer::Pareser() {
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
        visualize_parse_tree(root);
    }

    void  LexicalAnalyzer::Start(TreeNode* node) {
        TreeNode* decl = new TreeNode("Declaration");
        if( Declaration(decl)!=nullptr)
            node->addChild(decl);

        /*TreeNode* fun = new TreeNode("Fundefs");
    if (Fundefs(fun) != nullptr) {
        node->addChild(fun);

    }*/

    }

    TreeNode* LexicalAnalyzer::Fundefs(TreeNode* node) {
        TreeNode* fundefsNode =nullptr ;
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


    TreeNode* LexicalAnalyzer::Fundef(TreeNode* node) {
        TreeNode* fundefNode = nullptr;
        fundefNode = new TreeNode("header");

        if (Header(fundefNode)) {
            node->addChild(fundefNode);
            if (match("{")) {
                fundefNode = new TreeNode("{");
                node->addChild(fundefNode);
                while (currentPos + 1 <= Tokens.size() && Tokens[currentPos ].id != "}") {
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


    TreeNode* LexicalAnalyzer::Header(TreeNode* node) {
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
            headerNode=new TreeNode(currentToken.id);
            node->addChild(headerNode);
            if (match("(")) {
                headerNode = new TreeNode("(");
                node->addChild(headerNode);
                while (currentPos +1 <= Tokens.size() && Tokens[currentPos ].id != ")") {
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

    TreeNode* LexicalAnalyzer::ParamList(TreeNode* node) {

        TreeNode* paramListNode = nullptr;

        paramListNode=new TreeNode("ParamDecl");

        if (ParamDecl(paramListNode) != nullptr) {
            node->addChild(paramListNode);
        }
        paramListNode = new TreeNode("ParamListOpt");
        if (ParamListOpt(paramListNode) != nullptr) {
            node->addChild(paramListNode);
        }
        else {
            paramListNode = nullptr;
        }

        return paramListNode;
    }

    TreeNode* LexicalAnalyzer::ParamDecl(TreeNode* node) {
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

    TreeNode* LexicalAnalyzer::ParamListOpt(TreeNode* node) {
        TreeNode* paramListOptNode= nullptr;

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

    TreeNode* LexicalAnalyzer::Stmt(TreeNode* node) {
        TreeNode* stmtNode = nullptr;
        stmtNode = new TreeNode("Sinstmt");
        if (Sinstmt(stmtNode)!=nullptr) {
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

    TreeNode* LexicalAnalyzer::Substmt(TreeNode* node) {
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

    TreeNode* LexicalAnalyzer::Sinstmt(TreeNode* node) {
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

        /*if (Expr(expr1,x) != nullptr) {
        node->addChild(expr1);
        sinstmtNode = expr1;
    }
    else if (CondExpr(condexpr1,x) != nullptr) {
        node->addChild(condexpr1);
        sinstmtNode = condexpr1;
    }
    else if (IterationStmt(IterationStmt1,x) != nullptr) {
        node->addChild(IterationStmt1);
        sinstmtNode = IterationStmt1;
    }
    else*/ if (Funcall(funcall1,x) != nullptr) {
            node->addChild(funcall1);
            sinstmtNode = funcall1;
        }
        else if (ReturnStmt(returnstmt1,x) != nullptr) {
            node->addChild(returnstmt1);
            sinstmtNode = returnstmt1;
        }
        else if (ArrayDecl(ArrayDecl1,x) != nullptr) {
            node->addChild(ArrayDecl1);
            sinstmtNode = ArrayDecl1;
        }
        else if (pointerdecl(pointerdecl1,x) != nullptr) {
            node->addChild(pointerdecl1);
            sinstmtNode = pointerdecl1;
        }
        else if (Decl(Decl1,x) != nullptr) {
            node->addChild(Decl1);
            sinstmtNode = Decl1;
        }

        return sinstmtNode;
    }

    TreeNode* LexicalAnalyzer::ReturnStmt(TreeNode* node,int x) {
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

    TreeNode* LexicalAnalyzer::ReturnExpr(TreeNode* node) {
        TreeNode* returnExprNode = nullptr;
        TreeNode* BoolExpr1 = new TreeNode("boolexpr");
        TreeNode* ArithExpr1 = new TreeNode("arithexpr");
        TreeNode* Variable1 = new TreeNode("variable");
        if (match("1") || match("0")) {
            returnExprNode = new TreeNode(currentToken.id);
            node->addChild(returnExprNode);
        }
        /*else if (BoolExpr(BoolExpr1) != nullptr) {
        node->addChild(BoolExpr1);
        returnExprNode = BoolExpr1;

    }*/
        else if (ArithExpr(ArithExpr1) != nullptr) {
            node->addChild(ArithExpr1);
            returnExprNode = ArithExpr1;
        }
        else if (Variable(Variable1) != nullptr) {
            node->addChild(Variable1);
            returnExprNode = Variable1;
        }


        return returnExprNode;
    }

    TreeNode* LexicalAnalyzer::Funcall(TreeNode* node,int x) {
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
            }else {
                funcallNode = nullptr;
            }
        }


        return funcallNode;
    }

    TreeNode* LexicalAnalyzer::Argu(TreeNode* node) {
        TreeNode* arguNode = nullptr;

        arguNode =  new TreeNode("arguexpr");
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

    TreeNode* LexicalAnalyzer::ArgueExpr(TreeNode* node) {
        TreeNode* argueExprNode = nullptr;
        TreeNode* BoolExpr1 = new TreeNode("boolexpr");
        TreeNode* ArithExpr1 = new TreeNode("arithexpr");
        TreeNode* Variable1 = new TreeNode("variable");

        if (ArithExpr(node) != nullptr) {
            node->addChild(ArithExpr1);
            argueExprNode = ArithExpr1;
        }
        /* else if (BoolExpr(node) != nullptr) {
        node->addChild(ArithExpr1);
        argueExprNode = ArithExpr1;
    }*/
        else if (Variable(node) != nullptr) {
            node->addChild(ArithExpr1);
            argueExprNode = ArithExpr1;
        }


        return argueExprNode;
    }




    TreeNode* LexicalAnalyzer::Declaration(TreeNode* node) {
        TreeNode* declarationNode = nullptr;
        TreeNode* EnumSpec1 = new TreeNode("EnumSpec");
        TreeNode* StructSpec1 = new TreeNode("StructSpec");
        if (EnumSpec(EnumSpec1)!=nullptr) {
            node->addChild(EnumSpec1);
            declarationNode = EnumSpec1;
        }
        else if (StructSpec(StructSpec1)!=nullptr) {

            node->addChild(StructSpec1);
            declarationNode = StructSpec1;
        }
        /*else if (ArrayDecl(node)!=nullptr) {

        declarationNode = new TreeNode("arraydecl");
        node->addChild(declarationNode);
    }
    else if (PointerDecl(node)!=nullptr) {

        declarationNode = new TreeNode("pointerdecl");
        node->addChild(declarationNode);
    }
    else if (Decl(node)!=nullptr) {

        declarationNode = new TreeNode("Decl");
        node->addChild(declarationNode);
    }*/

        return declarationNode;
    }

    TreeNode* LexicalAnalyzer::EnumSpec(TreeNode* node) {
        TreeNode* enumSpecNode = nullptr;


        if (match( "enum")) {
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

    TreeNode* LexicalAnalyzer::EnumSpecSuffix(TreeNode* node) {
        TreeNode* enumSpecSuffixNode = nullptr;

        if (match( "{")) {
            enumSpecSuffixNode = new TreeNode("{");
            node->addChild(enumSpecSuffixNode);

            enumSpecSuffixNode = new TreeNode("EnumList");
            if (EnumList(enumSpecSuffixNode) != nullptr) {

                node->addChild(enumSpecSuffixNode);
            }
            if (match ( "}")) {
                enumSpecSuffixNode = new TreeNode("}");
                node->addChild(enumSpecSuffixNode);
            }

        }



        return enumSpecSuffixNode;
    }

    TreeNode* LexicalAnalyzer::EnumList(TreeNode* node) {
        TreeNode* enumListNode =  new TreeNode("Enum");
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

    TreeNode* LexicalAnalyzer::EnumListOpt(TreeNode* node) {
        TreeNode* commaNode = new TreeNode(",");
        if (match(",")) {
            node->addChild(commaNode);

            TreeNode* commaNode = new TreeNode("Enum");
            if (Enum(commaNode) != nullptr) {
                node->addChild(commaNode);

                commaNode = new TreeNode("EnumListopt");
                if(EnumListOpt(commaNode)!=nullptr)
                    node->addChild(commaNode);

            }
        }


        return node;
    }

    TreeNode* LexicalAnalyzer::Enum(TreeNode* node) {
        TreeNode* enumNode = nullptr;

        if (match( "id")) {
            enumNode = new TreeNode(currentToken.id);
            node->addChild(enumNode);
            enumNode = new TreeNode("EnumSuffix");
            EnumSuffix(enumNode);
            node->addChild(enumNode);
        }


        return enumNode;
    }

    TreeNode* LexicalAnalyzer::EnumSuffix(TreeNode* node) {
        TreeNode* enumSuffixNode = nullptr;

        if (match( "=")) {
            TreeNode* assignNode = new TreeNode("=");
            node->addChild(assignNode);

            if (match("num")) {
                TreeNode* digitNode = new TreeNode(currentToken.id);
                node->addChild(digitNode);
            }

        }


        return enumSuffixNode;
    }

    TreeNode* LexicalAnalyzer::StructSpec(TreeNode* node) {
        TreeNode* structSpecNode = new TreeNode("Structe");

        if (match("struct")) {
            node->addChild(structSpecNode);

            if (match("id")) {
                TreeNode* structSpecNode = new TreeNode("id");
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

    TreeNode* LexicalAnalyzer::StructSuffix(TreeNode* node) {
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
            structSuffixNode=nullptr;
            return structSuffixNode;
        }

        return structSuffixNode;
    }

    TreeNode* LexicalAnalyzer::TypeSpec(TreeNode* node) {
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


    TreeNode* LexicalAnalyzer::SList(TreeNode* node) {
        TreeNode* sListNode = nullptr;

        sListNode = new TreeNode("TypeSpec");
        if (TypeSpec(sListNode)!=nullptr) {
            node->addChild(sListNode);
            sListNode = new TreeNode("Eq");

            Eq(sListNode) != nullptr;
            node->addChild(sListNode);


            if (match("id")) {
                TreeNode* identifierNode = new TreeNode("id");
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

    TreeNode* LexicalAnalyzer::Eq(TreeNode* node) {
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

    TreeNode* LexicalAnalyzer::EqAssign(TreeNode* node) {
        TreeNode* eqAssignNode = nullptr;

        if (match("num") || match("id") || match("string") || match("char")) {
            eqAssignNode = new TreeNode(currentToken.id);
            node->addChild(eqAssignNode);
        }
        else if (match("arithexpr")) {
            eqAssignNode == new TreeNode("arithexpr");
            ArithExpr(eqAssignNode);
            node->addChild(eqAssignNode);

        }


        return eqAssignNode;
    }

    TreeNode* LexicalAnalyzer::ArithExpr(TreeNode* node) {
        TreeNode* arithExprNode = nullptr;

        if (Variable(arithExprNode)!=nullptr) {
            arithExprNode = new TreeNode("Variable");
            node->addChild(arithExprNode);

            arithExprNode = new TreeNode("SubArithExpr");
            SubArithExpr(arithExprNode);
            node->addChild(arithExprNode);

        }
        else {

            return arithExprNode;
        }

        return arithExprNode;
    }

    TreeNode* LexicalAnalyzer::Variable(TreeNode* node) {
        TreeNode* variableNode =nullptr;

        if (match("num") || match("id")) {
            variableNode = new TreeNode(currentToken.id);
            node->addChild(variableNode);
        }


        return variableNode;
    }

    TreeNode* LexicalAnalyzer::SubArithExpr(TreeNode* node) {
        TreeNode* subArithExprNode = new TreeNode("Unaryop");

        if (UnaryOp(subArithExprNode)) {

            node->addChild(subArithExprNode);

            subArithExprNode = new TreeNode("ArithExpr");
            ArithExpr(subArithExprNode);
            node->addChild(subArithExprNode);

        }
        else {
            subArithExprNode = nullptr;
            return subArithExprNode;
        }

        return subArithExprNode;
    }

    TreeNode* LexicalAnalyzer::UnaryOp(TreeNode* node) {
        TreeNode* unaryOpNode = nullptr;

        if (match("&") || match("*") || match("+") || match("-") || match("~") || match("!") ||
            match("%") || match("/") || match("^") || match("<<") || match(">>") || match("|")) {
            unaryOpNode = new TreeNode(currentToken.id);
            node->addChild(unaryOpNode);
        }


        return unaryOpNode;
    }

    TreeNode* LexicalAnalyzer::ArrayDecl(TreeNode* node,int x) {
        TreeNode* arrayDeclNode = nullptr;
        currentPos = x;
        arrayDeclNode = new TreeNode("TypeSpec");
        if (TypeSpec(arrayDeclNode)!=nullptr) {
            node->addChild(arrayDeclNode);
            if (match("*")) {
                arrayDeclNode = new TreeNode("*");
                node->addChild(arrayDeclNode);
            }
            if (match("id")) {
                arrayDeclNode = new TreeNode(currentToken.id);
                node->addChild(arrayDeclNode);

                arrayDeclNode = new TreeNode("ArrayD");
                if (ArrayD(arrayDeclNode)!=nullptr) {
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

    TreeNode* LexicalAnalyzer::ArrayD(TreeNode* node) {
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
                if (ArrayD(arraydNode)!=nullptr) {
                    node->addChild(arraydNode);
                }
            }

        }
        else {
            arraydNode = nullptr;
        }
        return arraydNode;
    }


    TreeNode* LexicalAnalyzer::pointerlist(TreeNode* node) {
        TreeNode* pointerlistNode = nullptr;
        if (match("*")) {
            pointerlistNode = new TreeNode("*");
            node->addChild(pointerlistNode);
            pointerlistNode = new TreeNode("variablep");
            if (VariableP(pointerlistNode)!=nullptr) {

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

    TreeNode* LexicalAnalyzer::VariableP(TreeNode* node) {
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


    TreeNode* LexicalAnalyzer::pointerdecl(TreeNode* node,int x) {
        currentPos = x;
        TreeNode* pointerdeclNode = nullptr;
        pointerdeclNode = new TreeNode("TypeSpec");
        if (TypeSpec(pointerdeclNode)!=nullptr) {
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

    TreeNode* LexicalAnalyzer::Decl(TreeNode* node,int x) {
        currentPos = x;
        TreeNode* declNode = nullptr;
        declNode = new TreeNode("TypeSpec");
        if (TypeSpec(declNode) != nullptr) {
            node->addChild(declNode);
            declNode = new TreeNode("Variablel");
            if (Variablel(declNode)!=nullptr) {
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

    TreeNode* LexicalAnalyzer::Variablel(TreeNode* node) {
        TreeNode* variablelNode = nullptr;
        if (match("id")) {
            variablelNode = new TreeNode(currentToken.id);
            node->addChild(variablelNode);
            variablelNode = new TreeNode("Eq");
            if (Eq(variablelNode)!=nullptr) {
                node->addChild(variablelNode);
                if (match(",")) {
                    variablelNode = new TreeNode(",");
                    node->addChild(variablelNode);
                    variablelNode = new TreeNode("Variablel");
                    if (Variablel(variablelNode) != nullptr) {
                        node->addChild(variablelNode);
                    }
                    else {
                        variablelNode =nullptr;
                    }
                }
            }
        }
        return variablelNode;
    }
