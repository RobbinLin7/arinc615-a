#include "stdafx.h"
#include "api_615a_socket.h"

#include "file_define.h"
#include "create_protocol.h"
#include "message_define.h"
#include "tftpClient.h"

#include "abort.h"
#include "findOp.h"
#include "informationOp.h"
#include "uploadOp.h"
#include "mediaDownloadOp.h"
#include "operatorDownloadOp.h"
#include "getStatus.h"
#include "globalConfig.h"

#include <process.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <io.h>

#define TEST_STATUS_THREAD 0			//测试状态文件线程的执行

char getFolderPath[FILE_PATH_BUFFER_SIZE] = { 0 };

char sendFolderPath[FILE_PATH_BUFFER_SIZE] = { 0 };

char fileFolderPath[FILE_PATH_BUFFER_SIZE] = { 0 };

char HOST_ADDRESS[MAX_BUFFER_64_SIZE] = {0};
//（需要 #include <io.h> 以及 #include <direct.h>）
// 创建文件夹
void createFolder(const char *folderName, char *destPath)
{
	int ret = 0;

	char basePath[FILE_NAME_BUFFER_SIZE] = { 0 };

	//判断传入的参数是否合法
	if (NULL == folderName)
	{
		printf("string is NULL!\n");
		return;
	}

	//获取基路径
	if (NULL == _getcwd(basePath, FILE_NAME_BUFFER_SIZE))
	{
		printf("Not get base path!\n");
	}
	else
	{
		printf("base path: %s.\n", basePath);
	}

	//拼完整的路径
	sprintf(basePath, "%s%s", basePath, folderName);
	printf("folder path: %s.\n", basePath);

	// 文件夹不存在则创建文件夹
	if (_access(basePath, 0) == -1)
	{
		printf("folder not exist!\n");
		ret = _mkdir(basePath); // 只能一次创建一层目录

		if (0 == ret)
		{
			printf("1 basePath = %d.\n", strlen(basePath));
			memcpy(destPath, basePath, strlen(basePath));
		}
	}
	else
	{
		printf("folder although exist!\n");
		printf("2 basePath = %d.\n", strlen(basePath));
		memcpy(destPath, basePath, strlen(basePath));
	}

	return ;
}

void strLUS2a615_status(struct File_LUS lus, a615_status_t *status)
{
	int i = 0;

	if (NULL == status)
	{
		printf("a615_status LUS's ptr is NULL!\n");
		return;
	}

	status->counter = lus.counter;
	memcpy(status->description, lus.stat_des, lus.stat_des_len);

	memcpy(status->load_ratio, lus.load_list_ratio, ARINC615_LOAD_RATIO_LENGTH);

	status->status_code = a615_status_t_codes_e(lus.op_stat_code);

	status->num_of_files = lus.hfile_num;
	status->load_files = (a615_file_status_t *)malloc(lus.hfile_num * sizeof(lus.hfile_num));
	for (i = 0; i < lus.hfile_num; i++)
	{
		memcpy(status->load_files[i].file_name, lus.LUS_files[i].Hfile_name, lus.LUS_files[i].Hfile_name_len);

		status->load_files[i].file_status = a615_status_t_codes_e(lus.LUS_files[i].load_stat);

		status->load_files[i].file_name_len = lus.LUS_files[i].Hfile_name_len;
	}

	//printf("a615_status LUS version finish!\n");

	return;
}

void strLNS2a615_status(struct File_LNS lns, a615_status_t *status)
{
	int i = 0, j = 0;
	if (NULL == status)
	{
		printf("a615_status LNS's ptr is NULL!\n");
		return;
	}

	status->counter = lns.counter;
	//strcpy(status->description, lns.stat_des, lns.stat_des_len);
	for (i = 0; i < lns.stat_des_len; i++)
	{
		status->description[i] = lns.stat_des[i];
	}
#if 0
	//memcpy(status->load_ratio, lns.down_list_ratio, 4);
	for (i = 0; i < lns.stat_des_len; i++)
	{
		status->load_ratio[i] = lns.down_list_ratio[i];
	}
#endif

    /* 打印下载比例，added by liuli 20211206 */
    memcpy(status->load_ratio, lns.down_list_ratio, ARINC615_LOAD_RATIO_LENGTH);
    printf("status->load_ratio:%s\n", status->load_ratio);

	status->status_code = a615_status_t_codes_e(lns.op_stat_code);

	printf("strLNS2a615_status-> lns.op_stat_code = 0x%x.\n", lns.op_stat_code);

	status->num_of_files = lns.file_num;

	if (lns.LNS_files == NULL || 0 == lns.file_num)
	{
		return;
	}
	status->load_files = (a615_file_status_t *)malloc(lns.file_num * sizeof(a615_file_status_t));
	for (i = 0; i < lns.file_num; i++)
	{
		//memcpy(status->load_files[i].file_name, lns.LNS_files[i].file_name, lns.LNS_files[i].file_name_len);
		for (j = 0; j < lns.LNS_files[i].file_name_len; j++)
		{
			status->load_files[i].file_name[j] = lns.LNS_files[i].file_name[j];
		}

		status->load_files[i].file_status = a615_status_t_codes_e(lns.LNS_files[i].file_stat);

		status->load_files[i].file_name_len = lns.LNS_files[i].file_name_len;
	}

	//printf("a615_status LNS version finish!\n");

	return;
}

