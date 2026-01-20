#include "IvanCornelius_FilesChecksum.hpp"
#include "IvanCornelius_OneFileChecksum.hpp"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QTextStream>
#include <QThread>


// КОНСТРУКТОР (БАЗОВОЕ ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ + ВЫБОР КОЛИЧЕСТВА ПОТОКОВ)
IvanCornelius_FilesChecksum::IvanCornelius_FilesChecksum(const QString &directoryPath, QObject *parent)
    : QObject(parent)
    , m_directoryPath(directoryPath)
    , m_maxThreads(QThread::idealThreadCount())
{
    if (m_maxThreads < 1)
    {
        m_maxThreads = 4;
    }
}


// ДЕСТРУКТОР (ОЧИСТКА ДИНАМИЧЕСКОЙ ПАМЯТИ + ЗАКРЫТИЕ ВСЕХ ПОТОКОВ)
IvanCornelius_FilesChecksum::~IvanCornelius_FilesChecksum()
{
    for (auto it = m_threads.begin(); it != m_threads.end(); ++it)
    {
        IvanCornelius_OneFileChecksum *thread = *it;
        thread->wait();
        delete thread;
    }
    m_threads.clear();
}


// ЗАПУСК РАСЧЁТА ВСЕХ КОНТРОЛЬНЫХ СУММ
void IvanCornelius_FilesChecksum::start()
{
    scanDirectory();

    if (m_filesToProcess.isEmpty())
    {
        QTextStream err(stderr);
        err << "No files found in directory: " << m_directoryPath << "\n";
        return;
    }

    processFiles();
    printResults();
}


// ПОЛУЧИТЬ РЕЗУЛЬТАТЫ КОНТРОЛЬНЫХ СУММ ФАЙЛОВ
QMap<QString, QString> IvanCornelius_FilesChecksum::results() const
{
    return m_results;
}


// ПОИСК ФАЙЛОВ В КАТАЛОГЕ
void IvanCornelius_FilesChecksum::scanDirectory()
{
    QFileInfo dirInfo(m_directoryPath);
    if (!dirInfo.exists())
    {
        QTextStream err(stderr);
        err << "Directory does not exist: " << m_directoryPath << "\n";
        return;
    }

    if (!dirInfo.isDir())
    {
        QTextStream err(stderr);
        err << "Path is not a directory: " << m_directoryPath << "\n";
        return;
    }

    if (!dirInfo.isReadable())
    {
        QTextStream err(stderr);
        err << "Cannot read directory: " << m_directoryPath << "\n";
        return;
    }

    QDirIterator it(m_directoryPath, QDir::Files | QDir::Readable,
                    QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        if (fileInfo.isReadable())
        {
            m_filesToProcess.append(filePath);
        }
    }
}



void IvanCornelius_FilesChecksum::processFiles()
{
    /// Индекс текущего файла
    int fileIndex = 0;
    /// количество всех файлов для обработки
    int totalFiles = m_filesToProcess.size();

    // Цикл обработки
    while (fileIndex < totalFiles)
    {
        // Очистка завершенных потоков
        for (int i = m_threads.size() - 1; i >= 0; --i)
        {
            if (m_threads[i]->isFinished())
            {
                m_threads[i]->wait();
                delete m_threads[i];
                m_threads.removeAt(i);
            }
        }

        // Запускайте новых потоков до максимального количества
        while (m_threads.size() < m_maxThreads && fileIndex < totalFiles)
        {
            QString filePath = m_filesToProcess.at(fileIndex);
            ++fileIndex;

            IvanCornelius_OneFileChecksum *new_thread = new IvanCornelius_OneFileChecksum(filePath, &m_results, &m_mutex);
            m_threads.append(new_thread);
            new_thread->start();
        }

        // Дожидаемся завершения хотя бы одного потока
        if (m_threads.size() >= m_maxThreads && fileIndex < totalFiles)
        {
            for (auto it = m_threads.begin(); it != m_threads.end(); ++it)
            {
                IvanCornelius_OneFileChecksum *thread = *it;
                if (!thread->isFinished())
                {
                    thread->wait();
                    break;
                }
            }
        }
    }

    // Дожидаемся завершения всех оставшихся потоков
    for (auto it = m_threads.begin(); it != m_threads.end(); ++it)
    {
        IvanCornelius_OneFileChecksum *thread = *it;
        thread->wait();
        delete thread;
    }
    m_threads.clear();
}


//  ВЫВОД ИТОГОВОГО РЕЗУЛЬТАТА РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ДЛЯ ВСЕХ ФАЙЛОВ
void IvanCornelius_FilesChecksum::printResults()
{
    QTextStream out(stdout);

    for (auto it = m_results.constBegin(); it != m_results.constEnd(); ++it)
    {
        out << it.key() << "  " << it.value() << "\n";
    }
}
