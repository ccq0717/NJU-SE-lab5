#include "confirmtool.h"
#include "ui_confirmtool.h"
#include <QMessageBox>
#include <iostream>
#include <string>
#include "node.h"
using namespace std;

string path_str = "D:/CodeProjects/Qt/SE_lab_5/";//文件绝对路径前缀

ConfirmTool::ConfirmTool(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConfirmTool)
{
    isStart = false;
    ind = 0;
    sz = 0;
    recmd = -1;
    before_ind = -1;
    //读入程序对
    string tmp = path_str + "input/equal.csv";
    FILE* f_equal = fopen(tmp.c_str(), "r");
    if (f_equal == NULL) {
        cerr << "equal.csv打开失败";
        exit(-1);
    }
    tmp = path_str + "input/inequal.csv";
    FILE* f_inequal = fopen(tmp.c_str(), "r");
    if (f_inequal == NULL) {
        cerr << "inequal.csv打开失败";
        exit(-1);
    }
    //读入程序对到数组中
    char file1_path[100] = { 0 }, file2_path[100] = { 0 };
    while (!feof(f_equal)) {
        fscanf(f_equal, "%[^,],%s\n", file1_path, file2_path);
        node* n = new node(file1_path, file2_path, true);
        program_pair.push_back(n);
        sz++;
    }
    while (!feof(f_inequal)) {
        fscanf(f_inequal, "%[^,],%s\n", file1_path, file2_path);
        node* n = new node(file1_path, file2_path, false);
        program_pair.push_back(n);
        sz++;
    }
    fclose(f_equal);
    fclose(f_inequal);

    ui->setupUi(this);
}

ConfirmTool::~ConfirmTool()
{
    for (int i = 0; i < program_pair.size(); i++) {
        delete program_pair[i];
    }
    delete ui;
}

void ConfirmTool::insertLine(char* text, bool color, int n){//n表示哪个文本框
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(QColor("black")));//字体颜色
    //背景颜色
    if(color){
        fmt.setBackground(QBrush(QColor("yellow")));//文本不同，为黄色
    }
    else{
        fmt.setBackground(QBrush(QColor("white")));
    }
    if(n == 1){
        ui->text_program_1->mergeCurrentCharFormat(fmt);
        ui->text_program_1->insertPlainText(text);
    }
    else{
        ui->text_program_2->mergeCurrentCharFormat(fmt);
        ui->text_program_2->insertPlainText(text);
    }
}

void ConfirmTool::showText(){
    //清除之前的文本
    ui->text_program_1->clear();
    ui->text_program_2->clear();
    if(ind >= sz){
        QMessageBox::information(this,"Finished","所有程序对都已确认");
        return ;
    }
    string tmp = path_str + string(program_pair[ind]->file1_path);
    FILE* program_1 = fopen(tmp.c_str(), "r");
    if (program_1 == NULL) {
        cerr << "program_1.cpp打开失败";
        exit(-1);
    }
    tmp = path_str + string(program_pair[ind]->file2_path);
    FILE* program_2 = fopen(tmp.c_str(), "r");
    if (program_2 == NULL) {
        cerr << "program_2.cpp打开失败";
        exit(-1);
    }
    //显示程序对路径
    ui->text_program_path->setText(program_pair[ind]->file1_path);
    ui->text_program_path->append(program_pair[ind]->file2_path);
    char text1[200] = { 0 }, text2[200] = { 0 };
    bool color = false;//是否需要不同颜色
    while (!feof(program_1) && !feof(program_2)) {
        //逐行比对，如果文本不同用不同颜色显示
        fgets(text1, 199, program_1);
        fgets(text2, 199, program_2);
        if (strcmp(text1, text2) != 0) {
            color = true;
        }
        //输出该行
        insertLine(text1,color,1);
        insertLine(text2,color,2);
    }
    //程序1没读完，而2读完了
    if(!feof(program_1)){
        while(!feof(program_1)){
            fgets(text1, 99, program_1);
            //直接设为高亮（背景黄色）
            insertLine(text1,true,1);
        }
    }
    //程序2没读完，而1读完了
    else if(!feof(program_2)){
        while(!feof(program_2)){
            fgets(text2, 99, program_2);
            //直接设为高亮（背景黄色）
            insertLine(text2,true,2);
        }
    }
    fclose(program_1);
    fclose(program_2);
}