void strLCL2a615_target_info(struct File_LCL str_lcl, a615_target_info_t *info)
{
	int i = 0, j = 0;
	if (NULL == info)
	{
		printf("a615_status LCL's ptr is NULL!\n");
		return;
	}

	info->file_len = str_lcl.file_len;
	info->pro_ver = str_lcl.Pro_ver;

	info->hw_num = str_lcl.Hw_num;

	for (int i = 0; i < str_lcl.Hw_num; i++)
	{
		info->hws[i].lit_name_len = str_lcl.Hws[i].name_len;

		memcpy(info->hws[i].literal_name, str_lcl.Hws[i].file_name, str_lcl.Hws[i].name_len);

		info->hws[i].se_num_len = str_lcl.Hws[i].se_num_len;

		memcpy(info->hws[i].serial_num, str_lcl.Hws[i].serial_num, str_lcl.Hws[i].se_num_len);

		info->hws[i].parts_num = str_lcl.Hws[i].parts_num;

		info->hws[i].parts = (a615_parts_info_t *)malloc(sizeof(a615_parts_info_t) * str_lcl.Hws[i].parts_num);
		for (int j = 0; j < str_lcl.Hws[i].parts_num; j++)
		{

			info->hws[i].parts[j].amend_len = str_lcl.Hws[i].parts[j].Amendment_len;

			memcpy(info->hws[i].parts[j].amend,
				str_lcl.Hws[i].parts[j].Amendment,
				str_lcl.Hws[i].parts[j].Amendment_len);

			info->hws[i].parts[j].parts_num_len = str_lcl.Hws[i].parts[j].parts_num_len;

			memcpy(info->hws[i].parts[j].parts_num,
				str_lcl.Hws[i].parts[j].partsNo,
				str_lcl.Hws[i].parts[j].parts_num_len);

			info->hws[i].parts[j].part_desig_text_len = str_lcl.Hws[i].parts[j].Part_Designation_Text_Length;

			memcpy(info->hws[i].parts[j].part_desig_text,
				str_lcl.Hws[i].parts[j].Part_Designation_Text,
				str_lcl.Hws[i].parts[j].Part_Designation_Text_Length);

		}

	}

	return;
}

void strLNL2file_download(struct File_LNL lnl, a615_down_file_list_t *file_list)
{
	int i = 0;
	if (NULL == file_list)
	{
		printf("a615_status LNL's ptr is NULL!\n");
		return;
	}

	file_list->file_num = lnl.file_num;

	for (i = 0; i < lnl.file_num; i++)
	{
		strcpy(file_list->file_name[i], lnl.file[i].file_name);
	}

	return;
}

void file_download2FileList(a615_down_file_list_t fd, FileList *fl)
{
	int i = 0;
	if (NULL == fl)
	{
		printf("a615_status LNL's ptr is NULL!\n");
		return;
	}
	fl->file_number = fd.file_num;

	for (i = 0; i < fd.file_num; i++)
	{
		strcpy(fl->oneFile[i].name, fd.file_name[i]);

		fl->oneFile[i].file_len = 1;
	}

	return;
}

//bool_t a615_init(uint16_t net_type, void *network_info,
//	uint16_t* err_code, uint8_t* err_msg)
int a615_init(uint16_t net_type, void *network_info, int16_t* err_code)
{
	int retVal = A615_SUCC;
	int ret_val = 0;

	*err_code = A615_SUCC;
	//memcpy(err_msg, EXEC_SUCCEED_MSG, strlen(EXEC_SUCCEED_MSG));

	
	//if (net_type == AFDX_NETWORK)
	if (net_type == AFDX_NET)
	{
#if AFDX_NETWORK
		afdx_cfg_info tmp_network;

		memcpy(&tmp_network, (afdx_cfg_info *)network_info, sizeof(afdx_cfg_info));

		ret_val = AfdxOpenDev(tmp_network);

		if (0 != ret_val)
		{
			*err_code = A615_INIT_ERROR02;
			//memcpy(err_msg, NETWORK_INIT_ERR_MSG_001, strlen(NETWORK_INIT_ERR_MSG_001));
			retVal = A615_ERROR;
		}
#endif

	}
	else if (net_type == SOCKET_NET)
	{
		//*err_code = A615_INIT_ERROR02;
		//设置本机IP
		if(network_info != NULL)
		{
			memcpy(HOST_ADDRESS, (char *)network_info, strlen((char *)network_info));

			printf("a615_init:: set host IP is %s, %s, length: %d!\n", HOST_ADDRESS, (char *)network_info, sizeof((char *)network_info));
		}
		else
		{
			char default_ip[] = "192.168.5.5";
			memcpy(HOST_ADDRESS, default_ip, strlen(default_ip));

			printf("a615_init:: set default IP is %s!\n", HOST_ADDRESS);
		}

		initUDPSocketAddr();
	}
	else
	{
		*err_code = A615_INIT_ERROR01;
		retVal = A615_ERROR;
	}
	return retVal;
}

