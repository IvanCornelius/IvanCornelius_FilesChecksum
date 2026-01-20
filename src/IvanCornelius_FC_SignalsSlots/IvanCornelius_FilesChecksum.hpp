// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ВСЕХ ФАЙЛОВ В КАТАЛОГЕ
// ДОП. ДАННЫЕ: 20.01.2025; v1.0.0_SignalsSlots.
#ifndef IVANCORNELIUS_FILESCHECKSUM_H
#define IVANCORNELIUS_FILESCHECKSUM_H
#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QThread>


/// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ВСЕХ ФАЙЛОВ В КАТАЛОГЕ
class IvanCornelius_FilesChecksum : public QObject
{
    Q_OBJECT
public:
    /// Конструктор (базовое объявление переменных + выбор количества потоков (по умолчанию 4))
    explicit IvanCornelius_FilesChecksum(const QString &directoryPath,
                                         QObject *parent = nullptr);
    /// Деструктор (очистка динамической памяти + закрытие всех потоков)
    ~IvanCornelius_FilesChecksum();
    /// Запуск расчёта всех контрольных сумм
    void start();
    /// Получить результаты контрольных сумм файлов (ключ - имя и путь файла, значение - контрольная сумма)
    QMap<QString, QString> results() const;
signals:
    /// Сигнал о завершении расчёта
    void finished();
private slots:
    /// Слот получения результата расчёта контрольной суммы одного файла
    void onResultReady(const QString &path, const QString &hash);
    /// Слот получение и вывод ошибки расчёта контрольной суммы одного файла
    void onErrorOccurred(const QString &path, const QString &error);
    /// Слот получение сообщения о завершении, создание новой задачи или вывод итогового результата
    void onWorkerFinished();
private:
    /// Поиск файлов в каталоге
    void scanDirectory();
    /// Создания новой задачи расчёта контрольной суммы одного файла
    void processNextFile();
    /// Вывод итогового результата расчёта контрольной суммы для всех файлов
    void printResults();
    // Переменные класса
    QString m_directoryPath;          ///< Путь к каталогу
    QStringList m_filesToProcess;     ///< Список всех файлов для обработки
    QMap<QString, QString> m_results; ///< Результат (ключ - имя и путь файла, значение - контрольная сумма)
    int m_maxThreads;                 ///< Максимальное количество возможных потоков
    int m_activeThreads;              ///< Количество активных потоков
    int m_currentFileIndex;           ///< Текущий файл для расчёта контрольной суммы
    QList<QThread*> m_threads;        ///< Список потоков
};


#endif // IVANCORNELIUS_FILESCHECKSUM_H
