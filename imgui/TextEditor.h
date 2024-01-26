#pragma once

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <regex>
#include "imgui.h"

class TextEditor
{
public:
enum class PaletteIndex
{
    Default,                // 默认颜色
    Keyword,                // 关键字颜色
    Number,                 // 数字颜色
    String,                 // 字符串颜色
    CharLiteral,            // 字符字面量颜色
    Punctuation,            // 标点符号颜色
    Preprocessor,           // 预处理器颜色
    Identifier,             // 标识符颜色
    KnownIdentifier,        // 已知标识符颜色
    PreprocIdentifier,      // 预处理器标识符颜色
    Comment,                // 注释颜色
    MultiLineComment,       // 多行注释颜色
    Background,             // 背景颜色
    Cursor,                 // 光标颜色
    Selection,              // 选择颜色
    ErrorMarker,            // 错误标记颜色
    Breakpoint,             // 断点颜色
    LineNumber,             // 行号颜色
    CurrentLineFill,        // 当前行填充颜色
    CurrentLineFillInactive,// 非活动状态下的当前行填充颜色
    CurrentLineEdge,        // 当前行边缘颜色
    Max                     // 枚举的最大值，用于数组大小等
};


enum class SelectionMode
{
    Normal,   // 普通选择模式，以字符为单位选择
    Word,     // 单词选择模式，以单词为单位选择
    Line      // 行选择模式，以行为单位选择
};


struct Breakpoint
{
    int mLine;             // 断点所在的行号
    bool mEnabled;         // 断点是否启用（true 表示启用，false 表示禁用）
    std::string mCondition; // 断点的条件（可以为空字符串）

    // 默认构造函数，用于初始化结构体成员
    Breakpoint()
        : mLine(-1)          // 初始化行号为 -1
        , mEnabled(false)   // 初始化为禁用状态
    {}
};

// 表示用户视角下的字符坐标
// 假设屏幕上呈现一个统一的网格（假设使用等宽字体），每个单元格都有自己的坐标，从0开始。
// 制表符按照[1..mTabSize]的空格数量计算，具体取决于达到下一个制表符停止点所需的空格数。
// 例如，当mTabSize = 4时，坐标(1, 5)表示在行"\tABC"中的字符'B'，因为在屏幕上呈现为"    ABC"。
	// 表示坐标的结构体
struct Coordinates
{
    int mLine, mColumn;

    // 默认构造函数
    Coordinates() : mLine(0), mColumn(0) {}

    // 带参数的构造函数
    Coordinates(int aLine, int aColumn) : mLine(aLine), mColumn(aColumn)
    {
        assert(aLine >= 0);
        assert(aColumn >= 0);
    }

    // 静态函数，返回无效坐标
    static Coordinates Invalid() { static Coordinates invalid(-1, -1); return invalid; }

    // 相等运算符重载
    bool operator ==(const Coordinates& o) const
    {
        return
            mLine == o.mLine &&
            mColumn == o.mColumn;
    }

    // 不等运算符重载
    bool operator !=(const Coordinates& o) const
    {
        return
            mLine != o.mLine ||
            mColumn != o.mColumn;
    }

    // 小于运算符重载
    bool operator <(const Coordinates& o) const
    {
        if (mLine != o.mLine)
            return mLine < o.mLine;
        return mColumn < o.mColumn;
    }

    // 大于运算符重载
    bool operator >(const Coordinates& o) const
    {
        if (mLine != o.mLine)
            return mLine > o.mLine;
        return mColumn > o.mColumn;
    }

    // 小于等于运算符重载
    bool operator <=(const Coordinates& o) const
    {
        if (mLine != o.mLine)
            return mLine < o.mLine;
        return mColumn <= o.mColumn;
    }

    // 大于等于运算符重载
    bool operator >=(const Coordinates& o) const
    {
        if (mLine != o.mLine)
            return mLine > o.mLine;
        return mColumn >= o.mColumn;
    }
};

// 表示标识符的结构体
struct Identifier
{
    // 标识符的位置坐标
    Coordinates mLocation;

