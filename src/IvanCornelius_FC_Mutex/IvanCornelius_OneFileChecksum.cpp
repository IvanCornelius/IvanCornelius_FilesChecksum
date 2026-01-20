// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ОДНОГО ФАЙЛА
// ДОП. ДАННЫЕ: 20.01.2025; v1.0.0_Mutex.
#include "IvanCornelius_OneFileChecksum.hpp"
#include <QFile>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QTextStream>
#include <QMutexLocker>


// КОНСТРУКТОР (БАЗОВОЕ ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ)
IvanCornelius_OneFileChecksum::IvanCornelius_OneFileChecksum(const QString &filePath,
                               QMap<QString, QString> *results,
                               QMutex *mutex,
                               QObject *parent)
    : QThread(parent)
    , m_filePath(filePath)
    , m_results(results)
    , m_mutex(mutex)
{}


// ПРОЦЕСС РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ
void IvanCornelius_OneFileChecksum::run()
{
    // Открытие файла
    QFileInfo fileInfo(m_filePath);

    if (!fileInfo.isReadable()) {
        QTextStream err(stderr);
        err << "Error: Cannot read file: " << m_filePath << "\n";
        return;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QTextStream err(stderr);
        err << "Error opening " << m_filePath << ": " << file.errorString() << "\n";
        return;
    }

    // Расчёт контрольный суммы
    QCryptographicHash hash(QCryptographicHash::Md5);

    const qint64 bufferSize = 64 * 1024; // 64 Кб буфер

    while (!file.atEnd()) {
        QByteArray buffer = file.read(bufferSize);
        if (buffer.isEmpty() && !file.atEnd()) {
            QTextStream err(stderr);
            err << "Error reading file: " << m_filePath << "\n";
            return;
        }
        hash.addData(buffer);
    }

    // Отправка результата
    file.close();

    QString hashString = QString::fromLatin1(hash.result().toHex());

    QMutexLocker locker(m_mutex);
    m_results->insert(m_filePath, hashString);
}
