#include "canInit.h"
#include "serialSend.h"

unsigned char calc_checksum(unsigned char *can_data)
{
	unsigned int len = ((unsigned int) can_data[2]) << 8 | can_data[3];
	unsigned int check = 0, index = 0;

	len += 4;
	printf("len=%x\n", len);
	printf("orig check=%x\n", can_data[len-1]);
	can_data[len-1] = 0;
	for (index=2; index<len; index++) {
		check += can_data[index];
	}
	printf("check=%x\n", check);
	can_data[len-1] = (unsigned char)(check & 0xFF);
	return (unsigned char)(check & 0xFF);
}

void can_send_raw(int fd, unsigned char *raw_data, unsigned int len)
{
	unsigned char can_header[0x20];

	unsigned int pack_len;
	len = 8;
	pack_len = len + 8;

	can_header[0] = 0x66;
	can_header[1] = 0xCC;
	can_header[2] = (unsigned char)(pack_len >> 8);
	can_header[3] = (unsigned char)(pack_len & 0xFF);
	can_header[4] = 0x30;
	can_header[5] = 0x03;	//Type ID
	can_header[6] = 0x00;
	can_header[7] = 0x00;
	can_header[8] = 0x04;
	can_header[9] = 0xF7;
	can_header[0xA] = (unsigned char)(len & 0xFF); 

	can_header[0xb] = 0x0D;
	can_header[0xc] = 0x0E;
	can_header[0xd] = 0x0D;
	can_header[0xe] = 0x0E;
	can_header[0xf] = 0x0D;
	can_header[0x10] = 0x0E;
	can_header[0x11] = 0x0F;
	can_header[0x12] = 0x01;

	can_header[0x13] = 0x0a; // checksum
	
	calc_checksum(can_header);
}
void can_query_hw_version(int fd)
{
	bool isBaudRateSet = false;
	bool isTransferSet = false;

	//设置波特率参数
	unsigned char setup_baudrate_send[8];
	setup_baudrate_send[0]=0x66;
	setup_baudrate_send[1]=0xCC;
	setup_baudrate_send[2]=0x00;
	setup_baudrate_send[3]=0x04;
	setup_baudrate_send[4]=0x12;
	setup_baudrate_send[5]=0x01;
	setup_baudrate_send[6]=0x32;
	setup_baudrate_send[7]=0x49;


	unsigned char setup_baudrate_recv[7];
	for(int i=0;i<7;i++)
	{
		setup_baudrate_recv[i] = 1;
	}


	//设置透传参数
	unsigned char setup_transfer_send[13];
	setup_transfer_send[0]=0x66;
	setup_transfer_send[1]=0xCC;
	setup_transfer_send[2]=0x00;
	setup_transfer_send[3]=0x09;
	setup_transfer_send[4]=0x16;
	setup_transfer_send[5]=0x02;
	setup_transfer_send[6]=0x18;
	setup_transfer_send[7]=0xFF;
	setup_transfer_send[8]=0x15;
	setup_transfer_send[9]=0x07;
	setup_transfer_send[10]=0x08;
	setup_transfer_send[11]=0x01;
	setup_transfer_send[12]=0x5D;

	unsigned char setup_transfer_recv[7];
	for(int i=0;i<7;i++)
	{
		setup_baudrate_recv[i] = 1;
	}

#if !OFFLINE_TEST
	while(true)
	{
		if(isTransferSet) break;
		//设置波特率
		while(true)
		{
			serial_send(fd,setup_baudrate_send,8);
			usleep(500000);
			if(0 == serial_recv(fd,setup_baudrate_recv,7))
			{
				printf("0: %2x \n",setup_baudrate_recv[0]);
				printf("1: %2x \n",setup_baudrate_recv[1]);
				printf("2: %2x \n",setup_baudrate_recv[2]);
				printf("3: %2x \n",setup_baudrate_recv[3]);
				printf("4: %2x \n",setup_baudrate_recv[4]);
				printf("5: %2x \n",setup_baudrate_recv[5]);
				printf("6: %2x \n",setup_baudrate_recv[6]);

				if(0 == setup_baudrate_recv[5])
				{
					cout<<"baud rate setup success,resp is "<<endl;
					isBaudRateSet = true;
					break;
				}
				else
					cout<<"baud rate setup fail"<<endl;

			}
			else 
				cout<<"rec fail"<<endl;
		}
		
		if(isBaudRateSet)
		{
			//设置透传参数
			while(true)
			{
				serial_send(fd,setup_transfer_send,13);
				usleep(500000);
				if(0 == serial_recv(fd,setup_transfer_recv,7))
				{
					printf("0: %2x \n",setup_transfer_recv[0]);
					printf("1: %2x \n",setup_transfer_recv[1]);
					printf("2: %2x \n",setup_transfer_recv[2]);
					printf("3: %2x \n",setup_transfer_recv[3]);
					printf("4: %2x \n",setup_transfer_recv[4]);
					printf("5: %2x \n",setup_transfer_recv[5]);
					printf("6: %2x \n",setup_transfer_recv[6]);

					if(0 == setup_transfer_recv[5])
					{
						cout<<"tranfer param setup success!!!"<<endl;
						isTransferSet = true;
						break;
					}
					else
						cout<<"transfer param setup fail"<<endl;
	
				}
				else 
					cout<<"rec fail"<<endl;
			}
		}
	
	}   
#endif

}
void InitCanBus(int fd)
{
	bool isBaudRateSet = false;
	bool isTransferSet = false;

	//设置波特率参数
	unsigned char setup_baudrate_send[8];
	setup_baudrate_send[0]=0x66;
	setup_baudrate_send[1]=0xCC;
	setup_baudrate_send[2]=0x00;
	setup_baudrate_send[3]=0x04;
	setup_baudrate_send[4]=0x12;
	setup_baudrate_send[5]=0x01;
	//20k,50k,100k,125k,200k,250k,400k,500k,600k,800k,1000k
	setup_baudrate_send[6]=0x64; //100k
	setup_baudrate_send[7]=0x49;

	calc_checksum(setup_baudrate_send);

	unsigned char setup_baudrate_recv[7];
	for(int i=0;i<7;i++)
	{
		setup_baudrate_recv[i] = 1;
	}


	//设置透传参数
	unsigned char setup_transfer_send[13];
	setup_transfer_send[0]=0x66;
	setup_transfer_send[1]=0xCC;
	setup_transfer_send[2]=0x00;
	setup_transfer_send[3]=0x09;
	setup_transfer_send[4]=0x16;
	setup_transfer_send[5]=0x02;
	setup_transfer_send[6]=0x18;
	setup_transfer_send[7]=0xFF;
	setup_transfer_send[8]=0x15;
	setup_transfer_send[9]=0x07;
	setup_transfer_send[10]=0x08;
	setup_transfer_send[11]=0x01;
	setup_transfer_send[12]=0x5D;

	calc_checksum(setup_transfer_send);
	unsigned char setup_transfer_recv[7];
	for(int i=0;i<7;i++)
	{
		setup_baudrate_recv[i] = 1;
	}
#if !OFFLINE_TEST
	while(true)
	{
		if(isBaudRateSet) break;
		//设置波特率
		while(true)
		{
			serial_send(fd,setup_baudrate_send,8);
			usleep(500000);
			if(0 == serial_recv(fd,setup_baudrate_recv,7))
			{
				printf("0: %2x \n",setup_baudrate_recv[0]);
				printf("1: %2x \n",setup_baudrate_recv[1]);
				printf("2: %2x \n",setup_baudrate_recv[2]);
				printf("3: %2x \n",setup_baudrate_recv[3]);
				printf("4: %2x \n",setup_baudrate_recv[4]);
				printf("5: %2x \n",setup_baudrate_recv[5]);
				printf("6: %2x \n",setup_baudrate_recv[6]);

				if(0 == setup_baudrate_recv[5])
				{
					cout<<"baud rate setup success,resp is "<<endl;
					isBaudRateSet = true;
					break;
				}
				else
					cout<<"baud rate setup fail"<<endl;

			}
			else 
				cout<<"rec fail"<<endl;
		}
#if 0	
		if(isBaudRateSet)
		{
			//设置透传参数
			while(true)
			{
				serial_send(fd,setup_transfer_send,13);
				usleep(500000);
				if(0 == serial_recv(fd,setup_transfer_recv,7))
				{
					printf("0: %2x \n",setup_transfer_recv[0]);
					printf("1: %2x \n",setup_transfer_recv[1]);
					printf("2: %2x \n",setup_transfer_recv[2]);
					printf("3: %2x \n",setup_transfer_recv[3]);
					printf("4: %2x \n",setup_transfer_recv[4]);
					printf("5: %2x \n",setup_transfer_recv[5]);
					printf("6: %2x \n",setup_transfer_recv[6]);

					if(0 == setup_transfer_recv[5])
					{
						cout<<"tranfer param setup success!!!"<<endl;
						isTransferSet = true;
						break;
					}
					else
						cout<<"transfer param setup fail"<<endl;
	
				}
				else 
					cout<<"rec fail"<<endl;
			}
		}
#endif
	}   

#endif
}
