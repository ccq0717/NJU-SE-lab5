#ifndef CONFIRMTOOL_H
#define CONFIRMTOOL_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ConfirmTool; }
QT_END_NAMESPACE

#include "node.h"
#include <vector>
using namespace std;

class ConfirmTool : public QDialog
{
    Q_OBJECT

private:
    vector<node*>program_pair;
    bool isStart;//是否开始确认了
    int ind;//当前显示的程序对的下标
    int before_ind;//用于在跳转到推荐的程序对时，保存先前的确认进度
    int sz;//程序对总数
    int recmd;//推荐的程序对在数组中的下标

public:
    ConfirmTool(QWidget *parent = nullptr);
    ~ConfirmTool();

    void showText();//显示程序文本

    void insertLine(char *text, bool color, int n);//控制某一行文本的输出和颜色

    void recommend();//根据目前显示程序对的确认结果，自动推荐下一个

    void res_output();//将确认结果输出到文件

private slots:
    void on_btn_start_clicked();

    void on_btn_equal_clicked();

    void on_btn_inequal_clicked();

    void on_btn_unsure_clicked();

    void on_btn_end_clicked();

    void on_btn_recommend_clicked();

private:
    Ui::ConfirmTool *ui;
};
#endif // CONFIRMTOOL_H
