#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "em_parser.h"
#include "wc_ping_viewer.h"

#include <QLabel>
#include <QComboBox>
#include <QFileDialog>
#include <QProcess>
#include <QDataStream>
#include <QMessageBox>
#include <QCheckBox>
#include <QAction>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <fstream>

QImage Mat2QImage(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cv::cvtColor(src, temp, CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}

cv::Mat QImage2Mat(QImage const& src)
{
     cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
     cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
     cv::cvtColor(tmp, result,CV_BGR2RGB);
     return result;
}

WC_Ping_Label::WC_Ping_Label( const QString & text, QWidget * parent, Qt::WindowFlags f ) :
    QLabel(text, parent, f), d_cur_id(-1)
{
    d_action_output = new QAction(tr("Output"), this);
    addAction(d_action_output);
    QAction* d_action_ping_data = new QAction(tr("Ping Data"), this);
    addAction(d_action_ping_data);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(d_action_output, SIGNAL(triggered()), this, SLOT(output()));
    connect(d_action_ping_data, SIGNAL(triggered()), this, SLOT(outputPingData()));
}

bool WC_Ping_Label::output()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("save wci"), tr("E:"), tr("Image files (*.jpg *.png)"));
    if (!filename.isEmpty()) {
        pixmap()->save(filename);
        QMessageBox::information(this, "", "OK");
        return true;
    }
    return false;
}


