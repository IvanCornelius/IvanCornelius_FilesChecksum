// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ОДНОГО ФАЙЛА
// ДОП. ДАННЫЕ: 20.01.2025; v1.0.0_SignalsSlots.
#ifndef IVANCORNELIUS_ONEFILECHECKSUM_H
#define IVANCORNELIUS_ONEFILECHECKSUM_H
#include <QObject>
#include <QString>


/// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ОДНОГО ФАЙЛА
class IvanCornelius_OneFileChecksum : public QObject
{
    Q_OBJECT
public:
    /// Конструктор (базовое объявление переменных)
    explicit IvanCornelius_OneFileChecksum(const QString &filePath,
                                           QObject *parent = nullptr);
public slots:
    /// Процесс расчёта контрольной суммы (буффер 64 Кб)
    void process();
signals:
    /// Сигнал выдачи результата расчёта контрольной суммы одного файла
    void resultReady(const QString &path, const QString &hash);
    /// Сигнал выдачи ошибки расчёта контрольной суммы одного файла
    void errorOccurred(const QString &path, const QString &error);
    /// Сигнал о завершении расчёта
    void finished();
private:
    QString m_filePath; ///< Путь к файлу
};

#endif // IVANCORNELIUS_ONEFILECHECKSUM_H