    // 标识符的声明
    std::string mDeclaration;
};


// 别名，表示字符串类型
typedef std::string String;

// 别名，表示一个无序映射，将字符串映射到标识符类型
typedef std::unordered_map<std::string, Identifier> Identifiers;

// 别名，表示一个无序集合，用于存储关键字字符串
typedef std::unordered_set<std::string> Keywords;

// 别名，表示一个有序映射，将整数映射到字符串，用于存储错误标记
typedef std::map<int, std::string> ErrorMarkers;

// 别名，表示一个无序集合，用于存储断点的行号
typedef std::unordered_set<int> Breakpoints;

// 别名，表示一个包含固定大小的颜色数组，用于表示调色板
typedef std::array<ImU32, (unsigned)PaletteIndex::Max> Palette;

// 别名，表示一个无符号8位整数，用于表示字符
typedef uint8_t Char;


	// 字形结构体
struct Glyph
{
    Char mChar;
    PaletteIndex mColorIndex = PaletteIndex::Default;
    bool mComment : 1;
    bool mMultiLineComment : 1;
    bool mPreprocessor : 1;

    // 构造函数
    Glyph(Char aChar, PaletteIndex aColorIndex)
        : mChar(aChar), mColorIndex(aColorIndex), mComment(false), mMultiLineComment(false), mPreprocessor(false) {}
};

// 代表一行的容器
typedef std::vector<Glyph> Line;

// 代表多行的容器
typedef std::vector<Line> Lines;

// 语言定义结构体
struct LanguageDefinition
{
    // Token 正则表达式字符串的类型
    typedef std::pair<std::string, PaletteIndex> TokenRegexString;

    // Token 正则表达式字符串的容器
    typedef std::vector<TokenRegexString> TokenRegexStrings;

    // Tokenize 回调函数类型
    typedef bool (*TokenizeCallback)(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end, PaletteIndex& paletteIndex);

    // 语言名称
    std::string mName;

    // 关键字集合
    Keywords mKeywords;

    // 标识符集合
    Identifiers mIdentifiers;

    // 预处理标识符集合
    Identifiers mPreprocIdentifiers;

    // 注释开始、注释结束和单行注释标记
    std::string mCommentStart, mCommentEnd, mSingleLineComment;

    // 预处理符号
    char mPreprocChar;

    // 是否自动缩进
    bool mAutoIndentation;

    // Tokenize 回调函数
    TokenizeCallback mTokenize;

    // Token 正则表达式字符串集合
    TokenRegexStrings mTokenRegexStrings;

    // 是否区分大小写
    bool mCaseSensitive;

    // 构造函数
    LanguageDefinition()
        : mPreprocChar('#'), mAutoIndentation(true), mTokenize(nullptr), mCaseSensitive(true)
    {
    }

    // 静态函数，返回 C++ 语言定义
    static const LanguageDefinition& CPlusPlus();

    // 静态函数，返回 HLSL 语言定义
    static const LanguageDefinition& HLSL();

    // 静态函数，返回 GLSL 语言定义
    static const LanguageDefinition& GLSL();

    // 静态函数，返回 C 语言定义
    static const LanguageDefinition& C();

    // 静态函数，返回 SQL 语言定义
    static const LanguageDefinition& SQL();

    // 静态函数，返回 AngelScript 语言定义
    static const LanguageDefinition& AngelScript();

