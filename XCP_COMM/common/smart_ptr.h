#ifndef SMART_PTR_H
#define SMART_PTR_H

#include <QSharedPointer>
#include <memory>

/**
 * @brief 线程安全的字节数组智能指针
 * 用于跨线程信号槽传递数据，自动管理内存
 */
using ByteArrayPtr = QSharedPointer<quint8>;

/**
 * @brief 创建字节数组智能指针
 * @param size 数组大小
 * @return 智能指针，引用计数归零时自动释放
 */
inline ByteArrayPtr makeByteArray(int size) {
    return ByteArrayPtr(new quint8[size], [](quint8* p) { delete[] p; });
}

/**
 * @brief 线程安全的字符数组智能指针
 */
using CharArrayPtr = QSharedPointer<char>;

/**
 * @brief 创建字符数组智能指针
 * @param size 数组大小
 * @return 智能指针
 */
inline CharArrayPtr makeCharArray(int size) {
    return CharArrayPtr(new char[size], [](char* p) { delete[] p; });
}

/**
 * @brief 线程安全的无符号字节数组智能指针
 */
using UByteArrayPtr = QSharedPointer<unsigned char>;

/**
 * @brief 创建无符号字节数组智能指针
 */
inline UByteArrayPtr makeUByteArray(int size) {
    return UByteArrayPtr(new unsigned char[size], [](unsigned char* p) { delete[] p; });
}

#endif // SMART_PTR_H
