// КЛАСС ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ФАЙЛОВ В КАТАЛОГЕ
// ДОП. ДАННЫЕ: 20.01.2025; v1.0.0_SignalsSlots.
#include "IvanCornelius_FilesChecksum.hpp"
#include "IvanCornelius_OneFileChecksum.hpp"
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QTextStream>
#include <QCoreApplication>


// КОНСТРУКТОР (БАЗОВОЕ ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ + ВЫБОР КОЛИЧЕСТВА ПОТОКОВ)
IvanCornelius_FilesChecksum::IvanCornelius_FilesChecksum(const QString &directoryPath, QObject *parent)
    : QObject(parent)
    , m_directoryPath(directoryPath)
    , m_maxThreads(QThread::idealThreadCount())
    , m_activeThreads(0)
    , m_currentFileIndex(0)
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
        QThread *thread = *it;
        thread->quit();
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
        emit finished();
        return;
    }

    int initialThreads = qMin(m_maxThreads, m_filesToProcess.size());
    for (int i = 0; i < initialThreads; ++i)
    {
        processNextFile();
    }
}


// ПОЛУЧИТЬ РЕЗУЛЬТАТЫ КОНТРОЛЬНЫХ СУММ ФАЙЛОВ
QMap<QString, QString> IvanCornelius_FilesChecksum::results() const
{
    return m_results;
}


// СЛОТ ПОЛУЧЕНИЯ РЕЗУЛЬТАТА РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ОДНОГО ФАЙЛА
void IvanCornelius_FilesChecksum::onResultReady(const QString &path, const QString &hash)
{
    m_results.insert(path, hash);
}


// СЛОТ ПОЛУЧЕНИЕ И ВЫВОД ОШИБКИ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ОДНОГО ФАЙЛА
void IvanCornelius_FilesChecksum::onErrorOccurred(const QString &path, const QString &error)
{
    QTextStream err(stderr);
    err << "Error processing " << path << ": " << error << "\n";
}


// СЛОТ ПОЛУЧЕНИЕ СООБЩЕНИЯ О ЗАВЕРШЕНИИ, СОЗДАНИЕ НОВОЙ ЗАДАЧИ ИЛИ ВЫВОД ИТОГОВОГО РЕЗУЛЬТАТА
void IvanCornelius_FilesChecksum::onWorkerFinished()
{
    --m_activeThreads;
    if (m_currentFileIndex < m_filesToProcess.size())
    {
        processNextFile();
    }
    else if (m_activeThreads == 0)
    {
        printResults();
        emit finished();
    }
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


// СОЗДАНИЯ НОВОЙ ЗАДАЧИ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ОДНОГО ФАЙЛА
void IvanCornelius_FilesChecksum::processNextFile()
{
    if (m_currentFileIndex >= m_filesToProcess.size())
    {
        return;
    }

    QString filePath = m_filesToProcess.at(m_currentFileIndex);
    ++m_currentFileIndex;
    ++m_activeThreads;

    QThread *thread = new QThread();
    m_threads.append(thread);

    IvanCornelius_OneFileChecksum *worker = new IvanCornelius_OneFileChecksum(filePath);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &IvanCornelius_OneFileChecksum::process);
    connect(worker, &IvanCornelius_OneFileChecksum::resultReady, this, &IvanCornelius_FilesChecksum::onResultReady);
    connect(worker, &IvanCornelius_OneFileChecksum::errorOccurred, this, &IvanCornelius_FilesChecksum::onErrorOccurred);
    connect(worker, &IvanCornelius_OneFileChecksum::finished, this, &IvanCornelius_FilesChecksum::onWorkerFinished);
    connect(worker, &IvanCornelius_OneFileChecksum::finished, worker, &QObject::deleteLater);
    connect(worker, &IvanCornelius_OneFileChecksum::finished, thread, &QThread::quit);

    thread->start();
}


// ВЫВОД ИТОГОВОГО РЕЗУЛЬТАТА РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ДЛЯ ВСЕХ ФАЙЛОВ
void IvanCornelius_FilesChecksum::printResults()
{
    QTextStream out(stdout);

    for (auto it = m_results.constBegin(); it != m_results.constEnd(); ++it)
    {
        out << it.key()  << "  " << it.value() << "\n";
    }
}