    // 静态函数，返回 Lua 语言定义
    static const LanguageDefinition& Lua();
};


// 构造函数
TextEditor();

// 析构函数
~TextEditor();

// 设置语言定义（LanguageDefinition）以进行语法高亮
void SetLanguageDefinition(const LanguageDefinition& aLanguageDef);

// 获取当前语言定义
const LanguageDefinition& GetLanguageDefinition() const { return mLanguageDefinition; }

// 获取调色板（Palette）
const Palette& GetPalette() const { return mPaletteBase; }

// 设置调色板
void SetPalette(const Palette& aValue);

// 设置错误标记
void SetErrorMarkers(const ErrorMarkers& aMarkers) { mErrorMarkers = aMarkers; }

// 设置断点
void SetBreakpoints(const Breakpoints& aMarkers) { mBreakpoints = aMarkers; }

// 渲染文本编辑器
void Render(const char* aTitle, const ImVec2& aSize = ImVec2(), bool aBorder = false);

// 设置文本内容
void SetText(const std::string& aText);

// 获取当前文本内容
std::string GetText() const;

// 设置文本行
void SetTextLines(const std::vector<std::string>& aLines);

// 获取文本行
std::vector<std::string> GetTextLines() const;

// 获取选中文本
std::string GetSelectedText() const;

// 获取当前行文本
std::string GetCurrentLineText() const;

// 获取总行数
int GetTotalLines() const { return (int)mLines.size(); }

// 获取覆盖模式状态
bool IsOverwrite() const { return mOverwrite; }

// 设置只读状态
void SetReadOnly(bool aValue);

// 获取只读状态
bool IsReadOnly() const { return mReadOnly; }

// 判断文本是否被更改
bool IsTextChanged() const { return mTextChanged; }

// 判断光标位置是否发生改变
bool IsCursorPositionChanged() const { return mCursorPositionChanged; }

// 判断语法高亮是否启用
bool IsColorizerEnabled() const { return mColorizerEnabled; }

// 启用/禁用语法高亮
void SetColorizerEnable(bool aValue);

// 获取光标位置
Coordinates GetCursorPosition() const { return GetActualCursorCoordinates(); }

// 设置光标位置
void SetCursorPosition(const Coordinates& aPosition);

// 启用/禁用鼠标输入处理
inline void SetHandleMouseInputs(bool aValue) { mHandleMouseInputs = aValue; }
inline bool IsHandleMouseInputsEnabled() const { return mHandleMouseInputs; }

// 启用/禁用键盘输入处理
inline void SetHandleKeyboardInputs(bool aValue) { mHandleKeyboardInputs = aValue; }
inline bool IsHandleKeyboardInputsEnabled() const { return mHandleKeyboardInputs; }

// 忽略 ImGui 子窗口
inline void SetImGuiChildIgnored(bool aValue) { mIgnoreImGuiChild = aValue; }
inline bool IsImGuiChildIgnored() const { return mIgnoreImGuiChild; }

// 启用/禁用显示空白字符
inline void SetShowWhitespaces(bool aValue) { mShowWhitespaces = aValue; }
inline bool IsShowingWhitespaces() const { return mShowWhitespaces; }

// 设置制表符大小
void SetTabSize(int aValue);
inline int GetTabSize() const { return mTabSize; }

// 插入文本
void InsertText(const std::string& aValue);
void InsertText(const char* aValue);

// 光标向上移动
void MoveUp(int aAmount = 1, bool aSelect = false);

// 光标向下移动
void MoveDown(int aAmount = 1, bool aSelect = false);

// 光标向左移动
void MoveLeft(int aAmount = 1, bool aSelect = false, bool aWordMode = false);

// 光标向右移动
void MoveRight(int aAmount = 1, bool aSelect = false, bool aWordMode = false);

// 移动到文本顶部
void MoveTop(bool aSelect = false);

// 移动到文本底部
void MoveBottom(bool aSelect = false);

// 移动到行首
void MoveHome(bool aSelect = false);

// 移动到行末
void MoveEnd(bool aSelect = false);

// 设置选择起始位置
void SetSelectionStart(const Coordinates& aPosition);

// 设置选择结束位置
void SetSelectionEnd(const Coordinates& aPosition);

// 设置选择区域
void SetSelection(const Coordinates& aStart, const Coordinates& aEnd, SelectionMode aMode = SelectionMode::Normal);

// 选择光标所在的单词
void SelectWordUnderCursor();

// 选择全部文本
void SelectAll();

// 判断是否存在选择
bool HasSelection() const;


