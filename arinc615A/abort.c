#include "abort.h"
#include "tftpClient.h"



int create_abort_packet(char* pk, const char abort_code)
{
    int pck_len = 0;
	if (pk == NULL)
	{
		return -1;
	}

	if ((abort_code != '4') && (abort_code != '5'))
	{
		return -2;
	}

    pk[0] = 0x0;
	pk[1] = 0x5;
	pk[2] = 0x0;
    pk[3] = 0x0;
    pk[4] = 'A';
    pk[5] = 'B';
    pk[6] = 'O';
    pk[7] = 'R';
    pk[8] = 'T';
    pk[9] = ':';
	pk[10] = '1';
    pk[11] = '0';
    pk[12] = '0';
	pk[13] = abort_code;
    pk[14] = 0x0;

    pck_len = 15;

	return pck_len;
}

int send_abort_packet(SOCKET sockfd, NetAddr addrInfo, const char abort_code)
{
	char abort_pack[MAX_BUF_16_SIZE_T] = { 0 };
	int abort_pack_len = 0;
	int retval = 0;

	//∑¢ÀÕ”√	   2022-6-28 add
    /*
	operationNetAddr.cAddr.sin_family = AF_INET;
	operationNetAddr.cAddr.sin_port = htons(HOST_OPERATION_SEND_PORT);

	// add end
	if (dest_ip_addr != NULL)
	{
		operationNetAddr.cAddr.sin_addr.s_addr = inet_addr(dest_ip_addr);

		statusNetAddr.cAddr.sin_addr.s_addr = inet_addr(dest_ip_addr);
	}
	else
	{
		printf("dest ip is invalid!\n");
		*err_code = IP_ADDR_INVALID_ERR_CODE;
		return (-1);
	}
    */
	abort_pack_len = create_abort_packet(abort_pack, abort_code);

	retval = A615NetSendto(sockfd, abort_pack, addrInfo, abort_pack_len);

    printf("sent abort packet: len: %d, pck: %d.\n\n", abort_pack_len, retval);

	return retval;

}

