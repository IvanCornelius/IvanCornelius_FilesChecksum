// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ОДНОГО ФАЙЛА
// ДОП. ДАННЫЕ: 20.01.2025; v1.0.0_SignalsSlots.
#include "IvanCornelius_OneFileChecksum.hpp"
#include <QFile>
#include <QCryptographicHash>
#include <QFileInfo>


// КОНСТРУКТОР (БАЗОВОЕ ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ)
IvanCornelius_OneFileChecksum::IvanCornelius_OneFileChecksum(const QString &filePath, QObject *parent)
    : QObject(parent)
    , m_filePath(filePath)
{}


// ПРОЦЕСС РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ
void IvanCornelius_OneFileChecksum::process()
{
    // Открытие файла
    QFileInfo fileInfo(m_filePath);

    if (!fileInfo.isReadable())
    {
        emit errorOccurred(m_filePath, QStringLiteral("Cannot read file"));
        emit finished();
        return;
    }

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred(m_filePath, file.errorString());
        emit finished();
        return;
    }

    // Расчёт контрольный суммы
    QCryptographicHash hash(QCryptographicHash::Md5);

    const qint64 bufferSize = 64 * 1024; // 64 KB буффер

    while (!file.atEnd()) {
        QByteArray buffer = file.read(bufferSize);
        if (buffer.isEmpty() && !file.atEnd())
        {
            emit errorOccurred(m_filePath, QStringLiteral("Error reading file"));
            emit finished();
            return;
        }
        hash.addData(buffer);
    }

    // Отправка результата
    file.close();

    QString hashString = QString::fromLatin1(hash.result().toHex());
    emit resultReady(m_filePath, hashString);
    emit finished();
}
