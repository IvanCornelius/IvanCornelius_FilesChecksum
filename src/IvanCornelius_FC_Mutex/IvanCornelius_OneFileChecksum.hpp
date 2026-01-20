// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ОДНОГО ФАЙЛА
// ДОП. ДАННЫЕ: 20.01.2025; v1.0.0_Mutex.
#ifndef IVANCORNELIUS_ONEFILECHECKSUM_H
#define IVANCORNELIUS_ONEFILECHECKSUM_H
#include <QThread>
#include <QString>
#include <QMap>
#include <QMutex>


/// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ОДНОГО ФАЙЛА
class IvanCornelius_OneFileChecksum : public QThread
{
    Q_OBJECT
public:
    /** Конструктор (базовое объявление переменных).
     *  results - это список всех результатов (хранится у родителя). */
    IvanCornelius_OneFileChecksum(const QString &filePath,
                   QMap<QString, QString> *results,
                   QMutex *mutex,
                   QObject *parent = nullptr);
protected:
    /** Процесс расчёта контрольной суммы (буффер 64 Кб).
     *  Результат добавляем в results. */
    void run() override;
private:
    QString m_filePath;                ///< Путь к файлу
    QMap<QString, QString> *m_results; ///< Результат
    QMutex *m_mutex; ///< Для защиты приложения при работе с многопоточностью
};

#endif // IVANCORNELIUS_ONEFILECHECKSUM_H