void WC_Ping_Label::outputPingData()
{
    emit out_ping_data(d_cur_id);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    d_em_parser(NULL),
    d_current_cm(-1),
    d_current_index(0),
    d_binterpolated(true)
{
    ui->setupUi(this);

//    ui->img_label->hide();

    QLabel* label = new QLabel(tr("ColorMap:"), this);
    QComboBox *colormapComboBox = new QComboBox(this);    
    QCheckBox *_checkbox = new QCheckBox(tr("Interpolate?"), this);
    ui->mainToolBar->addWidget(label);
    ui->mainToolBar->addWidget(colormapComboBox);
    ui->mainToolBar->addWidget(_checkbox);
    ui->actionInterpolate->setChecked(true);
    _checkbox->setChecked(true);

    QString cur_path = QDir::currentPath();

    QDir dir(cur_path);
    if (dir.cd("colormaps")) {
        QStringList list;
        list << "*.map";

        d_colormaps = dir.entryInfoList(list, QDir::Files);
        foreach (QFileInfo file, d_colormaps) {
            colormapComboBox->addItem(file.fileName());
        }

        if (!d_colormaps.empty()) d_current_cm = 0;
    }

    connect(colormapComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(change_colormap(int)));
    connect(_checkbox, SIGNAL( stateChanged(int) ),
            this, SLOT(on_actionInterpolate_triggered()));

    d_wc_ping_label = new WC_Ping_Label("what");
    connect(d_wc_ping_label, SIGNAL( out_ping_data(int) ),
            this, SLOT(output_pingdata(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
    if (d_em_parser) delete d_em_parser;
}

void MainWindow::open_file(QString file_name)
{
    if (d_em_parser) delete d_em_parser; //! clear memory
    d_em_parser = new EM_Parser(file_name.toStdString());
    d_em_parser->pre_parse();
//    d_em_parser->parse();
    ui->basicInfoTextEdit->setHtml(QString::fromStdString(d_em_parser->basicInfo_toHtml()));

    if (d_em_parser->find_type_num(EM_Water_Column_Datagram)) {
        d_wc_pak_id.clear();
        if (d_em_parser->pre_parse_wci_packet())
            d_wc_pak_id = QVector<int>::fromStdVector( d_em_parser->d_wc_pak_id );
    }
    on_actionPing_Viewer_triggered();

    NavData data = d_em_parser->get_navigation();
    ui->track_viewer->addTrackLine(data);
    printf("");
}

void MainWindow::on_actionOpen_triggered()
{
    QString exts = tr("Swath files (*.wcd);;All files (*.*)");
    QString fileName = QFileDialog::getOpenFileName( this,
                               tr("Open all"), "", exts);

    if (!fileName.isEmpty()) {
        open_file(fileName);
        QFileInfo _info(fileName);
        setWindowTitle(tr("%1 - WC Viewer").arg(_info.fileName()));
    }

}

void MainWindow::change_colormap(int index)
{
    d_current_cm = index;
//    printf("%s\n", d_colormaps[index].absoluteFilePath().toStdString().c_str());
//    draw_WCO(d_em_parser);
}

void MainWindow::out_svp(EM_Parser *_parser)
{
    std::ofstream _out("svp.txt");
    _out << _parser->d_ssp.ssp_header.date << "\n";
    _out << _parser->d_ssp.ssp_header.millsecond << "\n";
    _out << _parser->d_ssp.ssp_info.date << "\n";
    _out << _parser->d_ssp.ssp_info.time << "\n";
    for (size_t i = 0; i < _parser->d_ssp.ssp_data.size(); i++ ) {
        em_ssp_entry &e = _parser->d_ssp.ssp_data[i];
        _out << e.depth / 100. << "\t" << e.sound_velocity / 10. << "\n";
    }
    _out.close();

    QStringList strlist;
    strlist << "svp.txt";
    QProcess::startDetached("notepad.exe", strlist);
}

cv::Mat MainWindow::draw_WCO2(size_t index, bool interpolate)
{
    EM_Parser* parser = d_em_parser;

    std::vector<RGB> color_map;
    if (d_current_cm >= 0)
        readColorMap(d_colormaps[d_current_cm].absoluteFilePath().toStdString(), color_map);
    else
        readColorMap("colormaps/GREY.MAP", color_map);

    em_wci_pak _pak = parser->get_full_wci(index);

    std::vector<em_wcb> &_wcbs = _pak.wcbs;
    float _min = 0, _max = -256;
    for (size_t ii = 0; ii < _wcbs.size(); ii++ ) {
        em_wcb _wcbe = _wcbs[ii];
        for (size_t j = 0; j < _wcbe.amps.size(); j++)
        {
            _min = _wcbe.amps[j] <= _min ? _wcbe.amps[j] : _min;
            _max = _wcbe.amps[j] >= _max ? _wcbe.amps[j] : _max;
        }
    }

    std::vector<float> _zs;
    QVector<float> _ys;
    for (size_t ii = 0; ii < _wcbs.size(); ii++ ) {
        em_wcb _wcbe = _wcbs[ii];
        size_t j = _wcbe.amps.size() - 1;
        float _range = 5. * _pak.wc.SS * j / _pak.wc.SF;
        float _z = _range * std::cos(_wcbe.wcb.beam_angle * 0.01 / 180 * 3.141592654);
        float _y = _range * std::sin(_wcbe.wcb.beam_angle * 0.01 / 180 * 3.141592654);

        _zs.push_back(_z);
        _ys.push_back(_y);
    }

    float img_scale = _zs[_zs.size() / 2] / _wcbs[_zs.size() / 2].amps.size(); //! 1 pt means 1m * 1m
    img_scale *= 1.;
    int img_width = std::max(std::abs(_ys[_ys.size() - 1]), std::abs(_ys[0])) * 2 / img_scale;
    int img_height = _zs[_zs.size() / 2] / img_scale;
    //! return value
    cv::Mat image(img_height, img_width, CV_8UC3);
    cv::Mat_<cv::Vec3b> im2 = image;

    //! white or black
    for (int ii = 0; ii < img_height; ii++)
        for (int jj = 0; jj < img_width; jj++) {
            im2(ii, jj)[0] = 255;
            im2(ii, jj)[1] = 255;
            im2(ii, jj)[2] = 255;
        }

    for (size_t ii = 0; ii < _wcbs.size(); ii++ ) {
        em_wcb _wcbe = _wcbs[ii];
        for (size_t j = 0; j < _wcbe.amps.size(); j++) {
            // if (_wcbe.wcb.DR == j)  std::cout << _wcbe.wcb.DR << "\n";
            float _range = 5. * _pak.wc.SS * j / _pak.wc.SF;
            float _z = _range * std::cos(_wcbe.wcb.beam_angle * 0.01 / 180 * 3.141592654);
            float _y = _range * std::sin(_wcbe.wcb.beam_angle * 0.01 / 180 * 3.141592654);

            int _c = (int)(_y / img_scale + img_width / 2 + 0.5);
            int _r = (int)(_z / img_scale + .5);

            if (_r < 0 || _r > img_height - 1) continue;
            if (_c > img_width - 1 || _c < 0) continue;
            int _id = (_wcbe.amps[j] - _min) / (_max- _min) * color_map.size();
            im2(_r, _c)[0] = color_map[_id].r;
            im2(_r, _c)[1] = color_map[_id].g;
            im2(_r, _c)[2] = color_map[_id].b;
        }
    }

    if (interpolate) {
        //! interpolate_wc_across
        float _start_a = _wcbs[_wcbs.size() - 1].wcb.beam_angle * 0.01 / 180 * 3.141592654;
        float _end_a = _wcbs[0].wcb.beam_angle * 0.01 / 180 * 3.141592654;
        for (int ii = 0; ii < img_height; ii++) {
            int _start_w = (int)(img_width / 2.0 + 0.5) + ii * std::tan(_start_a);
            int _end_w = (int)(img_width / 2.0 + 0.5) + ii * std::tan(_end_a);
            if (_end_w > img_width - 1) _end_w = img_width - 1;
            if (_start_w < 0) _start_w = 0;
            QVector<int> exits_ps;
            QVector<cv::Vec3b> exits_dbs;
            QVector<int> not_exits_ps;
            for (int jj = _start_w; jj <= _end_w; jj++) {
                int sum_rgb = im2(ii, jj)[0] + im2(ii, jj)[1] + im2(ii, jj)[2];
                if (sum_rgb != 255 * 3) {
                    exits_ps.push_back(jj);
                    exits_dbs.push_back(cv::Vec3b(im2(ii, jj)[0], im2(ii, jj)[1], im2(ii, jj)[2]));
                }
                else
                    not_exits_ps.push_back(jj);
            }
            if (exits_ps.size() == _end_w - _start_w + 1) continue;
            if (exits_ps.empty()) continue;

            for (int k = 0; k < not_exits_ps.size(); k++) {
                int _before = -1, _after = -1;
                for (int kk = 0; kk < exits_ps.size() - 1; kk++)
                    if (not_exits_ps[k] > exits_ps[kk] && not_exits_ps[k] < exits_ps[kk + 1]) {
                        _before = kk;
                        _after = kk + 1;
                        break;
                    }
                if (_before < 0 || _after < 0) continue;

                int _space = exits_ps[_after] - exits_ps[_before];
                int _i = not_exits_ps[k] - exits_ps[_before];
                im2(ii, not_exits_ps[k])[0] = float(im2(ii, exits_ps[_after])[0] - im2(ii, exits_ps[_before])[0]) / _space * _i + im2(ii, exits_ps[_before])[0];
                im2(ii, not_exits_ps[k])[1] = float(im2(ii, exits_ps[_after])[1] - im2(ii, exits_ps[_before])[1]) / _space * _i + im2(ii, exits_ps[_before])[1];
                im2(ii, not_exits_ps[k])[2] = float(im2(ii, exits_ps[_after])[2] - im2(ii, exits_ps[_before])[2]) / _space * _i + im2(ii, exits_ps[_before])[2];
            }
        }
    }

    return image;
}

void MainWindow::draw_WCO(EM_Parser* parser, bool interpolate, size_t index)
{
    std::vector<RGB> color_map;
    if (d_current_cm >= 0)
        readColorMap(d_colormaps[d_current_cm].absoluteFilePath().toStdString(), color_map);
    else
        readColorMap("colormaps/GREY.MAP", color_map);

    em_wci_pak _pak = parser->get_full_wci(index);
//    em_wci_pak _pak;
//    for (size_t i = 0; i <  parser->d_wci_packets.size(); i++) {

//        _pak = parser->d_wci_packets[i];

//        if (_pak.wc.Nd > 1) {
//            for (u16 j = 1; j < _pak.wc.Nd; j++) {
//                em_wci_pak &_pakj = parser->d_wci_packets[i + j];
//                for (size_t ii = 0; ii < _pakj.wcbs.size(); ii++)
//                    _pak.wcbs.push_back(_pakj.wcbs[ii]);
//            }
//        }
//        if (i == index) break;
//        i += _pak.wc.Nd - 1;
//    }

    std::vector<em_wcb> &_wcbs = _pak.wcbs;
    float _min = 0, _max = -256;
    for (size_t ii = 0; ii < _wcbs.size(); ii++ ) {
        em_wcb _wcbe = _wcbs[ii];
        for (size_t j = 0; j < _wcbe.amps.size(); j++)
        {
            _min = _wcbe.amps[j] <= _min ? _wcbe.amps[j] : _min;
            _max = _wcbe.amps[j] >= _max ? _wcbe.amps[j] : _max;
        }
    }

    std::vector<float> _zs;
    QVector<float> _ys;
    for (size_t ii = 0; ii < _wcbs.size(); ii++ ) {
        em_wcb _wcbe = _wcbs[ii];
        size_t j = _wcbe.amps.size() - 1;
        float _range = 5. * _pak.wc.SS * j / _pak.wc.SF;
        float _z = _range * std::cos(_wcbe.wcb.beam_angle * 0.01 / 180 * 3.141592654);
        float _y = _range * std::sin(_wcbe.wcb.beam_angle * 0.01 / 180 * 3.141592654);

        _zs.push_back(_z);
        _ys.push_back(_y);
    }

    float img_scale = _zs[_zs.size() / 2] / _wcbs[_zs.size() / 2].amps.size(); //! 1 pt means 1m * 1m
    img_scale *= 1.;
    int img_width = std::max(std::abs(_ys[_ys.size() - 1]), std::abs(_ys[0])) * 2 / img_scale;
//    printf("%d, %lf %lf", img_width, _ys[_ys.size() - 1], _ys[0]);
    int img_height = _zs[_zs.size() / 2] / img_scale;
    cv::Mat image(img_height, img_width, CV_8UC3);
    cv::Mat_<cv::Vec3b> im2 = image;

    //! white or black
    for (int ii = 0; ii < img_height; ii++)
        for (int jj = 0; jj < img_width; jj++) {
            im2(ii, jj)[0] = 255;
            im2(ii, jj)[1] = 255;
            im2(ii, jj)[2] = 255;
        }

    for (size_t ii = 0; ii < _wcbs.size(); ii++ ) {
        em_wcb _wcbe = _wcbs[ii];
        for (size_t j = 0; j < _wcbe.amps.size(); j++) {
            //                        if (_wcbe.wcb.DR == j)  std::cout << _wcbe.wcb.DR << "\n";
            float _range = 5. * _pak.wc.SS * j / _pak.wc.SF;
            float _z = _range * std::cos(_wcbe.wcb.beam_angle * 0.01 / 180 * 3.141592654);
            float _y = _range * std::sin(_wcbe.wcb.beam_angle * 0.01 / 180 * 3.141592654);

            int _c = (int)(_y / img_scale + img_width / 2 + 0.5);
            int _r = (int)(_z / img_scale + .5);

            if (_r < 0 || _r > img_height - 1) continue;
            if (_c > img_width - 1 || _c < 0) continue;
            int _id = (_wcbe.amps[j] - _min) / (_max- _min) * color_map.size();
            im2(_r, _c)[0] = color_map[_id].r;
            im2(_r, _c)[1] = color_map[_id].g;
            im2(_r, _c)[2] = color_map[_id].b;
        }
    }

    if (interpolate) {
        //! interpolate_wc_across
        float _start_a = _wcbs[_wcbs.size() - 1].wcb.beam_angle * 0.01 / 180 * 3.141592654;
        float _end_a = _wcbs[0].wcb.beam_angle * 0.01 / 180 * 3.141592654;
        for (int ii = 0; ii < img_height; ii++) {
            int _start_w = (int)(img_width / 2.0 + 0.5) + ii * std::tan(_start_a);
            int _end_w = (int)(img_width / 2.0 + 0.5) + ii * std::tan(_end_a);
            if (_end_w > img_width - 1) _start_w = img_width - 1;
            if (_start_w < 0) _start_w = 0;
            QVector<int> exits_ps;
            QVector<cv::Vec3b> exits_dbs;
            QVector<int> not_exits_ps;
            for (int jj = _start_w; jj <= _end_w; jj++) {
                int sum_rgb = im2(ii, jj)[0] + im2(ii, jj)[1] + im2(ii, jj)[2];
                if (sum_rgb != 255 * 3) {
                    exits_ps.push_back(jj);
                    exits_dbs.push_back(cv::Vec3b(im2(ii, jj)[0], im2(ii, jj)[1], im2(ii, jj)[2]));
                }
                else
                    not_exits_ps.push_back(jj);
            }
            if (exits_ps.size() == _end_w - _start_w + 1) continue;
            if (exits_ps.empty()) continue;
            //        if (!exits_ps.empty() && !not_exits_ps.empty()) {
            //            if (exits_ps[0] > not_exits_ps[0]
            //                    || exits_ps[exits_ps.size() - 1] < not_exits_ps[not_exits_ps.size() - 1]) {
            //                printf("sth wrong!");
            ////                return;
            //            }
            //        }
            for (int k = 0; k < not_exits_ps.size(); k++) {
                int _before = -1, _after = -1;
                for (int kk = 0; kk < exits_ps.size() - 1; kk++)
                    if (not_exits_ps[k] > exits_ps[kk] && not_exits_ps[k] < exits_ps[kk + 1]) {
                        _before = kk;
                        _after = kk + 1;
                        break;
                    }
                if (_before < 0 || _after < 0) continue;

                int _space = exits_ps[_after] - exits_ps[_before];
                int _i = not_exits_ps[k] - exits_ps[_before];
                im2(ii, not_exits_ps[k])[0] = float(im2(ii, exits_ps[_after])[0] - im2(ii, exits_ps[_before])[0]) / _space * _i + im2(ii, exits_ps[_before])[0];
                im2(ii, not_exits_ps[k])[1] = float(im2(ii, exits_ps[_after])[1] - im2(ii, exits_ps[_before])[1]) / _space * _i + im2(ii, exits_ps[_before])[1];
                im2(ii, not_exits_ps[k])[2] = float(im2(ii, exits_ps[_after])[2] - im2(ii, exits_ps[_before])[2]) / _space * _i + im2(ii, exits_ps[_before])[2];
            }
        }
    }

    // cv::namedWindow("wci Image");
    // cv::imshow("wci Image",image);
    cv::imwrite("wco.jpg", image);

    ui->basicInfoTextEdit->hide();
//    ui->img_label->show();
//    ui->img_label->setPixmap(QPixmap::fromImage(Mat2QImage(image)));

}

void MainWindow::draw_WCAS(EM_Parser *parser, bool popout)
{
    if (!parser) parser = d_em_parser;
    if (!parser) return;

    std::vector<RGB> color_map;
    if (d_current_cm >= 0)
        readColorMap(d_colormaps[d_current_cm].absoluteFilePath().toStdString(), color_map);
    else
        readColorMap("colormaps/GREY.MAP", color_map);

    QVector<em_wcb> _al_bs;
    QVector<em_wc_datagram> _al_wcs;

    for (int i = 0; i < d_wc_pak_id.size(); i++) {
        em_wcb beam;
        em_wc_datagram info;
        d_em_parser->get_wci_center_beam(i, beam, info);
        _al_bs.push_back(beam);
        _al_wcs.push_back(info);
    }

//    for (size_t i = 0; i <  parser->d_wci_packets.size(); i++) {
//        em_wci_pak _pak = parser->d_wci_packets[i];
//        std::vector<em_wcb> &_wcbs = _pak.wcbs;

//        if (_pak.wc.Nd > 1) {
//            for (u16 j = 1; j < _pak.wc.Nd; j++) {
//                em_wci_pak &_pakj = parser->d_wci_packets[i + j];
//                for (size_t ii = 0; ii < _pakj.wcbs.size(); ii++)
//                    _pak.wcbs.push_back(_pakj.wcbs[ii]);
//            }
//        }

//        for (size_t ii = 0; ii < _wcbs.size(); ii++ ) {
//            em_wcb _wcbe = _wcbs[ii];
//            if (ii == int(_wcbs.size() / 2. + 0.5)) {
//                _al_bs.push_back(_wcbe);
//                _al_wcs.push_back(_pak.wc);
//                break;
//            }
//        }
//        i += _pak.wc.Nd - 1;
//    }

    float _min = 0, _max = -256, _depth_max = 0, _max_img_scale = 0;
    int _sample_n = -1;
    for (int ii = 0; ii < _al_bs.size(); ii++ ) {
        em_wcb _wcbe = _al_bs[ii];
        for (size_t j = 0; j < _wcbe.amps.size(); j++)
        {
            _min = _wcbe.amps[j] <= _min ? _wcbe.amps[j] : _min;
            _max = _wcbe.amps[j] >= _max ? _wcbe.amps[j] : _max;
        }
        em_wc_datagram _wc = _al_wcs[ii];
        double _r = 5. * _wc.SS * _wcbe.amps.size() / _wc.SF - _wc.TX_time_heave / 100.;
        if (_depth_max <= _r ) {
            _depth_max = _r;
            _sample_n = _wcbe.amps.size();
        }
        _max_img_scale = _max_img_scale >= (_r /  _wcbe.amps.size())
                ? _max_img_scale : (_r /  _wcbe.amps.size());
    }

    float img_scale = _max_img_scale;//_depth_max / _sample_n;
//    printf("max img scale %f defaut %f\n", _max_img_scale, _depth_max / _sample_n);
    //            double img_scale = 2; //! 1 pt means 1m * 1m
    int img_width = _al_bs.size();   //3000 / img_scale;
    int img_height = _depth_max / img_scale ;//1500 / img_scale;
    cv::Mat image(img_height, img_width, CV_8UC3);
    cv::Mat_<cv::Vec3b> im2 = image;

    /*! make it white */
    for (int ii = 0; ii < img_height; ii++)
        for (int jj = 0; jj < img_width; jj++) {
            im2(ii, jj)[0] = 255;
            im2(ii, jj)[1] = 255;
            im2(ii, jj)[2] = 255;
        }

    for (int ii = 0; ii < _al_bs.size(); ii++ ) {
        const em_wcb& _wcbe = _al_bs[ii];
        const em_wc_datagram& _wc = _al_wcs[ii];
        for (size_t j = 0; j < _wcbe.amps.size(); j++) {
            float _range = 5. * _wc.SS * j / _wc.SF - _wc.TX_time_heave / 100.;

            int _c = ii;
            int _r = (int)(_range / img_scale + .5);

            if (_r < 0 || _r > img_height - 1) continue;
            int _id = (_wcbe.amps[j] - _min) / (_max- _min) * color_map.size();
            im2(_r, _c)[0] = color_map[_id].r;
            im2(_r, _c)[1] = color_map[_id].g;
            im2(_r, _c)[2] = color_map[_id].b;
        }
    }

    if (popout) {
    cv::namedWindow("wci Image");
    cv::imshow("wci Image",image);
    cv::imwrite("wci Image.jpg", image);
    }
    else {
        QFileInfo info(QString::fromStdString(d_em_parser->d_file_path));
        cv::imwrite(tr("wcas_%1.jpg").arg(info.baseName()).toStdString().c_str(), image);
    }
    _al_bs.clear();
    _al_wcs.clear();
}

void MainWindow::on_actionWCO_triggered()
{
    if (d_em_parser) draw_WCO(d_em_parser, d_binterpolated, d_wc_pak_id[d_current_index]);
}

void MainWindow::on_actionWC_AS_triggered()
{
    if (d_em_parser) draw_WCAS(d_em_parser);
}

void MainWindow::on_actionSVP_triggered()
{
    if (d_em_parser) out_svp(d_em_parser);
}

void MainWindow::on_actionNext_triggered()
{
    d_current_index += 1;
    if (d_current_index < d_wc_pak_id.size()) {
        draw_WCO(d_em_parser, d_binterpolated, d_current_index);
    }
    else
        QMessageBox::information(this, "", "no next");
}

void MainWindow::on_actionInterpolate_triggered()
{
    d_binterpolated = !d_binterpolated;
    ui->actionInterpolate->setChecked(d_binterpolated);
    printf("%d", ui->actionInterpolate->isChecked());
}


/** */
void WCI_Generater::open_file(QString file_name)
{
    if (d_em_parser) delete d_em_parser; //! clear memory
    d_em_parser = new EM_Parser(file_name.toStdString());
    d_em_parser->pre_parse();

    if (d_em_parser->find_type_num(EM_Water_Column_Datagram)) {
        d_wc_pak_id.clear();
        if (d_em_parser->pre_parse_wci_packet())
            d_wc_pak_id = QVector<int>::fromStdVector( d_em_parser->d_wc_pak_id );
    }
}

void WCI_Generater::draw_WCAS(EM_Parser *parser, bool popout, QString out_dir)
{
    if (!parser) parser = d_em_parser;
    if (!parser) return;

    std::vector<RGB> color_map;
    if (d_current_cm >= 0)
        readColorMap(d_colormaps[d_current_cm].absoluteFilePath().toStdString(), color_map);
    else
        readColorMap("colormaps/3D.MAP", color_map);

    QVector<em_wcb> _al_bs;
    QVector<em_wc_datagram> _al_wcs;

    for (int i = 0; i < d_wc_pak_id.size(); i++) {
        em_wcb beam;
        em_wc_datagram info;
        d_em_parser->get_wci_center_beam(i, beam, info);
        _al_bs.push_back(beam);
        _al_wcs.push_back(info);
    }

    float _min = 0, _max = -256, _depth_max = 0, _max_img_scale = 0;
    int _sample_n = -1;
    for (int ii = 0; ii < _al_bs.size(); ii++ ) {
        em_wcb _wcbe = _al_bs[ii];
        for (size_t j = 0; j < _wcbe.amps.size(); j++)
        {
            _min = _wcbe.amps[j] <= _min ? _wcbe.amps[j] : _min;
            _max = _wcbe.amps[j] >= _max ? _wcbe.amps[j] : _max;
        }
        em_wc_datagram _wc = _al_wcs[ii];
        double _r = 5. * _wc.SS * _wcbe.amps.size() / _wc.SF - _wc.TX_time_heave / 100.;
        if (_depth_max <= _r ) {
            _depth_max = _r;
            _sample_n = _wcbe.amps.size();
        }
        _max_img_scale = _max_img_scale >= (_r /  _wcbe.amps.size())
                ? _max_img_scale : (_r /  _wcbe.amps.size());
    }

    float img_scale = _max_img_scale;//_depth_max / _sample_n;
//    printf("max img scale %f defaut %f\n", _max_img_scale, _depth_max / _sample_n);
    //            double img_scale = 2; //! 1 pt means 1m * 1m
    int img_width = _al_bs.size();   //3000 / img_scale;
    int img_height = _depth_max / img_scale ;//1500 / img_scale;
    cv::Mat image(img_height, img_width, CV_8UC3);
    cv::Mat_<cv::Vec3b> im2 = image;

    /*! make it white */
    for (int ii = 0; ii < img_height; ii++)
        for (int jj = 0; jj < img_width; jj++) {
            im2(ii, jj)[0] = 255;
            im2(ii, jj)[1] = 255;
            im2(ii, jj)[2] = 255;
        }

    for (int ii = 0; ii < _al_bs.size(); ii++ ) {
        em_wcb _wcbe = _al_bs[ii];
        em_wc_datagram _wc = _al_wcs[ii];
        for (size_t j = 0; j < _wcbe.amps.size(); j++) {
            float _range = 5. * _wc.SS * j / _wc.SF - _wc.TX_time_heave / 100.;

            int _c = ii;
            int _r = (int)(_range / img_scale + .5);

            if (_r < 0 || _r > img_height - 1) continue;
            int _id = (_wcbe.amps[j] - _min) / (_max- _min) * color_map.size();
            im2(_r, _c)[0] = color_map[_id].r;
            im2(_r, _c)[1] = color_map[_id].g;
            im2(_r, _c)[2] = color_map[_id].b;
        }
    }

    if (popout) {
        cv::namedWindow("wci Image");
        cv::imshow("wci Image",image);
        cv::imwrite("wci Image.jpg", image);
    }
    else {
        QFileInfo info(QString::fromStdString(d_em_parser->d_file_path));
        cv::imwrite(QObject::tr("%1/%2_wcas.jpg").arg(out_dir).arg(info.baseName())
                    .toStdString().c_str(), image);
    }

    _al_bs.clear();
    _al_wcs.clear();
}

void MainWindow::on_actionPing_Viewer_triggered()
{
//    QDialog dlg;
//    QHBoxLayout *_l = new QHBoxLayout(&dlg);
    WC_Ping_Viewer* _v = ui->ping_viewer; //new WC_Ping_Viewer;
    connect(_v, SIGNAL(cur_id_changed(int)), this, SLOT(update_wc_ping_label(int)));
    QHBoxLayout *_l2 = new QHBoxLayout;
    _l2->addWidget(d_wc_ping_label);
    _v->set_main_widget_layout(_l2);
    _v->set_range(d_wc_pak_id.size());
    _v->next();
//    dlg.exec();
}

void MainWindow::update_wc_ping_label(int id)
{
//    d_wc_ping_label->size();
    d_wc_ping_label->set_cur_id(id);
    d_wc_ping_label->setPixmap(
                QPixmap::fromImage(Mat2QImage(draw_WCO2(id, d_binterpolated)))
                .scaled(d_wc_ping_label->size(), Qt::KeepAspectRatio));
}

void MainWindow::output_pingdata(int index)
{
    em_wci_pak _pak = d_em_parser->get_full_wci(index);

    size_t _beam_num = _pak.wcbs.size();

    std::ofstream _out1("angles.txt");
    std::ofstream _out2(tr("samples_%1_%2.txt")
                        .arg(_pak.wc.SS / 10.)
                        .arg(_pak.wc.SF / 100.).toAscii()
                        );
    std::ofstream _out3("bottoms.txt");

    //! output angles samples
    for (size_t i = 0; i < _beam_num; i++) {
        em_wcb &_b = _pak.wcbs[i];
        _out1 << _b.wcb.beam_angle * .01 << "\t";
        for (size_t j = 0; j < _b.amps.size(); j++)
            _out2 << _b.amps[j] * .5 << "\t";
        _out2 << "\n";
        _out3 << _b.wcb.DR << "\t";
    }
    _out1.close();
    _out2.close();
}