//a615_targets_find* arinc615_find_op(uint16_t* err_code, uint8_t* err_msg)
int a615_find_op(a615_targets_find_list_t *targets_info, int16_t *err_code)
{
	//a615_targets_find_list_t *targets_info = NULL;

	//targets_info = (a615_targets_find_list_t *)malloc(sizeof(a615_targets_find_list_t));

	targets_info->device_num = 0;

	*err_code = A615_SUCC;

	//memcpy(err_msg, EXEC_SUCCEED_MSG, strlen(EXEC_SUCCEED_MSG));
	//targets_info.device_num = 0;

	//targets_info->targets_info = (a615_target_find_info_t *)malloc(MAX_FIND_DEVICE_NUMBER * sizeof(a615_target_find_info_t));

	printf("find OP begining!\n");
	int flag_register = FINDRequest(targets_info);

	if (Find_Ok == flag_register)
	{
		//find完后在当前工作目录下创建三个文件夹
		//分别存放接收、发送、和数据文件
#if 1			//2022-3-1 add
		createFolder("\\Get", getFolderPath);

		printf("\nfind getFolderPath: %s.\n", getFolderPath);

		createFolder("\\Send", sendFolderPath);

       // /* 20211202：修改数据文件存储路径为用户指定路径，by liuli  */
		createFolder("\\File", fileFolderPath);
#endif
		printf("find OP finished!\n");
		return A615_SUCC;
	}
	else
	{
		*err_code = A615_OPT_ERROR02;
		//memcpy(err_msg, FIND_ERR_MSG_100, strlen(FIND_ERR_MSG_100));
		return A615_ERROR;
	}

	return 0;
}


//bool_t arinc615_information_op(a615_status_func fptr, char *client_ID, a615_target_information* target_info,
//	uint16_t* err_code, uint8_t* err_msg)
int a615_information_op(a615_status_t_callback_func_t fptr, char *client_id, char *dest_ip_addr,
	a615_target_info_t *target_info, int16_t *err_code)
{
	int retVal = A615_SUCC;

	bool_t func_ret = bool_t::ARINC615_TRUE;

	uint8_t err_msg[ERROR_MESSAGE_BUFFER] = { 0 };

	a615_status_t tmp_status;

	struct File_LCL lcl;

    //request_pck_timeout = 0;

	//target_info = (a615_target_info_t *)malloc(sizeof(a615_target_info_t));
    //if(NULL != target_info)
    //{
    //    memset(target_info, 0, sizeof(a615_target_info_t));
    //}
    

	//tmp_info = (a615_target_information *)malloc(sizeof(a615_target_information));
	//memset(target_info, 0, sizeof(a615_target_information));

	//初始化错误码和错误信息
	*err_code = A615_SUCC;
	//memcpy(err_msg, EXEC_SUCCEED_MSG, strlen(EXEC_SUCCEED_MSG));
#if 0
	//初始化开始，起状态文件接收线程
	uintptr_t hStdOutStr = _beginthread(getStatusFile, 0, NULL);

	if (hStdOutStr == -1) {
		printf("getStatusFile ThreadStatus error!!\n\n");
		*err_code = STATUS_THREAD_FAILED_CODE;
		memcpy(err_msg, STATUS_THREAD_FAILED_MSG, strlen(STATUS_THREAD_FAILED_MSG));
		return NULL;
	}
#endif
	//初始化阶段
	func_ret = informationInit(client_id, dest_ip_addr, err_code, err_msg);

	if (bool_t::ARINC615_FALSE == func_ret)
	{
		*err_code = A615_OPT_ERROR04;
		printf("err msg: %s.\n", err_msg);

		if (NULL != target_info)
		{
			free(target_info);

			target_info = NULL;
		}

		return A615_ERROR;
	}
	else
	{
		//执行回调函数
		//fptr(&tmp_status);

	}

	//传输阶段, 传出设备信息
	func_ret = informationFile(err_code, err_msg);

	if (bool_t::ARINC615_FALSE == func_ret)
	{
		*err_code = A615_OPT_ERROR05;
		printf("err msg: %s.\n", err_msg);

		return A615_ERROR;
	}
	else
	{
		//LCL文件解析，结构体赋值，传出
		//fptr(&tmp_status);

		lcl = parsing_LCL(client_id);

		target_info->hws = (a615_hardware_info_t*)malloc(sizeof(a615_hardware_info_t) * lcl.Hw_num);

		strLCL2a615_target_info(lcl, target_info);

        printf("target num: befor: %d, after: %d.\n", lcl.Hw_num, target_info->hw_num);
        printf("file len: befor: %d, after: %d.\n", lcl.file_len, target_info->file_len);
		//return retVal;
	}
#if 0
	if (NULL != target_info->hws)
	{

		free(target_info->hws);

		target_info->hws = NULL;
	}

	if (NULL != target_info)
	{
		free(target_info);

		target_info = NULL;
	}
#endif
	return retVal;
}

