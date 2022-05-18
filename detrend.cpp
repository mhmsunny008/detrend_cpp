#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <numeric>
#include <set>
#include <algorithm>
#include "filter.h"
#include <complex.h>
#include <math.h>
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <numeric>
#include <set>
vector<float> detrend_func(vector<float> y, int m)
{
    // https://www.cxymm.net/article/weixin_42918498/120875063
    float xmean, ymean;
    int i;
    float temp;
    float Sxy;
    float Sxx;

    float grad;
    float yint;

    /********************************
    Set the X axis Liner Values
    *********************************/
    vector<float> x;
    for (i = 0; i < m; i++)
        x.push_back(i);

    /********************************
    Calculate the mean of x and y
    *********************************/
    xmean = 0;
    ymean = 0;
    for (i = 0; i < m; i++)
    {
        xmean += x[i];
        ymean += y[i];
    }
    xmean /= m;
    ymean /= m;

    /********************************
    Calculate Covariance
    *********************************/
    temp = 0;
    for (i = 0; i < m; i++)
        temp += x[i] * y[i];
    Sxy = temp / m - xmean * ymean;

    temp = 0;
    for (i = 0; i < m; i++)
        temp += x[i] * x[i];
    Sxx = temp / m - xmean * xmean;

    /********************************
    Calculate Gradient and Y intercept
    *********************************/
    grad = Sxy / Sxx;
    yint = -grad * xmean + ymean;

    /********************************
    Removing Linear Trend
    *********************************/
    vector<float> y_new;
    for (i = 0; i < m; i++)
        y_new.push_back(y[i] - (grad * i + yint));
    return y_new;
}

vector<float> detrend_window(vector<float> signal, int FPS)
{
    /*步骤如下：
    1. 整体线性detrend，使得余数部分的信号在拼接时不会相差太大
    2. 对整除部分的信号，使用滑动时间窗进行线性detrend，使其达到非线性detrend的效果
    3. 将整除部分detrend之后的信号和余数部分的信号进行拼接
    4. 输出拼接之后的信号*/

    //线性detrend：利用整体信号进行detrend，使得能够保持余数部分的数据，不至于拼接时数组元素之间的值相差太大
    vector<float> signal_detrend = detrend_func(signal, signal.size());

    // detrend的整除部分，使用滑动时间窗进行线性detrend，以达到非线性detrend效果
    int win_len = 2;//以每win_len秒帧数的滑动窗口长度来计算detrend
    int count_num = signal_detrend.size() / (FPS * win_len);
    vector<double> signal_win_arr;
    for (int i = 0; i < count_num; i++) {
        vector<float> signal_win;
        for (int j = 0; j < (FPS * win_len); j++)
        {
            signal_win.push_back(signal_detrend[i * FPS * win_len + j]);

        }
        signal_win = detrend_func(signal_win, signal_win.size());//滑动时间窗进行detrend
        for (int k = 0; k < signal_win.size(); k++)
        {
            signal_win_arr.push_back(signal_win[k]);//拼接每个滑动时间窗的信号
        }
    }

    // 信号拼接
    vector<float> signal_detrend_win;
    for (int m = 0; m < signal_detrend.size(); m++)
    {
        if (m < signal_win_arr.size())
        {
            signal_detrend_win.push_back(signal_win_arr[m]);//拼接整除部分的信号
        }
        else
        {
            signal_detrend_win.push_back(signal_detrend[m]);// 拼接余数部分的信号
        }

    }
    return signal_detrend_win;
}

vector<float> readCsv(string file)
{
    vector<float> signalArr;
    ifstream fp(file); //定义声明一个ifstream对象，指定文件路径
    string line;
    float value;
    while (getline(fp, line)) { //循环读取每行数据
        //vector<float> data_line;
        string number;
        istringstream readstr(line); //string数据流化
        getline(readstr, number, ','); //循环读取数据;将一行数据按'，'分割
        value = atof(number.c_str());
        signalArr.push_back(value);
    }
    return signalArr;
}
 int main()
{
    string data_path = "D:/VS_Program/polyfit_cpp/signal_rr/ppg_infrared_nose_25.csv";
    vector<float> nose_signal_original = readCsv(data_path);//读取呼吸信号
    vector<float> signal_detrend_unlinear = detrend_window(nose_signal_original, FPS);
    return 0;
}
                