void ConfirmTool::on_btn_start_clicked()
{
    if(ind >= sz){
        QMessageBox::information(this,"注意","所有程序对都已被确认过！");
        return;
    }
    if(!isStart){
        //“开始”按钮与“确认下一个”按钮共用
        //在点击“开始”后，“开始”变为“确认下一个”
        ui->btn_start->setText("顺序确认下一个");
        isStart = true;
    }
    else{//点击顺序下一个
        if(program_pair[ind]->flag == 0){
            //要求必须对当前程序对进行确认
            QMessageBox::information(this,"注意","该程序对尚未确认！");
            return;
        }
        //说明目前显示的是跳转到的推荐程序对，而顺序下一个是指没跳转之前的下一个
        if(before_ind != -1){
            ind = before_ind;
            before_ind = -1;
        }
        ind++;
        if(ind >= sz){
            QMessageBox::information(this,"注意","所有程序对都已被确认过！");
            return;
        }
        if(program_pair[ind]->flag != 0){//已经被确认过（之前推荐过并进行了确认）
            QMessageBox::information(this,"注意","下一个程序对已被确认过！");
        }
    }
    showText();
    //清除之前推荐的结果
    ui->text_recommend->clear();
    recmd = -1;
}


void ConfirmTool::on_btn_equal_clicked()
{
    if(!isStart || ind >= sz){//开始前或结束后点击按钮无反应
        return;
    }
    //二次确认
    QMessageBox::StandardButton mes;
    mes = QMessageBox::question(this, "二次确认", "确认结果为：等价？", QMessageBox::Yes|QMessageBox::No);
    if (mes == QMessageBox::Yes) {
        program_pair[ind]->flag = 1;
    }
    //推荐下一个
    recommend();
}


void ConfirmTool::on_btn_inequal_clicked()
{
    if(!isStart || ind >= sz){//开始前点击按钮无反应
        return;
    }
    //二次确认
    QMessageBox::StandardButton mes;
    mes = QMessageBox::question(this, "二次确认", "确认结果为：不等价？", QMessageBox::Yes|QMessageBox::No);
    if (mes == QMessageBox::Yes) {
        program_pair[ind]->flag = 2;
    }
    //推荐下一个
    recommend();
}


void ConfirmTool::on_btn_unsure_clicked()
{
    if(!isStart || ind >= sz){//开始前点击按钮无反应
        return;
    }
    //二次确认
    QMessageBox::StandardButton mes;
    mes = QMessageBox::question(this, "二次确认", "确认结果为：存疑？", QMessageBox::Yes|QMessageBox::No);
    if (mes == QMessageBox::Yes) {
        program_pair[ind]->flag = 3;
    }
    //不会推荐下一个，但会清除推荐文本框
    ui->text_recommend->clear();
}

