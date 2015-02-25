#ifndef WC_PING_VIEWER_H
#define WC_PING_VIEWER_H

#include <QWidget>
#include <QTimer>

class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QToolButton;
class QSlider;
class QComboBox;

class WC_Ping_Viewer : public QWidget
{
    Q_OBJECT

public:
    explicit WC_Ping_Viewer(QWidget *parent = 0);
    ~WC_Ping_Viewer();

    void setup_ui();

    void set_range(int r);

    //! output the profile data
    void output(QString out_str);

    void set_main_widget_layout(QLayout* l);

signals:
    void cur_id_changed(int id);

public slots:
    void next();
    void update_viewer();
    void on_startOrPauseToolButton_clicked();
    void on_backwardToolButton_clicked();
    void on_forwardToolButton_clicked();
    void ping_id_changed(int id);

private:
    QWidget *d_main_widget;
    bool d_isRunning;
    int d_step;
    size_t d_cur_index;
    int d_max_index;

    QTimer d_timer;
    bool d_b_showComparison;
    bool d_b_showComparison_att;

    //! ui
    QToolButton *d_backwardToolButton;
    QToolButton *d_startOrPauseToolButton;
    QToolButton *d_forwardToolButton;
    QSlider     *d_pingInfoHorizontalSlider;
    QComboBox   *d_ping_shown_combobox;
};

#endif // WC_PING_VIEWER_H