//bool_t arinc615_upload_op(a615_status_func fptr, char *client_ID, 
//	uint16_t* err_code, uint8_t* err_msg, luh_info str_luh)
int a615_upload_op(void *input, a615_status_t_callback_func_t fptr, char *dest_ip_addr,
	a615_data_file_list_t load_file, char *client_id, int16_t *err_code)
{
	int retVal = A615_SUCC;

	bool_t func_ret = bool_t::ARINC615_TRUE;

	uint8_t err_msg[ERROR_MESSAGE_BUFFER] = { 0 };

	int i = 0;
    int access_ret = 0;

	int wait_status_times = 0;

	uint8 test10_11[2] = { 0 };

    char file_LUS[FILE_PATH_BUFFER_SIZE] = {0};
    sprintf(file_LUS, "%s\\%s%s", getFolderPath, client_id,".LUS");

	struct File_LUS filelus;
	a615_status_t lus_file;

    request_pck_timeout = 0;

    //File_LUH tmp_luh;
    int createLURRet = 0;

	FileList tmp_file_list;

    //memset(&tmpLUR, 0, sizeof(FileList));

	//初始化错误码和错误信息
	*err_code = A615_SUCC;

	if (load_file.file_num <= 0 || load_file.file_num > MAX_TRANS_FILE_NUMBER)
	{
		*err_code = A615_OPT_ERROR22;
		//printf("err msg: %s.\n", err_msg);
		return A615_ERROR;
	}
	//memcpy(err_msg, EXEC_SUCCEED_MSG, strlen(EXEC_SUCCEED_MSG));

	//初始化阶段
	func_ret = uploadInit(client_id, dest_ip_addr, err_code, err_msg);

	if (bool_t::ARINC615_FALSE == func_ret)
	{
		*err_code = A615_OPT_ERROR08;
		printf("err msg: %s.\n", err_msg);
		return A615_ERROR;
	}

	printf("Upload Init finished, begin parse LUS file.\n");
    //若文件存在,先删除旧文件
    access_ret = _access(file_LUS, 0);
    if(0 == access_ret)
    {
        if(0 == remove(file_LUS))
            printf("remove file: %s OK.\n\n\n", file_LUS);
        else
            printf("remove file: %s ERROR.\n\n\n", file_LUS);
    }
    else
    {
        printf(" file: %s not exist!\n\n", file_LUS);
    }
    printf("remove LUS file.\n");
#if 1
	//起状态文件接收线程
	uintptr_t hStdOutStr = _beginthread(getStatusFile, 0, dest_ip_addr);

	if (hStdOutStr == -1)
	{
		printf("getStatusFile ThreadStatus error!!\n\n");
		*err_code = STATUS_THREAD_FAILED_CODE;
		//printf("err msg: %s.\n", err_msg);
		//memcpy(err_msg, STATUS_THREAD_FAILED_MSG, strlen(STATUS_THREAD_FAILED_MSG));
		return A615_ERROR;
	}
#endif

    while(1)
    {
        access_ret = _access(file_LUS, 0);
        //阻塞等待LUS文件生成成功
        printf("wait LUS file!\n\n");
        if(0 == access_ret)
        {
	        //解析LUS文件
	        filelus = parsing_LUS(client_id);

	        if (PROTOCOL_FILE_ERROR == filelus.file_len)
	        {
		        printf("Parse LUS error LUS not exist!\n");
		        *err_code = A615_OPT_ERROR21;
                //continue;
                return A615_ERROR;
	        }
	        else
	        {
		        //结构体赋值
		        strLUS2a615_status(filelus, &lus_file);

                printf("L*S status code : 0x%x.\n\n", lus_file.status_code);
                //执行回调函数
		        retVal = fptr(input, &lus_file);

                
		        if (lus_file.status_code == OPT_ABTED_DUETO_INIT ||
			        lus_file.status_code == OPT_ABTED_DUETO_DATALOADER ||
			        lus_file.status_code == OPT_ABT_BY_TAR)
		        {
			        //接收到终止码，停止upload操作
			        //global_abort_code = AOORT_CODE;
                    printf("recv A ABORT STATUS! ABORT UPLOAD OPERATION!");
                    WaitForSingleObject(mutex, 100);
                    global_abort_code = 0;
                    backflag = 0;
                    ReleaseMutex(mutex);
                    free(lus_file.load_files);
                    *err_code = A615_ABORT_CODE;
			        return A615_ABORT_CODE;
		        }
                break;

	        }
            free(filelus.LUS_files);

            break;
        }
        else
        {
			wait_status_times++;
            
			if (wait_status_times > MAX_STATUS_TIMEOUT)
			{
				//解析失败
				printf("Parse LNS error!\n");
				*err_code = A615_OPT_ERROR23;
				//printf("err msg: %s.\n", err_msg);
				wait_status_times = 0;
				return A615_ERROR;
			}

			Sleep(STATUS_TIMEOUT);
            continue;
        }

    }

	//列表传输阶段

	//生成LUR文件
	//create_LUR_new(client_id, str_luh.file_list.file_num, str_luh.file_list);

	tmp_file_list.file_number = load_file.file_num;

	memset(tmp_file_list.oneFile, 0, sizeof(OneFile) * 50);
	for (i = 0; i < load_file.file_num; i++)
	{
		tmp_file_list.oneFile[i].check_value_type = load_file.data_file[i].check_value_type;
		tmp_file_list.oneFile[i].file_len = load_file.data_file[i].file_len;
		strcpy(tmp_file_list.oneFile[i].name,
			load_file.data_file[i].name);
		//strlen(load_file.data_file[i].name));
		strcpy(tmp_file_list.oneFile[i].path,
			load_file.data_file[i].path);
		//strlen(load_file.data_file[i].path));
	}

	createLURRet =create_LUR_new(client_id, load_file.file_num, tmp_file_list);

    if(-1 == createLURRet)
    {
        *err_code = A615_OPT_ERROR09;

        return A615_ERROR;
    }

	printf("arinc615_upload_op send LUR file.\n");
	func_ret = uploadList(client_id, dest_ip_addr, err_code, err_msg);

	if (bool_t::ARINC615_FALSE == func_ret)
	{
		printf("arinc615_upload_op send LUR file Error.\n");
		*err_code = A615_OPT_ERROR09;
		printf("LUR err msg: %s.\n", err_msg);

        free(lus_file.load_files);
		return A615_ERROR;
	}
	//头文件传输阶段,需要生成LUH文件
    //printf()
	create_LUH(client_id, 2, (uint16 *)test10_11, 2, (uint16 *)test10_11, 4, tmp_file_list);

    //tmp_luh.file_len

	func_ret = uploadFileHeader(err_code, err_msg);

	if (bool_t::ARINC615_FALSE == func_ret)
	{
		*err_code = A615_OPT_ERROR09;
		printf("LUH err msg: %s.\n", err_msg);

        //free(lus_file.load_files);
		return A615_ERROR;
	}

	//数据文件传输阶段
	uintptr_t hStdOutFile = _beginthread(uploadFileData, 0, NULL);

	if (hStdOutFile == -1) {
		printf("uploadFileData Thread file error!!\n");
		*err_code = A615_THREAD_ERROR08;
		memcpy(err_msg, STATUS_THREAD_FAILED_MSG, strlen(STATUS_THREAD_FAILED_MSG));
		printf("err msg: %s.\n", err_msg);
        *err_code = A615_ERROR;
		return A615_ERROR;
	}

	//状态传输阶段
	char oldLUS[FILE_PATH_BUFFER_SIZE] = { 0 };
	sprintf(oldLUS, "%s\\%s%s", getFolderPath, client_id, ".LUS");
	remove(oldLUS);
	while (1) {
		Sleep(ANALYSIS_STATUS_FILE_PERIOD);
		struct File_LUS filelus = parsing_LUS(client_id);

		int rate_ = (filelus.load_list_ratio[0] - '0') * 100
			+ (filelus.load_list_ratio[1] - '0') * 10
			+ (filelus.load_list_ratio[2] - '0') * 1;

        if (filelus.op_stat_code == OPERATION_FINISH_CODE || abs(rate_ - 100) < MIN_NUMBER_ERROR) {

            //调用回调函数
			strLUS2a615_status(filelus, &lus_file);
            retVal = fptr(input, &lus_file);
			WaitForSingleObject(mutex, 100);
			backflag = 0;
			ReleaseMutex(mutex);
            printf("upload main while STOP!\n\n");
            free(lus_file.load_files);
            *err_code = A615_SUCC;
			return A615_SUCC;
		}

		if (0 <= rate_ && rate_ <= A_HUNDRED_PERCENT)
		{
			//调用回调函数
			strLUS2a615_status(filelus, &lus_file);
			retVal = fptr(input, &lus_file);

			//判断操作状态码
			if (lus_file.status_code == OPT_ABTED_DUETO_INIT ||
				lus_file.status_code == OPT_ABTED_DUETO_DATALOADER ||
				lus_file.status_code == OPT_ABT_BY_TAR)
			{
				//接收到终止码，停止upload操作
				//global_abort_code = AOORT_CODE;
                printf("recv A ABORT STATUS! ABORT OPERATION!");
                WaitForSingleObject(mutex, 100);
                global_abort_code = 0;
                backflag = 0;
                ReleaseMutex(mutex);
                printf("upload main while STOP!\n\n");
                free(lus_file.load_files);
                *err_code = A615_ABORT_CODE;
				return A615_ABORT_CODE;
			}

		}

		if (rate_ > A_HUNDRED_PERCENT) {
			rate_ = A_HUNDRED_PERCENT;
			break;
		}
		
	}

    printf("upload main while STOP!\n\n");
    free(lus_file.load_files);

	return retVal;
}

