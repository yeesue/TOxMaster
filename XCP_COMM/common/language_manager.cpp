#include "language_manager.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

LanguageManager* LanguageManager::s_instance = nullptr;

LanguageManager::LanguageManager(QObject *parent) : QObject(parent)
{
    initializeSupportedLanguages();

    m_applicationName = QCoreApplication::applicationName();
    if (m_applicationName.isEmpty()) {
        m_applicationName = "XCP_COMM";
    }

    m_translationDirectory = QCoreApplication::applicationDirPath() + "/translations";
}

LanguageManager::~LanguageManager()
{
    removeTranslator();
}

LanguageManager* LanguageManager::instance()
{
    if (!s_instance) {
        s_instance = new LanguageManager();
    }
    return s_instance;
}

bool LanguageManager::loadLanguage(Language language)
{
    if (m_currentLanguage == language) {
        return true;
    }

    if (!isLanguageSupported(language)) {
        qWarning() << "Language not supported:" << static_cast<int>(language);
        return false;
    }

    // 卸载当前语言
    unloadCurrentLanguage();

    // 加载新语言
    QString translationFile = getTranslationFilePath(language);
    if (!loadTranslator(translationFile)) {
        qWarning() << "Failed to load translation file:" << translationFile;
        // 如果加载失败，尝试使用默认翻译
        if (language != Language::English) {
            qInfo() << "Falling back to English";
            return loadLanguage(Language::English);
        }
        return false;
    }

    m_currentLanguage = language;

    emit languageChanged(language);
    emit languageChanged(languageToCode(language));

    qInfo() << "Language changed to:" << languageToName(language);

    return true;
}

bool LanguageManager::loadLanguage(const QString& languageCode)
{
    Language language = languageFromCode(languageCode);
    return loadLanguage(language);
}

void LanguageManager::unloadCurrentLanguage()
{
    removeTranslator();
    m_currentLanguage = Language::English;
}

Language LanguageManager::getCurrentLanguage() const
{
    return m_currentLanguage;
}

QString LanguageManager::getCurrentLanguageCode() const
{
    return languageToCode(m_currentLanguage);
}

QString LanguageManager::getCurrentLanguageName() const
{
    return languageToName(m_currentLanguage);
}

QList<LanguageInfo> LanguageManager::getSupportedLanguages()
{
    LanguageManager* mgr = instance();
    return mgr->m_languageMap.values();
}

QStringList LanguageManager::getSupportedLanguageCodes()
{
    QStringList codes;
    for (const auto& info : getSupportedLanguages()) {
        codes.append(info.code);
    }
    return codes;
}

QStringList LanguageManager::getSupportedLanguageNames()
{
    QStringList names;
    for (const auto& info : getSupportedLanguages()) {
        names.append(info.name);
    }
    return names;
}

Language LanguageManager::languageFromCode(const QString& code)
{
    QString lowerCode = code.toLower();

    if (lowerCode == "en" || lowerCode == "en_us" || lowerCode == "en_gb") {
        return Language::English;
    } else if (lowerCode == "zh" || lowerCode == "zh_cn" || lowerCode == "zh_sg") {
        return Language::Chinese;
    } else if (lowerCode == "zh_tw" || lowerCode == "zh_hk" || lowerCode == "zh_mo") {
        return Language::Chinese_TW;
    } else if (lowerCode == "ja" || lowerCode == "jp") {
        return Language::Japanese;
    } else if (lowerCode == "de" || lowerCode == "de_de") {
        return Language::German;
    } else if (lowerCode == "fr" || lowerCode == "fr_fr") {
        return Language::French;
    } else if (lowerCode == "es" || lowerCode == "es_es") {
        return Language::Spanish;
    } else if (lowerCode == "ru" || lowerCode == "ru_ru") {
        return Language::Russian;
    }

    return Language::English; // 默认返回英语
}

QString LanguageManager::languageToCode(Language language)
{
    switch (language) {
    case Language::English: return "en_US";
    case Language::Chinese: return "zh_CN";
    case Language::Chinese_TW: return "zh_TW";
    case Language::Japanese: return "ja_JP";
    case Language::German: return "de_DE";
    case Language::French: return "fr_FR";
    case Language::Spanish: return "es_ES";
    case Language::Russian: return "ru_RU";
    default: return "en_US";
    }
}

QString LanguageManager::languageToName(Language language)
{
    switch (language) {
    case Language::English: return "English";
    case Language::Chinese: return "简体中文";
    case Language::Chinese_TW: return "繁體中文";
    case Language::Japanese: return "日本語";
    case Language::German: return "Deutsch";
    case Language::French: return "Français";
    case Language::Spanish: return "Español";
    case Language::Russian: return "Русский";
    default: return "English";
    }
}

