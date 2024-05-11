#ifndef comp
#define comp

#include <string>
#include <vector>
#include <string>
#include <vector>
#include <stdexcept>

struct T
{
    std::string id;
    std::string type;
    int level;
};

class TreeNode {
public:
    std::string data;
    std::vector<TreeNode*> next;

    TreeNode(std::string value);
    void addChild(TreeNode* child);
};

class LexicalAnalyzer { 
public:
    std::vector<T> Tokens;
    std::vector<T> Symb;
    std::string com;
    int next_token_index = 0;
    T t;
    bool sign = true;
    bool lon = true;
    bool sho = true;
    int lonc = 0;
    int shoc = 0;
    int woh1;
    T getToken();
    T findTokenById(const std::string& idToFind, int levelToFind);
    void SymbolTable(std::vector<T>& tokens);
    void set_next_token();
    bool isOperator(const std::string& code);
    bool aOperator(const std::string& code);
    bool detectMemberAccess(const std::string& input);
    bool KeywordRegex(const std::string& pattern);
    bool isID(const std::string& str);
    bool isComment(const std::string& str);
    bool special(const std::string& str);
    bool isDigit(const std::string& str);
    bool isString(const std::string& str);
    bool ischar(const std::string& str);
    bool isExponential(const std::string& str);
    bool isLiteral(const std::string& str);
    bool isSeparator(const std::string& str);
    bool isNotLegal(const std::string& str);
    void printRoleOfToken(const std::string& token);
    void lexicalAnalyze(const std::string& nameOfFile);
    void processTokens(std::vector<T>& tokens);
    void level(std::vector<T>& tokens);
    std::string readFileToString(const std::string& filename);
    bool match(const std::string& expectedId);
    void printTree(TreeNode* root, int depth = 0, bool lastChild = false);
    void visualize_parse_tree_aux(TreeNode* node, std::ostream& out);
    void visualize_parse_tree(TreeNode* root);
    void Pareser();
    void Start(TreeNode* node);

    TreeNode* Fundefs(TreeNode* node);
    TreeNode* Fundef(TreeNode* node);
    TreeNode* Header(TreeNode* node);
    TreeNode* ParamList(TreeNode* node);
    TreeNode* ParamDecl(TreeNode* node);
    TreeNode* ParamListOpt(TreeNode* node);
    TreeNode* Stmt(TreeNode* node);
    TreeNode* Substmt(TreeNode* node);
    TreeNode* Sinstmt(TreeNode* node);
    TreeNode* ReturnStmt(TreeNode* node,int x);
    TreeNode* ReturnExpr(TreeNode* node);
    TreeNode* Funcall(TreeNode* node,int x);
    TreeNode* Argu(TreeNode* node);
    TreeNode* ArgueExpr(TreeNode* node);


    TreeNode* Declaration(TreeNode* node);
    TreeNode* EnumSpec(TreeNode* node);
    TreeNode* EnumSpecSuffix(TreeNode* node);
    TreeNode* EnumList(TreeNode* node);
    TreeNode* EnumListOpt(TreeNode* node);
    TreeNode* Enum(TreeNode* node);
    TreeNode* EnumSuffix(TreeNode* node);
    TreeNode* StructSpec(TreeNode* node);
    TreeNode* StructSuffix(TreeNode* node);
    TreeNode* TypeSpec(TreeNode* node);
    TreeNode* SList(TreeNode* node);
    TreeNode* Eq(TreeNode* node);
    TreeNode* EqAssign(TreeNode* node);
    TreeNode* ArithExpr(TreeNode* node);
    TreeNode* Variable(TreeNode* node);
    TreeNode* SubArithExpr(TreeNode* node);
    TreeNode* UnaryOp(TreeNode* node);
    TreeNode* ArrayDecl(TreeNode* node,int x);
    TreeNode* ArrayD(TreeNode* node);
    TreeNode* pointerlist(TreeNode* node);
    TreeNode* VariableP(TreeNode* node);
    TreeNode* pointerdecl(TreeNode* node,int x);
    TreeNode* Decl(TreeNode* node,int x);
    TreeNode* Variablel(TreeNode* node);
};


#endif // comp
