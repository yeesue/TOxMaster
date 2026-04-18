#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QTranslator>
#include <QLocale>
#include <memory>

// 支持的语言枚举
enum class Language {
    English,    // 英语
    Chinese,    // 简体中文
    Chinese_TW, // 繁体中文
    Japanese,   // 日语
    German,     // 德语
    French,     // 法语
    Spanish,    // 西班牙语
    Russian     // 俄语
};

// 语言信息结构
struct LanguageInfo {
    Language language;
    QString code;           // 语言代码 (如 "zh_CN", "en_US")
    QString name;           // 语言名称 (如 "简体中文", "English")
    QString nativeName;     // 本地名称 (如 "简体中文", "English")
    QString flag;           // 国旗emoji或图标
    bool isRTL = false;     // 是否从右到左书写
    
    LanguageInfo() = default;
    LanguageInfo(Language lang, const QString& c, const QString& n, 
                 const QString& nn, const QString& f = "", bool rtl = false)
        : language(lang), code(c), name(n), nativeName(nn), flag(f), isRTL(rtl) {}
};

class LanguageManager : public QObject
{
    Q_OBJECT
public:
    explicit LanguageManager(QObject *parent = nullptr);
    ~LanguageManager();

    // 单例模式
    static LanguageManager* instance();

    // 语言管理
    bool loadLanguage(Language language);
    bool loadLanguage(const QString& languageCode);
    void unloadCurrentLanguage();
    
    // 获取当前语言
    Language getCurrentLanguage() const;
    QString getCurrentLanguageCode() const;
    QString getCurrentLanguageName() const;
    
    // 获取支持的语言列表
    static QList<LanguageInfo> getSupportedLanguages();
    static QStringList getSupportedLanguageCodes();
    static QStringList getSupportedLanguageNames();
    
    // 语言代码转换
    static Language languageFromCode(const QString& code);
    static QString languageToCode(Language language);
    static QString languageToName(Language language);
    static QString languageToNativeName(Language language);
    
    // 检查语言是否支持
    static bool isLanguageSupported(const QString& code);
    static bool isLanguageSupported(Language language);
    
    // 翻译文件管理
    bool loadTranslator(const QString& filePath);
    void removeTranslator();
    
    // 获取翻译字符串
    QString translate(const QString& context, const QString& text) const;
    QString translate(const QString& context, const QString& text, const QString& disambiguation) const;
    
    // 动态翻译
    void retranslateUi(QObject* ui);
    
    // 设置系统语言
    bool setSystemLanguage();
    
    // 保存和加载语言设置
    bool saveLanguageSettings(const QString& settingsFile);
    bool loadLanguageSettings(const QString& settingsFile);

signals:
    void languageChanged(Language newLanguage);
    void languageChanged(const QString& languageCode);

private:
    // 初始化支持的语言
    void initializeSupportedLanguages();
    
    // 获取翻译文件路径
    QString getTranslationFilePath(Language language);
    QString getTranslationFileName(Language language);
    
    // 确保翻译目录存在
    bool ensureTranslationDirectoryExists();

private:
    static LanguageManager* s_instance;
    
    Language m_currentLanguage = Language::English;
    std::unique_ptr<QTranslator> m_translator;
    QMap<Language, LanguageInfo> m_languageMap;
    
    QString m_translationDirectory;
    QString m_applicationName;
};

// 便捷的翻译宏
#define TR(text) LanguageManager::instance()->translate("", text)
#define TR_CTX(context, text) LanguageManager::instance()->translate(context, text)

#endif // LANGUAGE_MANAGER_H
