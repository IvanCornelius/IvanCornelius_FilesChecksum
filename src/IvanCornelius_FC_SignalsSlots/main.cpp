// ПРОГРАММА ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ФАЙЛОВ В КАТАЛОГЕ
// ДОП. ДАННЫЕ: 20.01.2025; v1.0.0_SignalsSlots.
#include <QCoreApplication>
#include <QTextStream>
#include <QDir>
#include "IvanCornelius_FilesChecksum.hpp"


/** ПРОГРАММА ДЛЯ РАСЧЁТА КОНТРОЛЬНОЙ СУММЫ ФАЙЛОВ В КАТАЛОГЕ.
 *  При запуске приложения указите стартовый каталог
 *  через параметр командной строки. */
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTextStream err(stderr);

    // Для тестирования программы
//    argc = 2;
//    const char* newArgv[] = {
//            argv[0],
//            "C:/Users/Vanya/Programming/GitHub/IvanCornelius_SimpleChatClient",
//            nullptr
//        };
//    argv = const_cast<char**>(newArgv);


    // Проверка и подготовка аргументов команды
    if (argc != 2)
    {
        err << "Error: The start directory is not specified. "
               "Specify the start directory is set via the "
               "parameter of the application launch command.\n";
        return 1;
    }
    QString dirPath = QString::fromLocal8Bit(argv[1]);
    QDir dir(dirPath);
    if (!dir.exists())
    {
        err << "Error: Directory does not exist: " << dirPath << "\n";
        return 1;
    }
    dirPath = dir.absolutePath();

    // Расчёт
    IvanCornelius_FilesChecksum calculator(dirPath);
    QObject::connect(&calculator, &IvanCornelius_FilesChecksum::finished,
                     &app, &QCoreApplication::quit);
    calculator.start();
    return app.exec();
}
