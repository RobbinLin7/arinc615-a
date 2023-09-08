#ifndef GLOBALDEFINE_H
#define GLOBALDEFINE_H
#include<string>
namespace GlobalDefine{

#define FUNC_RETUEN_SUCCESS         0           //函数正常返回
#define FUNC_RETUEN_ERROR           (-1)        //函数错误返回


#define MAINWINDOW_TITLE            "加卸载工具(ARINC615ATool-V1.0.20-beta)"
#define WINDOW_WIDTH                1500        //界面宽度
#define WINDOW_HEIGHT               700         //界面高度

#define MAX_LIST_WIDGET_WIDTH       600         //设备列表中widget的宽度
#define MAX_LIST_WIDGET_HEIGHT      70         //设备列表中widget的高度

#define MAX_ADD_VALUE               65535       //自增的最大值

#define MAX_TARGET_NUMBER           16          //最大支持的目标机数量

#define FIND_OPERATION                      "FIND"
#define INFORMATION_OPERATION               "INFORMATION"
#define UPLOAD_OPERATION                    "UPLOAD"
#define MEDIA_DOWNLOAD_OPERATION            "MEDIA-DOWNLOAD"
#define OPERATOR_DOWNLOAD_OPERATION         "OPERATOR-DOWNLOAD"
#define AUTO_CONFIG_OPERATION               "AUTO-CONFIG"
#define PARAMETER_CONFIG_OPERATION          "PARAMETER-CONFIG"

#define FIND_OP_CODE                        1
#define INFO_OP_CODE                        2
#define UPLOAD_OP_CODE                      3
#define MDOWN_OP_CODE                       4
#define ODOWN_OP_CODE                       5
#define AUTO_CONFIG_OP_CODE                 6
#define PARAMETER_CONFIG_OP_CODE            7

#define TFTP_LISTEN_PORT                    69
#define FIND_LISTEN_PORT                    1001

}




#endif // GLOBALDEFINE_H
