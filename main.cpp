#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

#include <omp.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    switch (argc) {
    case 1:
    {
        MainWindow w;
        w.show();
        return a.exec();
        break;
    }
    case 2:
    {
        MainWindow w;
        w.open_file(QString(argv[1]));
        w.show();
        return a.exec();
    }
    case 3:
    {
        if (QString(argv[1]) == "--as") {
            WCI_Generater w;
            QFileInfo _info(QString::fromAscii(argv[2]));
            if (_info.suffix() == "wcd") {
                w.open_file(_info.absoluteFilePath());
                w.draw_WCAS(0, 0);
            }
        }
        break;
    }
    case 4:
    case 6:
    {
        if ( QString(argv[1]) == "--as" && QString(argv[2]) == "--D") {
            QString out_dir(".");
            if (argc == 6 && QString(argv[4]) == "--O")
                out_dir = QString(argv[5]);
            QDir dir;//(  );
            if (dir.cd( QString(argv[3] )))
            {
                QStringList list;
                list << "*.wcd";
                QFileInfoList d_wcdfiles = dir.entryInfoList(list, QDir::Files);
    //            #pragma omp parallel for num_threads(2)
                for (int i = 0; i < d_wcdfiles.size(); i++)
                {
                    WCI_Generater* w = new WCI_Generater;
                    w->open_file(d_wcdfiles[i].absoluteFilePath());
                    w->draw_WCAS(0, 0, out_dir);
    //                printf( QObject::tr("%1:The parallel region is executed by thread %2\n").arg(i).arg(omp_get_thread_num()).toStdString().c_str() );
                    delete w;
                }
            }
        }
    }
    default:
        break;
    }
    return 0;
}
