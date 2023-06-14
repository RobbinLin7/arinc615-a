#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


//#include "arinc615_func_define.h"
#include "arinc615_global_var_define.h"
#include "arinc615_struct_define.h"


//产生一个伪随机数
int get_rand_num(int range)
{
    if(range <= 0)
    {
        printf("illegal range!\n");

        return 0;
    }
    srand(time(0));

    return (rand() % range);
}


bool_t a615_status_print(a615_status_t* cb_status)
{
	int i = 0;
	bool_t retVal = bool_t::ARINC615_TRUE;

    printf("\n*********call back func running!********\n");
#if 1
	printf("client_id: %s;\n", cb_status->client_id);
	printf("counter: %d;\n", cb_status->counter);
	//printf("description: %s;\n", cb_status->description);
	printf("load_ratio: %s;\n", cb_status->load_ratio);
	printf("num_of_files: %d;\n", cb_status->num_of_files);
	printf("status_code: %d;\n", cb_status->status_code);

	for (i = 0; i < cb_status->num_of_files; i++)
	{
		//printf("--description: %s;\n", cb_status->load_files[i].file_name);
		//printf("--file_name: %s;\n", cb_status->load_files[i].file_name);
		printf("--status: %d;\n", cb_status->load_files[i].file_status);
	}
#endif
	return retVal;
}

void printInformation(a615_target_info_t info)
{
	printf("file_len = %d.\n", info.file_len);
	printf("Hw_num = %d.\n", info.hw_num);
	printf("Pro_ver = %d.\n", info.pro_ver);
}

