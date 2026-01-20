// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ВСЕХ ФАЙЛОВ В КАТАЛОГЕ
// ДОП. ДАННЫЕ: 20.01.2025; v1.0.0_Mutex.
#ifndef IVANCORNELIUS_FILESCHECKSUM_H
#define IVANCORNELIUS_FILESCHECKSUM_H
#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QMutex>
#include <QList>
#include "IvanCornelius_OneFileChecksum.hpp"


/// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ВСЕХ ФАЙЛОВ В КАТАЛОГЕ
class IvanCornelius_FilesChecksum : public QObject
{
    Q_OBJECT
public:
    /// Конструктор (базовое объявление переменных + выбор количества потоков (по умолчанию 4))
    explicit IvanCornelius_FilesChecksum(const QString &directoryPath, QObject *parent = nullptr);
    ~IvanCornelius_FilesChecksum();
    /// Запуск расчёта всех контрольных сумм
    void start();
    /// Получить результаты контрольных сумм файлов (ключ - имя и путь файла, значение - контрольная сумма)
    QMap<QString, QString> results() const;
private:
    /// Поиск файлов в каталоге
    void scanDirectory();
    /// Создание новых задач расчёта контрольных сумм файлов
    void processFiles();
    /// Вывод итогового результата расчёта контрольной суммы для всех файлов
    void printResults();
    // Переменные класса
    QString m_directoryPath;            ///< Путь к каталогу
    QStringList m_filesToProcess;       ///< Список всех файлов для обработки
    QMap<QString, QString> m_results;   ///< Результат (ключ - имя и путь файла, значение - контрольная сумма)
    QMutex m_mutex;                     ///< Для защиты приложения при работе с многопоточностью
    int m_maxThreads;                   ///< Максимальное количество возможных потоков
    QList<IvanCornelius_OneFileChecksum*> m_threads; ///< Список потоков
};


#endif // IVANCORNELIUS_FILESCHECKSUM_H