//bool_t arinc615_media_download_op(a615_status_func fptr, char *client_ID,
//	uint16_t* err_code, uint8_t* err_msg)
int arinc615_media_download_op(void *input, a615_status_t_callback_func_t fptr, char *client_id, char *dest_ip_addr,
	a615_down_file_list_t file_list, int16_t *err_code)
{
	int retVal = A615_SUCC;

	bool_t func_ret = bool_t::ARINC615_TRUE;

	struct File_LNS lns;                //初始化阶段使用的临时结构体
	int analysisRet = 0;
	struct File_LNS filelns;            //数据文件交互时使用的结构体
	int lnsPhaseRet = 0;

	int loop = 0;

    int break_num = 0;          //用于中止while(1)循环
    int break_num2 = 0;

	int tmp_lnr = 0;

    request_pck_timeout = 0;

	uint8_t err_msg[ERROR_MESSAGE_BUFFER] = { 0 };

	a615_status_t stru_status;

	FileList tmp_file_list;

	//用于处理状态文件的临时变量
	char file_LNS[FILE_PATH_BUFFER_SIZE] = { 0 };
	sprintf(file_LNS, "%s\\%s%s", getFolderPath, client_id, ".LNS");
	int access_ret = 0;
	int wait_status_times = 0;

	//初始化结构体
	memset(&stru_status, 0, sizeof(a615_status_t));

	memset(&tmp_file_list, 0, sizeof(FileList));

	//初始化错误码和错误信息
	*err_code = A615_SUCC;
	//memcpy(err_msg, EXEC_SUCCEED_MSG, strlen(EXEC_SUCCEED_MSG));

	if (file_list.file_num <= 0 || file_list.file_num > 50)
	{
		*err_code = A615_OPT_ERROR22;
		//printf("err msg: %s.\n", err_msg);
		return A615_ERROR;
	}
	else
	{
		//结构体转换
		tmp_file_list.file_number = file_list.file_num;

		for (loop = 0; loop < file_list.file_num; loop++)
		{
			tmp_file_list.oneFile[loop].check_value_type = 1;
			tmp_file_list.oneFile[loop].file_len = 1;
			memset(tmp_file_list.oneFile[loop].path, 0, FILE_NAME_BUFFER_SIZE * sizeof(char));
			strcpy(tmp_file_list.oneFile[loop].name, file_list.file_name[loop]);
		}
	}

	//初始化阶段
	retVal = mediaDownloadInit(client_id, dest_ip_addr, err_code, err_msg);

	if (bool_t::ARINC615_FALSE == retVal)
	{
		//初始化阶段未执行成功
		*err_code = A615_OPT_ERROR11;
		printf("err msg: %s.\n", err_msg);
		return A615_ERROR;
	}

	//删除已有的状态文件
	access_ret = _access(file_LNS, 0);
	if (0 == access_ret)
	{
		if (0 == remove(file_LNS))
			printf("remove file: %s OK.\n\n\n", file_LNS);
		else
			printf("remove file: %s ERROR.\n\n\n", file_LNS);
	}
	else
	{
		printf(" file: %s not exist!\n\n", file_LNS);
	}

#if 1
	//起状态文件接收线程
	uintptr_t hStdOutStr = _beginthread(getStatusFile, 0, dest_ip_addr);

	if (hStdOutStr == -1) {
		printf("getStatusFile ThreadStatus error!!\n\n");
		*err_code = STATUS_THREAD_FAILED_CODE;
		//printf("err msg: %s.\n", err_msg);
		//memcpy(err_msg, STATUS_THREAD_FAILED_MSG, strlen(STATUS_THREAD_FAILED_MSG));
		return A615_ERROR;
	}
#endif

	//阻塞等待，保证接收到状态文件
	while (1)
	{
		access_ret = _access(file_LNS, 0);
		//阻塞等待LUS文件生成成功
		printf("wait LUS file!\n\n");
		if (0 == access_ret)
		{
			//状态文件存在，解析LNS状态文件
			analysisRet = parsing_LNS(client_id, &lns);
			if (0 == analysisRet)
			{
				//解析成功，执行回调函数
				strLNS2a615_status(lns, &stru_status);

				if (lns.op_stat_code == OPT_ABTED_DUETO_INIT ||
					lns.op_stat_code == OPT_ABTED_DUETO_DATALOADER ||
					lns.op_stat_code == OPT_ABT_BY_TAR)
				{
					//Abort操作
					printf("recv A ABORT STATUS! ABORT mediaDownload OPERATION!\n\n");
					WaitForSingleObject(mutex, 100);
					global_abort_code = 0;
					backflag = 0;
					ReleaseMutex(mutex);
					*err_code = A615_ABORT_CODE;
					return A615_ABORT_CODE;
				}

				fptr(input, &stru_status);
				//会在解析函数中malloc空间，在此释放
				free(stru_status.load_files);
			}
			else
			{
				//解析失败
				printf("Parse LNS error!\n");
				*err_code = A615_OPT_ERROR21;
				//printf("err msg: %s.\n", err_msg);
				return A615_ERROR;
			}

			free(lns.LNS_files);
		}
		else
		{
			wait_status_times++;

			if (wait_status_times > MAX_STATUS_TIMEOUT)
			{
				//解析失败
				printf("Parse LNS error!\n");
				*err_code = A615_OPT_ERROR21;
				//printf("err msg: %s.\n", err_msg);
				wait_status_times = 0;
				return A615_ERROR;
			}

			Sleep(STATUS_TIMEOUT);
			continue;
		}
	}

	

	//生成LNR文件

	tmp_lnr = create_LNR(client_id, file_list.file_num, tmp_file_list);

	if (tmp_lnr != 0)
	{
		*err_code = A615_OPT_ERROR12;
		printf("err msg: create LNR file ERROR.\n");
		return A615_ERROR;
	}

	//列表传输阶段
	retVal = mediaDownloadList(client_id, dest_ip_addr, err_code, err_msg);

	if (bool_t::ARINC615_FALSE == retVal)
	{
		//列表传输阶段未执行成功
		*err_code = A615_OPT_ERROR12;
		printf("err msg: %s.\n", err_msg);
		return A615_ERROR;
	}

	//起数据文件接收线程
	uintptr_t hStdOut = _beginthread(mediaDownloadFile, 0, client_id);

	if (hStdOut == -1) {
		printf("mediaDownloadFile Thread file error!!\n");
		*err_code = A615_THREAD_ERROR08;
		memcpy(err_msg, STATUS_THREAD_FAILED_MSG, strlen(STATUS_THREAD_FAILED_MSG));
		printf("err msg: %s.\n", err_msg);
		return A615_ERROR;
	}

	char oldLNS[FILE_PATH_BUFFER_SIZE] = { 0 };
	sprintf(oldLNS, "%s\\%s%s", getFolderPath, client_id, ".LNS");
	if (remove(oldLNS) == 0)
	{
		printf("mediadowntransfer: Remov file: %s.\n", oldLNS);
	}
	else
	{
		printf("mediadowntransfer: Remov file: %s error.\n", oldLNS);
	}

	memset(&filelns, 0, sizeof(struct File_LNS));

	//解析状态文件死循环
	//Sleep(1000);
	while (1) {
        //break_num2++;
        //if(break_num2 >= 100)
        //{
        //    break;
        //}
		Sleep(ANALYSIS_STATUS_FILE_PERIOD);
		lnsPhaseRet = parsing_LNS(client_id, &filelns);

        //printf("mediaDownload file trans status code: 0x%x.\n\n\n", filelns.op_stat_code);
        if(filelns.op_stat_code == OPT_ABTED_DUETO_INIT ||
				filelns.op_stat_code == OPT_ABTED_DUETO_DATALOADER ||
				filelns.op_stat_code == OPT_ABT_BY_TAR)
        {
            printf("recv A ABORT STATUS! ABORT OPERATION!");
            WaitForSingleObject(mutex, 100);
            global_abort_code = 0;
            backflag = 0;
            ReleaseMutex(mutex);
            *err_code = A615_ABORT_CODE;
            free(filelns.LNS_files);
			return A615_ABORT_CODE;
        }

		if (-1 == lnsPhaseRet) {
			printf("mediadowntransfer parsing LNS error, lns file not exist.\n");

            break_num++;
            if(break_num >= DELAY_TIME_TMP)
            {
                free(filelns.LNS_files);
                break;
            }
            free(filelns.LNS_files);
			continue;
		}

		int rate_ = (filelns.down_list_ratio[0] - '0') * 100
			+ (filelns.down_list_ratio[1] - '0') * 10
			+ (filelns.down_list_ratio[2] - '0') * 1;

		if (rate_ >= 0)
		{
			//执行回调函数
			//strLNS2a615_status(lns, &stru_status);
			strLNS2a615_status(filelns, &stru_status);
#if 0
            //2022-7-12
			if (stru_status.status_code == OPT_ABTED_DUETO_INIT ||
				stru_status.status_code == OPT_ABTED_DUETO_DATALOADER ||
				stru_status.status_code == OPT_ABT_BY_TAR)
			{
				//接收到终止码，停止upload操作
				//global_abort_code = AOORT_CODE;

                return stru_status.status_code;
			}
#endif

			fptr(input, &stru_status);

			free(stru_status.load_files);
		}

		if (filelns.op_stat_code == OPERATION_FINISH_CODE && abs(rate_ - A_HUNDRED_PERCENT) < MIN_NUMBER_ERROR)
		{
			WaitForSingleObject(mutex, 100);
			backflag = 0;
			ReleaseMutex(mutex);
			//结束循环
            free(filelns.LNS_files);
			break;
		}
        free(filelns.LNS_files);
	}

    
	return retVal;
}

