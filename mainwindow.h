#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfoList>
#include <QLabel>

#include <opencv2/core/core.hpp>

namespace Ui {
class MainWindow;
}

class QContextMenuEvent;
class EM_Parser;

class WC_Ping_Label : public QLabel
{
    Q_OBJECT
public:
    explicit WC_Ping_Label ( const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0 );

    void set_cur_id(int id) {d_cur_id = id;}
public slots:
    bool output();
    void outputPingData();

signals:
    void out_ping_data(int id);

private:
    QAction* d_action_output;
    int d_cur_id;
};



class WCI_Generater
{
public:
    WCI_Generater() : d_em_parser(NULL), d_current_cm(-1) {}
    void open_file(QString file_name);
    void draw_WCAS(EM_Parser *parser, bool popout = true, QString out_dir = QString("."));

    EM_Parser* d_em_parser;
    QFileInfoList d_colormaps;
    int d_current_cm;
    QVector<int> d_wc_pak_id;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void open_file(QString file_name);

    void draw_WCAS(EM_Parser *parser, bool popout = true);
public slots:
    void output_pingdata(int index = 0);
private slots:
    void change_colormap(int index);
    void update_wc_ping_label(int id);
    void on_actionOpen_triggered();

    void on_actionWCO_triggered();

    void on_actionWC_AS_triggered();

    void on_actionSVP_triggered();

    void on_actionNext_triggered();

    void on_actionInterpolate_triggered();

    void on_actionPing_Viewer_triggered();

private:
    Ui::MainWindow *ui;
    EM_Parser* d_em_parser;
    int d_current_cm;
    QFileInfoList d_colormaps;
    int d_current_index;
    bool d_binterpolated;
    QVector<int> d_wc_pak_id;
    WC_Ping_Label* d_wc_ping_label;

    void draw_WCO(EM_Parser *parser, bool interpolate = true, size_t index = 0);
    cv::Mat draw_WCO2(size_t index = 0, bool interpolate = true);
    void out_svp(EM_Parser *parser);

    void interpolate_wc_beam();
    void interpolate_wc_across();
};


struct RGB {
    int r;
    int g;
    int b;
};

inline bool readColorMap(std::string mapF, std::vector<RGB>& color_map)
{
    FILE* file;
    char buf[BUFSIZ * 16];

    if ((file = fopen(mapF.data(), "r")) == NULL) {
        fprintf(stderr, "readColorMap: error in opening data file \"%s\"\n", mapF.data());
        return false;
    }

    setvbuf( file, buf, _IOFBF, sizeof( buf ) );

    color_map.clear();

    while ( !feof(file) ) {
        RGB _rgb;
        fscanf(file, "%d%d%d\n", &_rgb.r, &_rgb.g, &_rgb.b);
        color_map.push_back(_rgb );
    }

    fclose(file);
    return true;
}

#endif // MAINWINDOW_H