void ConfirmTool::recommend()
{
    /* 推荐逻辑：
     * 如果当前判断a与b等价，则遍历数组，找与a等价的，记作c，再看c是否与b等价，如果不是则推荐b和c，
     * 如果找不到满足的则不做推荐
     * 不等价的情况类似，设a与b不等价，则找与a等价的c，若c与b等价则推荐c和b
     * 如果遍历到的程序对尚未确认过，以等价判断结果为准（即isEqual），否则按确认结果
    */
    //清除之前推荐的结果
    ui->text_recommend->clear();
    recmd = -1;//如果找不到满足条件的程序对的话，recmd会被设为-1，即不做推荐
    char a[100], b[100], c[100];
    strcpy(a, program_pair[ind]->file1_path);
    strcpy(b, program_pair[ind]->file2_path);
    if(program_pair[ind]->flag == 1){//等价
        for(int i = 0; i < program_pair.size(); i++){
            if(i == ind){//跳过本身
                continue;
            }
            if(strcmp(program_pair[i]->file1_path, a) == 0){//找与a等价的c
                if((program_pair[i]->flag == 0 && program_pair[i]->isEqual) || program_pair[i]->flag == 1){
                    strcpy(c, program_pair[i]->file2_path);
                    for(int j = 0; j < program_pair.size(); j++){
                        //定位到b和c的程序对
                        if((strcmp(program_pair[j]->file1_path, c) == 0 && strcmp(program_pair[j]->file2_path, b) == 0) || (strcmp(program_pair[j]->file1_path, b) == 0 && strcmp(program_pair[j]->file2_path, c) == 0)){
                            if((program_pair[j]->flag == 0 && !program_pair[j]->isEqual) || program_pair[j]->flag == 2 || program_pair[j]->flag == 3){
                                //推荐b和c
                                recmd = j;
                            }
                        }
                    }
                }
            }
            else if(strcmp(program_pair[i]->file2_path, a) == 0){
                if((program_pair[i]->flag == 0 && program_pair[i]->isEqual) || program_pair[i]->flag == 1){
                    strcpy(c, program_pair[i]->file1_path);
                    for(int j = 0; j < program_pair.size(); j++){
                        //定位到b和c的程序对
                        if((strcmp(program_pair[j]->file1_path, c) == 0 && strcmp(program_pair[j]->file2_path, b) == 0) || (strcmp(program_pair[j]->file1_path, b) == 0 && strcmp(program_pair[j]->file2_path, c) == 0)){
                            if((program_pair[j]->flag == 0 && !program_pair[j]->isEqual) || program_pair[j]->flag == 2 || program_pair[j]->flag == 3){
                                //推荐b和c
                                recmd = j;
                            }
                        }
                    }
                }
            }
        }
    }
    else if(program_pair[ind]->flag == 2){//不等价
        for(int i = 0; i < program_pair.size(); i++){
            if(i == ind){//跳过本身
                continue;
            }
            if(strcmp(program_pair[i]->file1_path, a) == 0){//找与a等价的c
                if((program_pair[i]->flag == 0 && program_pair[i]->isEqual) || program_pair[i]->flag == 1){
                    strcpy(c, program_pair[i]->file2_path);
                    for(int j = 0; j < program_pair.size(); j++){
                        //定位到b和c的程序对
                        if((strcmp(program_pair[j]->file1_path, c) == 0 && strcmp(program_pair[j]->file2_path, b) == 0) || (strcmp(program_pair[j]->file1_path, b) == 0 && strcmp(program_pair[j]->file2_path, c) == 0)){
                            if((program_pair[j]->flag == 0 && program_pair[j]->isEqual) || program_pair[j]->flag == 1){
                                //推荐b和c
                                recmd = j;
                            }
                        }
                    }
                }
            }
            else if(strcmp(program_pair[i]->file2_path, a) == 0){
                if((program_pair[i]->flag == 0 && program_pair[i]->isEqual) || program_pair[i]->flag == 1){
                    strcpy(c, program_pair[i]->file1_path);
                    for(int j = 0; j < program_pair.size(); j++){
                        //定位到b和c的程序对
                        if((strcmp(program_pair[j]->file1_path, c) == 0 && strcmp(program_pair[j]->file2_path, b) == 0) || (strcmp(program_pair[j]->file1_path, b) == 0 && strcmp(program_pair[j]->file2_path, c) == 0)){
                            if((program_pair[j]->flag == 0 && program_pair[j]->isEqual) || program_pair[j]->flag == 1){
                                //推荐b和c
                                recmd = j;
                            }
                        }
                    }
                }
            }
        }
    }
    //存疑不做推荐
    if(recmd != -1){
        //有推荐的，显示到文本框中
        ui->text_recommend->setText(program_pair[recmd]->file1_path);
        ui->text_recommend->append(program_pair[recmd]->file2_path);
    }
}

void ConfirmTool::on_btn_end_clicked()
{
    if(!isStart){
        return;
    }
    for(int i = 0; i < program_pair.size(); i++){
        if(program_pair[i]->flag == 0){
            QMessageBox::information(this,"注意","还有程序对没确认完，请继续确认！");
            return;
        }
    }
    res_output();
    QMessageBox::information(this,"结束","确认结果已输出到文件");
    this->close();
}

void ConfirmTool::res_output()
{
    string tmp = path_str + "output/equal.csv";
    FILE* f_equal = fopen(tmp.c_str(), "w");
    if (f_equal == NULL) {
        cerr << "equal.csv打开失败";
        exit(-1);
    }
    tmp = path_str + "output/inequal.csv";
    FILE* f_inequal = fopen(tmp.c_str(), "w");
    if (f_inequal == NULL) {
        cerr << "inequal.csv打开失败";
        exit(-1);
    }
    tmp = path_str + "output/unsure.csv";
    FILE* f_unsure = fopen(tmp.c_str(), "w");//存疑保存在该文件中
    if (f_unsure == NULL) {
        cerr << "unsure.csv打开失败";
        exit(-1);
    }
    for (int i = 0; i < program_pair.size(); i++) {
        if (program_pair[i]->flag == 1) {
            fprintf(f_equal, "%s,%s\n", program_pair[i]->file1_path, program_pair[i]->file2_path);
        }
        else if (program_pair[i]->flag == 2) {
            fprintf(f_inequal, "%s,%s\n", program_pair[i]->file1_path, program_pair[i]->file2_path);
        }
        else if (program_pair[i]->flag == 3) {
            fprintf(f_unsure, "%s,%s\n", program_pair[i]->file1_path, program_pair[i]->file2_path);
        }
    }
    fclose(f_equal);
    fclose(f_inequal);
    fclose(f_unsure);
}

void ConfirmTool::on_btn_recommend_clicked()
{
    if(recmd == -1 || ind >= sz){
        return;
    }
    //记录跳转前的ind，包括连续跳转
    if(before_ind == -1){
        before_ind = ind;
    }
    ind = recmd;
    showText();
}