//bool_t arinc615_operator_download_op(a615_status_func fptr, uint8_t stage, char *client_ID,
//	uint16_t* err_code, uint8_t* err_msg, file_download *file_list)
int arinc615_operator_download_op(void *input, a615_status_t_callback_func_t fptr, char *client_id,
	char *dest_ip_addr, uint8_t stage, int16_t *err_code, a615_down_file_list_t *file_list)
{
	int retVal = A615_SUCC;

	bool_t func_ret = bool_t::ARINC615_TRUE;

	struct File_LNS lns;
	struct File_LNL lnl;
	FileList filelist;
	int lnsRet = 0;

    request_pck_timeout = 0;

	uint8_t err_msg[ERROR_MESSAGE_BUFFER] = { 0 };

	a615_status_t stru_status;

	memset(&stru_status, 0, sizeof(a615_status_t));
	memset(&lns, 0, sizeof(struct File_LNS));
	memset(&lnl, 0, sizeof(struct File_LNL));

	//初始化错误码和错误信息
	*err_code = A615_SUCC;
	//memcpy(err_msg, EXEC_SUCCEED_MSG, strlen(EXEC_SUCCEED_MSG));

	if (OP_DOWNLOAD_INIT_STAGE == stage)
	{
		//初始化阶段
		func_ret = oDownloadInit(client_id, dest_ip_addr, err_code, err_msg);

		if (bool_t::ARINC615_FALSE == func_ret)
		{
			*err_code = A615_OPT_ERROR17;
			printf("err msg: %s.\n", err_msg);
			return A615_ERROR;
		}
		//解析一次状态文件

#if 1
		//起状态文件接收线程
		uintptr_t hStdOutStr = _beginthread(getStatusFile, 0, dest_ip_addr);

		if (hStdOutStr == -1) {
			printf("getStatusFile ThreadStatus error!!\n\n");
			*err_code = STATUS_THREAD_FAILED_CODE;

			return A615_ERROR;
		}
#endif

		Sleep(WAIT_RECV_STATUS_FILE_TIME);    //等待LNS文件

		lnsRet = parsing_LNS(client_id, &lns);

		if (0 != lnsRet)
		{
			*err_code = STATUS_FILE_ANA_ERR_CODE;
			memcpy(err_msg, STATUS_FILE_ANA_ERR_MSG, strlen(STATUS_FILE_ANA_ERR_MSG));
            free(lns.LNS_files);
			return bool_t::ARINC615_FALSE;
		}
		else
		{
			//执行回调函数
			strLNS2a615_status(lns, &stru_status);

			if (stru_status.status_code == OPT_ABTED_DUETO_INIT ||
				stru_status.status_code == OPT_ABTED_DUETO_DATALOADER ||
				stru_status.status_code == OPT_ABT_BY_TAR)
			{
				//接收到终止码，停止操作
				//global_abort_code = AOORT_CODE;

                printf("recv A ABORT STATUS! ABORT operatorDownload step 1 OPERATION!");
#if 0
                WaitForSingleObject(mutex, 100);
                global_abort_code = 0;
                backflag = 0;
                ReleaseMutex(mutex);
#endif
                *err_code = A615_ABORT_CODE;
                free(lns.LNS_files);
			    return A615_ABORT_CODE;
			}

			fptr(input, &stru_status);
            free(lns.LNS_files);
			free(stru_status.load_files);
		}

		//列表传输阶段,在此拿到一个文件列表，需要传出
		func_ret = oDownloadList(dest_ip_addr, err_code, err_msg);

		if (bool_t::ARINC615_FALSE == func_ret)
		{
			*err_code = A615_OPT_ERROR18;
			printf("err msg: %s.\n", err_msg);
			return A615_ERROR;
		}
		//解析LNL文件,并将结果写入file_list

		lnl = parsing_LNL(client_id);

		//申请存储空间
		//file_list->file_list = (file_info *)malloc(sizeof(file_info) * lnl.file_num);

		//深拷贝
		strLNL2file_download(lnl, file_list);

        free(lnl.file);
		return retVal;
	}
	else
	{
		//创建LNA文件，从file_list读出下载的文件列表
		file_download2FileList(*file_list, &filelist);

        request_pck_timeout = 0;

		create_LNA(client_id, file_list->file_num, filelist);

		//LNA文件传输
		func_ret = transLNAFile(client_id, dest_ip_addr, err_code, err_msg);

		if (bool_t::ARINC615_FALSE == func_ret)
		{
			*err_code = A615_OPT_ERROR19;
			printf("err msg: %s.\n", err_msg);
			return A615_ERROR;
		}
#if 0
        //起状态文件接收线程
		uintptr_t hStdOutStr = _beginthread(getStatusFile, 0, dest_ip_addr);

		if (hStdOutStr == -1) {
			printf("getStatusFile ThreadStatus error!!\n\n");
			*err_code = STATUS_THREAD_FAILED_CODE;

			return A615_ERROR;
		}
        else
        {
            printf("operatorDownloadFile status thread running!\n\n");
        }
#endif
		//数据传输阶段
		uintptr_t hStdOut = _beginthread(oDownloadFile, 0, client_id);

		if (hStdOut == -1) {
			printf("operatorDownloadFile Thread file error!!\n");
			*err_code = A615_THREAD_ERROR08;
			memcpy(err_msg, STATUS_THREAD_FAILED_MSG, strlen(STATUS_THREAD_FAILED_MSG));
			printf("err msg: %s.\n", err_msg);
			return A615_ERROR;
		}
        else
        {
            printf("operatorDownloadFile step 2 thread running!\n\n");
        }

		//解析状态文件
		while (1) {
			Sleep(ANALYSIS_STATUS_FILE_PERIOD);

			lnsRet = parsing_LNS(client_id, &lns);

			//if (-1 == lnsRet) {
			//	printf("opdowntransfer parsing_LNS err! no LNS file!");
			//	continue;
			//}
            //printf("OP Download file trans status code: 0x%x.\n\n\n", lns.op_stat_code);

            if(1 == request_pck_timeout)
            {
                //printf("L*S thread ID: 0x%x.\n\n", GetCurrentThread());
                printf("because TIMEOUT, OD STOP!\n");
                *err_code = A615_ERROR;
                free(lns.LNS_files);
                return A615_ERROR;
            }
            if(lns.op_stat_code == OPT_ABTED_DUETO_INIT ||
				lns.op_stat_code == OPT_ABTED_DUETO_DATALOADER ||
				lns.op_stat_code == OPT_ABT_BY_TAR)
            {
                printf("recv A ABORT STATUS! ABORT operatorDownload step 2 OPERATION!");
                WaitForSingleObject(mutex, 100);
                global_abort_code = 0;
                backflag = 0;
                *err_code = A615_ABORT_CODE;
                ReleaseMutex(mutex);
                *err_code = A615_ABORT_CODE;
                free(lns.LNS_files);
			    return A615_ABORT_CODE;
            }

			int rate_ = (lns.down_list_ratio[0] - '0') * 100
				+ (lns.down_list_ratio[1] - '0') * 10
				+ (lns.down_list_ratio[2] - '0');
			if (rate_ >= 0)
			{
				//执行回调函数
				strLNS2a615_status(lns, &stru_status);

                printf("\nafter fptr run, ratio:%s\n\n", stru_status.load_ratio);

				fptr(input, &stru_status);

				free(stru_status.load_files);
			}

			if ((lns.op_stat_code == OPERATION_FINISH_CODE) || (abs(rate_ - A_HUNDRED_PERCENT) < MIN_NUMBER_ERROR))
			{
				WaitForSingleObject(mutex, 100);
				backflag = 0;
				ReleaseMutex(mutex);
                *err_code = A615_SUCC;
                free(lns.LNS_files);
				return A615_SUCC;
			}
            free(lns.LNS_files);
		}

	}
    printf("main oDownloadFile thread dead!\n\n");
	return A615_SUCC;
}

uint32_t a615_exit(uint16_t net_type, int16_t *err_code)
{
	int retVal = A615_SUCC;
	int ret_val = 0;

	*err_code = A615_SUCC;
	//memcpy(err_msg, EXEC_SUCCEED_MSG, strlen(EXEC_SUCCEED_MSG));

	//if (net_type == AFDX_NETWORK)
	if (net_type == AFDX_NET)
	{
#if AFDX_NETWORK

#endif
	}
	else if (net_type == SOCKET_NET)
	{
		//*err_code = A615_INIT_ERROR02;

		close_socket_source();
	}
	else
	{
		*err_code = A615_INIT_ERROR01;
		retVal = A615_ERROR;
	}
	return retVal;
}

int arinc615_abort_op(char abort_code)
{
    global_abort_code = abort_code;
	//return send_abort_packet(dest_ip_addr, abort_code, err_code);

    //*err_code = abort_code;

    printf("arinc615_abort_op-> global_abort_code = 0x%c", global_abort_code);

    return 0;
}

uint32_t arinc615_get_part_ver(void)
{
	uint32 part_ver;

	part_ver = ARINC615_PART_VER;

	return part_ver;
}

