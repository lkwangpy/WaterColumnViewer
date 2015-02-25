#include "wc_ping_viewer.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QSlider>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>

WC_Ping_Viewer::WC_Ping_Viewer(QWidget *parent) :
    QWidget(parent),
    d_isRunning(false),
    d_step(1),
    d_cur_index(1)
{
    setup_ui();

    connect( &d_timer, SIGNAL(timeout()), this, SLOT(update_viewer()) );
}

WC_Ping_Viewer::~WC_Ping_Viewer()
{
}

void WC_Ping_Viewer::set_range(int r)
{
    //! beginning at 1
    d_max_index = r;
    d_pingInfoHorizontalSlider->setRange(1, r);
}

void WC_Ping_Viewer::set_main_widget_layout(QLayout *l)
{
    d_main_widget->setLayout(l);
}

void WC_Ping_Viewer::setup_ui()
{
    d_main_widget = new QWidget();

    d_backwardToolButton = new QToolButton(this);
    d_backwardToolButton->setObjectName(QString::fromUtf8("backwardToolButton"));
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/images/media_seek_backward.png"), QSize(), QIcon::Normal, QIcon::Off);
    d_backwardToolButton->setIcon(icon2);
    d_backwardToolButton->setIconSize(QSize(24, 24));

    d_startOrPauseToolButton = new QToolButton(this);
    d_startOrPauseToolButton->setObjectName(QString::fromUtf8("startOrPauseToolButton"));
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/images/media_playback_start.png"), QSize(), QIcon::Normal, QIcon::Off);
    d_startOrPauseToolButton->setIcon(icon3);
    d_startOrPauseToolButton->setIconSize(QSize(32, 32));

    d_forwardToolButton = new QToolButton(this);
    d_forwardToolButton->setObjectName(QString::fromUtf8("forwardToolButton"));
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/images/media_seek_forward.png"), QSize(), QIcon::Normal, QIcon::Off);
    d_forwardToolButton->setIcon(icon4);
    d_forwardToolButton->setIconSize(QSize(24, 24));

    d_pingInfoHorizontalSlider = new QSlider(Qt::Horizontal, this);
    d_pingInfoHorizontalSlider->setObjectName(QString::fromUtf8("pingInfoHorizontalSlider"));

    QLabel* _ping_id_label = new QLabel(tr("ID:"));
    QLabel* _ping_id_lineedit = new QLabel;
    QHBoxLayout* _h_l;
    _h_l = new QHBoxLayout();
    _h_l->addWidget(_ping_id_label);
    _h_l->addWidget(_ping_id_lineedit);
    connect(this, SIGNAL(cur_id_changed(int)), _ping_id_lineedit, SLOT(setNum(int)));

    QGridLayout *_func_tool_layout;
    _func_tool_layout = new QGridLayout();
    _func_tool_layout->addLayout(_h_l, 0, 0, 2, 1, Qt::AlignTop);
    _func_tool_layout->addWidget(d_startOrPauseToolButton, 0, 2, 1, 1, Qt::AlignTop);
    _func_tool_layout->addWidget(d_backwardToolButton, 0, 3, 1, 1, Qt::AlignTop);
    _func_tool_layout->addWidget(d_forwardToolButton, 0, 4, 1, 1, Qt::AlignTop);
    _func_tool_layout->addWidget(d_pingInfoHorizontalSlider, 0, 5, 4, 1, Qt::AlignTop);
    connect(d_pingInfoHorizontalSlider, SIGNAL(sliderMoved(int)), this, SLOT(ping_id_changed(int)));

    //! overall
    QVBoxLayout *d_horizontalLayout;
    d_horizontalLayout = new QVBoxLayout();
    d_horizontalLayout->addLayout(_func_tool_layout);
    d_horizontalLayout->addWidget(d_main_widget);

    setLayout(d_horizontalLayout);

    QMetaObject::connectSlotsByName(this);
}

void WC_Ping_Viewer::update_viewer()
{
    if (d_isRunning) {
        if (d_cur_index > d_max_index) {
            d_isRunning = false;
            return;
        }
        next();
    }
}

void WC_Ping_Viewer::next()
{
    d_pingInfoHorizontalSlider->setValue(d_cur_index);
    emit cur_id_changed(d_cur_index - 1);
    d_cur_index += d_step;
    //! out the profile data
//    output("profiles.txt");
}

void WC_Ping_Viewer::on_startOrPauseToolButton_clicked()
{
    QIcon icon;
    if (d_isRunning) {
        d_timer.stop();
        icon.addFile(QString::fromUtf8(":/images/media_playback_start.png"), QSize(), QIcon::Normal, QIcon::Off);
        d_startOrPauseToolButton->setIcon(icon);
        d_startOrPauseToolButton->setIconSize(QSize(32, 32));
    }
    else {
        d_timer.start(25);
        icon.addFile(QString::fromUtf8(":/images/media_playback_pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        d_startOrPauseToolButton->setIcon(icon);
        d_startOrPauseToolButton->setIconSize(QSize(32, 32));
    }
    d_isRunning = !d_isRunning;
}

void WC_Ping_Viewer::ping_id_changed(int id)
{
    d_cur_index = id;
    next();
}

void WC_Ping_Viewer::on_backwardToolButton_clicked()
{
    d_cur_index -= d_step * 2;
    next();
}

void WC_Ping_Viewer::on_forwardToolButton_clicked()
{
//    d_cur_index += d_step;
    next();
}