QString LanguageManager::languageToNativeName(Language language)
{
    switch (language) {
    case Language::English: return "English";
    case Language::Chinese: return "简体中文";
    case Language::Chinese_TW: return "繁體中文";
    case Language::Japanese: return "日本語";
    case Language::German: return "Deutsch";
    case Language::French: return "Français";
    case Language::Spanish: return "Español";
    case Language::Russian: return "Русский";
    default: return "English";
    }
}

bool LanguageManager::isLanguageSupported(const QString& code)
{
    Language lang = languageFromCode(code);
    return isLanguageSupported(lang);
}

bool LanguageManager::isLanguageSupported(Language language)
{
    LanguageManager* mgr = instance();
    return mgr->m_languageMap.contains(language);
}

bool LanguageManager::loadTranslator(const QString& filePath)
{
    if (!QFile::exists(filePath)) {
        qWarning() << "Translation file does not exist:" << filePath;
        return false;
    }

    m_translator.reset(new QTranslator());

    if (!m_translator->load(filePath)) {
        qWarning() << "Failed to load translator from:" << filePath;
        m_translator.reset();
        return false;
    }

    if (!QCoreApplication::installTranslator(m_translator.get())) {
        qWarning() << "Failed to install translator";
        m_translator.reset();
        return false;
    }

    return true;
}

void LanguageManager::removeTranslator()
{
    if (m_translator) {
        QCoreApplication::removeTranslator(m_translator.get());
        m_translator.reset();
    }
}

QString LanguageManager::translate(const QString& context, const QString& text) const
{
    if (text.isEmpty()) {
        return text;
    }

    return QCoreApplication::translate(context.toUtf8().constData(),
                                       text.toUtf8().constData());
}

QString LanguageManager::translate(const QString& context, const QString& text,
                                  const QString& disambiguation) const
{
    if (text.isEmpty()) {
        return text;
    }

    return QCoreApplication::translate(context.toUtf8().constData(),
                                       text.toUtf8().constData(),
                                       disambiguation.toUtf8().constData());
}

void LanguageManager::retranslateUi(QObject* ui)
{
    if (!ui) {
        return;
    }

    // 触发UI重新翻译
    QMetaObject::invokeMethod(ui, "retranslateUi", Qt::QueuedConnection);
}

bool LanguageManager::setSystemLanguage()
{
    QLocale systemLocale = QLocale::system();
    QString languageCode = systemLocale.name();

    qInfo() << "System language:" << languageCode;

    return loadLanguage(languageCode);
}

bool LanguageManager::saveLanguageSettings(const QString& settingsFile)
{
    QJsonObject settings;
    settings["language"] = getCurrentLanguageCode();
    settings["language_name"] = getCurrentLanguageName();

    QJsonDocument doc(settings);
    QFile file(settingsFile);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open settings file for writing:" << settingsFile;
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

bool LanguageManager::loadLanguageSettings(const QString& settingsFile)
{
    QFile file(settingsFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open settings file for reading:" << settingsFile;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid settings file format";
        return false;
    }

    QJsonObject settings = doc.object();
    QString languageCode = settings["language"].toString();

    if (!languageCode.isEmpty()) {
        return loadLanguage(languageCode);
    }

    return false;
}

void LanguageManager::initializeSupportedLanguages()
{
    m_languageMap[Language::English] = LanguageInfo(
        Language::English, "en_US", "English", "English", "", false);

    m_languageMap[Language::Chinese] = LanguageInfo(
        Language::Chinese, "zh_CN", "简体中文", "简体中文", "", false);

    m_languageMap[Language::Chinese_TW] = LanguageInfo(
        Language::Chinese_TW, "zh_TW", "繁體中文", "繁體中文", "", false);

    m_languageMap[Language::Japanese] = LanguageInfo(
        Language::Japanese, "ja_JP", "日本語", "日本語", "", false);

    m_languageMap[Language::German] = LanguageInfo(
        Language::German, "de_DE", "Deutsch", "Deutsch", "", false);

    m_languageMap[Language::French] = LanguageInfo(
        Language::French, "fr_FR", "Français", "Français", "", false);

    m_languageMap[Language::Spanish] = LanguageInfo(
        Language::Spanish, "es_ES", "Español", "Español", "", false);

    m_languageMap[Language::Russian] = LanguageInfo(
        Language::Russian, "ru_RU", "Русский", "Русский", "", false);
}

QString LanguageManager::getTranslationFilePath(Language language)
{
    QString fileName = getTranslationFileName(language);
    return m_translationDirectory + "/" + fileName;
}

QString LanguageManager::getTranslationFileName(Language language)
{
    return m_applicationName + "_" + languageToCode(language) + ".qm";
}

bool LanguageManager::ensureTranslationDirectoryExists()
{
    QDir dir(m_translationDirectory);
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}