int main(void)
{
	printf("for test.\n");

    int16_t err_code = 0;
    uint8_t err_msg[1024] = {0};

	int op_code = 0;

    int loop = 0;

	a615_targets_find_list_t *find_info = NULL;

    afdx_cfg_info network_info;

	a615_target_info_t *info_msg = NULL;

    char filename[256] = {0};

    char hd_id[32] = {0};

    int ret_val = 0;

    int range_num = 0;

    luh_info upload_file_list;

	a615_data_file_list_t data_up_list;

    a615_down_file_list_t down_file_list;

    a615_down_file_list_t od_file_list;

    //加载端IP 初始化
    network_info.laod_ip = 0x0a012101;
    //目标端IP初始化
    network_info.targets_num = 1;

    network_info.targets_list = (tar_cfg_info *)malloc(sizeof(tar_cfg_info) * network_info.targets_num);

    
    network_info.targets_list->tar_ip = 0x0a012102;
#if AFDX_NETWORK
    //发送VL ID初始化
    network_info.targets_list->tx_vl_info.vlid_num = 1;

    network_info.targets_list->tx_vl_info.vlid = (unsigned short *)malloc(sizeof(unsigned short)
        * network_info.targets_list->tx_vl_info.vlid_num);
    
    network_info.targets_list->tx_vl_info.vlid[0] = TX_VLID;

    //接收VL ID初始化
    network_info.targets_list->rx_vl_info.vlid_num = 1;

    network_info.targets_list->rx_vl_info.vlid = (unsigned short *)malloc(sizeof(unsigned short)
        * network_info.targets_list->rx_vl_info.vlid_num);
    
    network_info.targets_list->rx_vl_info.vlid[0] = RX_VLID;

    //发送端口初始化
    network_info.targets_list->tx_port_list.port_num = 4;

    network_info.targets_list->tx_port_list.port_list = (port_cfg_info *)malloc(sizeof(port_cfg_info)
        * network_info.targets_list->tx_port_list.port_num);

    network_info.targets_list->tx_port_list.port_list[0].port_id = FIND_TX_PORT;
    network_info.targets_list->tx_port_list.port_list[0].vlId = TX_VLID;

    network_info.targets_list->tx_port_list.port_list[1].port_id = CLIENT_TX_PORT;
    network_info.targets_list->tx_port_list.port_list[1].vlId = TX_VLID;

    network_info.targets_list->tx_port_list.port_list[2].port_id = SERVER_TX_PORT;
    network_info.targets_list->tx_port_list.port_list[2].vlId = TX_VLID;

    network_info.targets_list->tx_port_list.port_list[3].port_id = STATUS_TX_PORT;
    network_info.targets_list->tx_port_list.port_list[3].vlId = TX_VLID;

    //接收端口初始化
    network_info.targets_list->rx_port_list.port_num = 4;

    network_info.targets_list->rx_port_list.port_list = (port_cfg_info *)malloc(sizeof(port_cfg_info)
        * network_info.targets_list->rx_port_list.port_num);

    network_info.targets_list->rx_port_list.port_list[0].port_id = FIND_RX_PORT;
    network_info.targets_list->rx_port_list.port_list[0].vlId = RX_VLID;

    network_info.targets_list->rx_port_list.port_list[1].port_id = CLIENT_RX_PORT;
    network_info.targets_list->rx_port_list.port_list[1].vlId = RX_VLID;

    network_info.targets_list->rx_port_list.port_list[2].port_id = SERVER_RX_PORT;
    network_info.targets_list->rx_port_list.port_list[2].vlId = RX_VLID;

    network_info.targets_list->rx_port_list.port_list[3].port_id = STATUS_RX_PORT;
    network_info.targets_list->rx_port_list.port_list[3].vlId = RX_VLID;
#endif  
    //设备网络信息初始化
    a615_init(SOCKET_NET, &network_info, &err_code);

    printf("tips:\n");

    printf("\t--1--Find\n\t--2--Information\n\t--3--upload\n\t--4--Mdownload\n\t--5--Odownload(Step 1)\n\t--6--Odownload(Step 2)\n\t--0--quit\n");

	printf("input operation code: ");
	scanf("%d", &op_code);
	while (1)
	{
		switch (op_code)
		{
		case 1:
			printf("Find.\n");

            //find_info = arinc615_find_op(&err_code, err_msg);
			find_info = a615_find_op(&err_code);

            if(NULL == find_info)
            {
                

                printf("NOT Find target!\n");
            }
            else
            {
                sprintf(filename, "%s_%s", find_info->targets_info[0].identifier,
                    find_info->targets_info[0].position);

                //sprintf(hd_id, "%s", find_info->targets_info[0].identifier);

                printf("Target name: %s.\n", filename);

                printf("Find target!\n");
            }
			break;
		case 2:
			printf("Information.\n");

            //ret_val = arinc615_information_op(&a615_status_print, filename, info_msg, &err_code, err_msg);
			ret_val = a615_information_op(&a615_status_print, filename, info_msg, &err_code);

            if(A615_SUCC == ret_val)
            {
                printf("information finish.\n");
            }
            else
            {
                printf("information error.\n");
            }
			break;
		case 3:
			printf("Upload.\n");

            //初始化需要上传的文件列表

            memset(&upload_file_list, 0, sizeof(luh_info));

            //upload_file_list.file_list.file_number = 1;
			upload_file_list.file_list.file_num = 1;
            _snprintf(upload_file_list.head_file_name, strlen(filename) + 4, "%s%s", filename, ".LUH");
            for(loop = 0; ; loop++)
            {
                upload_file_list.target_hw_ID[loop] = filename[loop];
                if(filename[loop] == '_')
                {
                    upload_file_list.target_hw_ID[loop] = '\0';
                    break;
                }
            }

            printf("Upload hw id: %s.\n", upload_file_list.target_hw_ID);

            sprintf(upload_file_list.target_hw_ID, "%s", hd_id);
            upload_file_list.load_PN = 2;
            upload_file_list.load_type_description = 2;
            upload_file_list.file_list.data_file[0].file_len = (83411 + 1) / 2;
			//upload_file_list.file_list.data_file[0].file_len = (143 + 1) / 2;
            sprintf(upload_file_list.file_list.data_file[0].name, "%s", "thi12e.exe");
            
            upload_file_list.file_list.data_file[0].check_value_type = 1;

			data_up_list.file_num = 1;
			data_up_list.data_file[0].check_value_type = 1;
			//data_up_list.data_file[0].file_len = (143 + 1) / 2;
            data_up_list.data_file[0].file_len = (83411 + 1) / 2;

            memset(data_up_list.data_file[0].name, 0, sizeof(char) * FILE_NAME_MAX_LEN);
            memset(data_up_list.data_file[0].path, 0, sizeof(char) * FILE_NAME_MAX_LEN);
			//sprintf(data_up_list.data_file[0].name, "%s", "615A_dataloader.tmp");
            //sprintf(data_up_list.data_file[0].path, "%s", "E:/testUpload");
            //strcpy(data_up_list.data_file[0].name, "615A_dataloader.tmp");
            strcpy(data_up_list.data_file[0].name, "thi12e.exe");
            strcpy(data_up_list.data_file[0].path, "E:/testUpload");
			//_snprintf(data_up_list.data_file[0].path,
            //    strlen("F:\liuli\YW2006\dataloader_new\dataloader\615A_dataloader\Send"),
            //    "%s", "F:\liuli\YW2006\dataloader_new\dataloader\615A_dataloader\Send");

            //ret_val = arinc615_upload_op(&a615_status_print, filename, &err_code, err_msg, upload_file_list);
			ret_val = a615_upload_op(&a615_status_print, data_up_list, filename, &err_code);

            if(A615_SUCC == ret_val)
            {
                printf("\nupload OP finished.\n");
            }
            else
            {
                printf("\nupload OP ERROR! COED: %d\n", err_code);
            }

			break;
		case 4:
			printf("Mdownload.\n");

            //初始化传输列表
            down_file_list.file_num = 1;
            strcpy(down_file_list.file_name[0], "TTT-SWITCH-LAB_GROUND.LCL");

            ret_val = arinc615_media_download_op(&a615_status_print, filename, down_file_list, &err_code);

			break;
		case 5:
			printf("Odownload, Step 1:\n");
            ret_val = arinc615_operator_download_op(&a615_status_print, filename,
                OP_DOWNLOAD_INIT_STAGE, &err_code, &down_file_list);
            if(A615_SUCC == ret_val)
            {
                printf("O Download file list:\n");

                for(loop = 0; loop < down_file_list.file_num; loop++)
                {
                    printf("file [%d] name: %s\n", loop + 1, down_file_list.file_name[loop]);
                }
            }
            else
            {
                printf("Odownload, Step 1 ERROR!\n");
            }
			break;
        case 6:
			printf("Odownload, Step 2:\n");

            range_num = get_rand_num(down_file_list.file_num);
            od_file_list.file_num = 1;
            //strcpy(od_file_list.file_name[0], down_file_list.file_name[range_num]);
            strcpy(od_file_list.file_name[0], down_file_list.file_name[0]);

            printf("No %d of %d, OD file name: %s.\n",range_num, down_file_list.file_num, od_file_list.file_name[0]);
            ret_val = arinc615_operator_download_op(&a615_status_print, filename,
                OP_DOWNLOAD_DATA_STAGE, &err_code, &od_file_list);
            if(A615_SUCC == ret_val)
            {
                printf("Odownload, Step 2 finished!\n");
            }
            else
            {
                printf("Odownload, Step 2 ERROR!\n");
            }
			break;
		case 0:
			a615_exit(SOCKET_NET, &err_code);
			printf("Exit.\n");
			return 0;
		default:
			printf("code illegal! input again!\n");
            printf("\t--1--Find\n\t--2--Information\n\t--3--upload\n\t--4--Mdownload\n\t--5--Odownload\n\t--0--quit\n");
			break;
		}
		printf("input operation code: ");
		scanf("%d", &op_code);
	}
	printf("END OF RUN.\n");

	system("pause");
	return 0;
}