	// 复制选中文本
void Copy();

// 剪切选中文本
void Cut();

// 粘贴文本
void Paste();

// 删除选中文本或删除光标处的字符
void Delete();

// 检查是否可以执行撤销操作
bool CanUndo() const;

// 检查是否可以执行重做操作
bool CanRedo() const;

// 撤销操作
void Undo(int aSteps = 1);

// 重做操作
void Redo(int aSteps = 1);

// 获取深色主题调色板
static const Palette& GetDarkPalette();

// 获取浅色主题调色板
static const Palette& GetLightPalette();

// 获取复古蓝主题调色板
static const Palette& GetRetroBluePalette();

private:
    // 正则表达式列表，每个元素是一对（正则表达式，调色板索引）
    typedef std::vector<std::pair<std::regex, PaletteIndex>> RegexList;

    // 编辑器状态结构体
    struct EditorState
    {
        Coordinates mSelectionStart;
        Coordinates mSelectionEnd;
        Coordinates mCursorPosition;
    };

    // 撤销记录类
    class UndoRecord
    {
    public:
// 默认构造函数
UndoRecord() {}

// 析构函数
~UndoRecord() {}

// 带参数的构造函数，用于初始化 UndoRecord 对象
UndoRecord(
    const std::string& aAdded,                   // 已添加的文本
    const TextEditor::Coordinates aAddedStart,   // 已添加文本的起始坐标
    const TextEditor::Coordinates aAddedEnd,     // 已添加文本的结束坐标

    const std::string& aRemoved,                 // 已删除的文本
    const TextEditor::Coordinates aRemovedStart, // 已删除文本的起始坐标
    const TextEditor::Coordinates aRemovedEnd,   // 已删除文本的结束坐标

    TextEditor::EditorState& aBefore,            // 操作前的编辑器状态
    TextEditor::EditorState& aAfter);            // 操作后的编辑器状态



        // 执行撤销操作
        void Undo(TextEditor* aEditor);

        // 执行重做操作
        void Redo(TextEditor* aEditor);

        // 已添加的文本
		std::string mAdded;
		// 已添加文本的起始坐标
		Coordinates mAddedStart;
		// 已添加文本的结束坐标
		Coordinates mAddedEnd;


		// 已删除的文本
		std::string mRemoved;
		// 已删除文本的起始坐标
		Coordinates mRemovedStart;
		// 已删除文本的结束坐标
		Coordinates mRemovedEnd;


        // 操作前的编辑器状态
        EditorState mBefore;

        // 操作后的编辑器状态
        EditorState mAfter;
    };

// 撤销缓冲区，存储撤销记录
typedef std::vector<UndoRecord> UndoBuffer;

// 处理用户输入
void ProcessInputs();

// 对文本进行着色
void Colorize(int aFromLine = 0, int aCount = -1);

// 对指定范围的文本进行着色
void ColorizeRange(int aFromLine = 0, int aToLine = 0);

// 内部文本着色函数
void ColorizeInternal();

// 计算文本距离行起始位置的距离
float TextDistanceToLineStart(const Coordinates& aFrom) const;

// 确保光标可见
void EnsureCursorVisible();

// 获取页面大小
int GetPageSize() const;

// 获取指定坐标范围内的文本
std::string GetText(const Coordinates& aStart, const Coordinates& aEnd) const;

// 获取实际光标坐标
Coordinates GetActualCursorCoordinates() const;

// 校正坐标值，确保在有效范围内
Coordinates SanitizeCoordinates(const Coordinates& aValue) const;

// 将坐标向前移动
void Advance(Coordinates& aCoordinates) const;

// 删除指定范围的文本
void DeleteRange(const Coordinates& aStart, const Coordinates& aEnd);

// 在指定坐标处插入文本
int InsertTextAt(Coordinates& aWhere, const char* aValue);

// 添加撤销记录
void AddUndo(UndoRecord& aValue);

// 将屏幕位置转换为坐标
Coordinates ScreenPosToCoordinates(const ImVec2& aPosition) const;

// 查找单词的起始位置
Coordinates FindWordStart(const Coordinates& aFrom) const;

// 查找单词的结束位置
Coordinates FindWordEnd(const Coordinates& aFrom) const;

// 查找下一个单词的位置
Coordinates FindNextWord(const Coordinates& aFrom) const;

// 获取坐标对应的字符索引
int GetCharacterIndex(const Coordinates& aCoordinates) const;

// 获取指定行、指定索引的字符列数
int GetCharacterColumn(int aLine, int aIndex) const;

// 获取指定行的字符总数
int GetLineCharacterCount(int aLine) const;

// 获取指定行的最大列数
int GetLineMaxColumn(int aLine) const;

// 判断坐标是否位于单词边界
bool IsOnWordBoundary(const Coordinates& aAt) const;

// 移除指定范围的行
void RemoveLine(int aStart, int aEnd);

// 移除指定行
void RemoveLine(int aIndex);

// 在指定位置插入新行
Line& InsertLine(int aIndex);

// 输入字符（处理回车、Tab等）
void EnterCharacter(ImWchar aChar, bool aShift);

// 退格键处理
void Backspace();

// 删除选中文本
void DeleteSelection();

// 获取光标下的单词
std::string GetWordUnderCursor() const;

// 获取指定坐标处的单词
std::string GetWordAt(const Coordinates& aCoords) const;

// 获取字形的颜色
ImU32 GetGlyphColor(const Glyph& aGlyph) const;

// 处理键盘输入
void HandleKeyboardInputs();

// 处理鼠标输入
void HandleMouseInputs();

// 渲染文本编辑器
void Render();


// 行间距
float mLineSpacing;

// 存储文本行的容器
Lines mLines;

// 编辑器状态
EditorState mState;

// 撤销缓冲区
UndoBuffer mUndoBuffer;

// 撤销操作索引
int mUndoIndex;

// 制表符大小
int mTabSize;

// 覆盖模式开关
bool mOverwrite;

// 只读模式开关
bool mReadOnly;

// 渲染过程标志
bool mWithinRender;

// 是否滚动到光标位置
bool mScrollToCursor;

// 是否滚动到顶部
bool mScrollToTop;

// 文本是否被修改
bool mTextChanged;

// 语法高亮是否启用
bool mColorizerEnabled;

// 文本起始位置（相对于 TextEditor 左侧的像素位置）
float mTextStart;

// 左边距
int mLeftMargin;

// 光标位置是否发生变化
bool mCursorPositionChanged;

// 语法高亮的颜色范围
int mColorRangeMin, mColorRangeMax;

// 选择模式
SelectionMode mSelectionMode;

// 是否处理键盘输入
bool mHandleKeyboardInputs;

// 是否处理鼠标输入
bool mHandleMouseInputs;

// 是否忽略 ImGui 子窗口
bool mIgnoreImGuiChild;

// 是否显示空白字符
bool mShowWhitespaces;

// 基础调色板
Palette mPaletteBase;

// 当前调色板
Palette mPalette;

// 语言定义
LanguageDefinition mLanguageDefinition;

// 正则表达式列表
RegexList mRegexList;

// 是否检查注释
bool mCheckComments;

// 断点行集合
Breakpoints mBreakpoints;

// 错误标记集合
ErrorMarkers mErrorMarkers;

// 字符间距
ImVec2 mCharAdvance;

// 交互式选择起始和结束坐标
Coordinates mInteractiveStart, mInteractiveEnd;

// 临时存储行数据的缓冲区
std::string mLineBuffer;

// 渲染开始时间
uint64_t mStartTime;

// 上一次点击的时间
float mLastClick;

};
