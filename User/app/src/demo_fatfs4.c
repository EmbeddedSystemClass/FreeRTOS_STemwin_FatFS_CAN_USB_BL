/*
*********************************************************************************************************
*
*	ģ������ : U��Fat�ļ�ϵͳ��ʾģ�顣
*	�ļ����� : demo_fatfs.c
*	��    �� : V1.0
*	˵    �� : ��������ֲFatFS�ļ�ϵͳ���汾 R0.11a������ʾ��δ����ļ�����ȡ�ļ�������Ŀ¼��ɾ���ļ�
*			���������ļ���д�ٶȡ�
*           ʹ��˵����
*           1. ����̷�������ʱ�����ʱ�̱�֤�����̷��ţ�Ҫ�������޷���ȷʶ��ֻ��һ���̷���ʱ�����
*              �����̷��š�
*           2. ֧���Ȳ�Ρ�
*
*	�޸ļ�¼ :
*		�汾��   ����        ����       ˵��
*		V1.0    2016-02-27 	 Eric2013  ��ʽ����
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "ff.h"			/* FatFS�ļ�ϵͳģ��*/
#include "usbh_bsp_msc.h"
#include "demo_fatfs.h"

#define name_to_str(name_struct)  (#name_struct)

//extern char usb_flag;
uint8_t max_pro_num = 0;
char cfgname[4][16] = {0};
uint8_t jiexiwan_flag=0;
CONFIG_PARA cfg_para;
struct _CONFIG_CHANNEL_ cfg_ch_tmp;
struct _CONFIG_DATA cfg_data_tmp;
//CONFIG_NAME cfgname;

cfgnameFIL MyFiles;
extern DIR DirInf_usb;

/* �������ļ��ڵ��õĺ������� */
static void CreateNewFile(uint8_t pronum,uint8_t picnum);
static void DeleteDirFile(char *_ucVolume);
static void MSC_App3(int pronum);

/* FatFs API�ķ���ֵ */
static const char * FR_Table[]= 
{
	"FR_OK���ɹ�",				                             /* (0) Succeeded */
	"FR_DISK_ERR���ײ�Ӳ������",			                 /* (1) A hard error occurred in the low level disk I/O layer */
	"FR_INT_ERR������ʧ��",				                     /* (2) Assertion failed */
	"FR_NOT_READY����������û�й���",			             /* (3) The physical drive cannot work */
	"FR_NO_FILE���ļ�������",				                 /* (4) Could not find the file */
	"FR_NO_PATH��·��������",				                 /* (5) Could not find the path */
	"FR_INVALID_NAME����Ч�ļ���",		                     /* (6) The path name format is invalid */
//	"FR_DENIED�����ڽ�ֹ���ʻ���Ŀ¼�������ʱ��ܾ�",         /* (7) Access denied due to prohibited access or directory full */
//	"FR_EXIST���ļ��Ѿ�����",			                     /* (8) Access denied due to prohibited access */
//	"FR_INVALID_OBJECT���ļ�����Ŀ¼������Ч",		         /* (9) The file/directory object is invalid */
//	"FR_WRITE_PROTECTED������������д����",		             /* (10) The physical drive is write protected */
//	"FR_INVALID_DRIVE���߼���������Ч",		                 /* (11) The logical drive number is invalid */
//	"FR_NOT_ENABLED�������޹�����",			                 /* (12) The volume has no work area */
//	"FR_NO_FILESYSTEM��û����Ч��FAT��",		             /* (13) There is no valid FAT volume */
//	"FR_MKFS_ABORTED�����ڲ�������f_mkfs()����ֹ",	         /* (14) The f_mkfs() aborted due to any parameter error */
//	"FR_TIMEOUT���ڹ涨��ʱ�����޷���÷��ʾ�����",		 /* (15) Could not get a grant to access the volume within defined period */
//	"FR_LOCKED�������ļ�������Բ������ܾ�",				 /* (16) The operation is rejected according to the file sharing policy */
//	"FR_NOT_ENOUGH_CORE���޷����䳤�ļ���������",		     /* (17) LFN working buffer could not be allocated */
//	"FR_TOO_MANY_OPEN_FILES����ǰ�򿪵��ļ�������_FS_SHARE", /* (18) Number of open files > _FS_SHARE */
//	"FR_INVALID_PARAMETER��������Ч"	                     /* (19) Given parameter is invalid */
};

/*
*********************************************************************************************************
*	�� �� ��: DemoFatFS
*	����˵��: FatFS�ļ�ϵͳ��ʾ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DemoFatFS(void)
{
	FRESULT result;
	FATFS fs;
	DIR DirInf;
//	FILINFO FileInf;
	char path[20]={0};
	uint32_t i;
//	uint32_t cnt = 0;
//	char lfname[32];

	/* ��ȡ��ǰ�ļ����µ��ļ���Ŀ¼ */
//	FileInf.lfname = lfname;
//	FileInf.lfsize = 32;

//	for (i = 0; ;i++)
//	{
//		result = f_readdir(&DirInf_usb,&FileInf); 		/* ��ȡĿ¼��������Զ����� */
//		if (result != FR_OK || FileInf.fname[0] == 0)
//		{
//			break;
//		}

//		if (FileInf.fname[0] == '.')
//		{
//			continue;
//		}

//		/* �ж����ļ�������Ŀ¼ */
//		if (!(FileInf.fattrib & AM_DIR))
//		{
//			if(strstr(FileInf.fname,".txt"))
//			{
//				strcpy(cfgname[cnt],FileInf.fname);
//				max_pro_num = ++cnt;
//			}
//		}
//	}
		
			max_pro_num = ViewRootDir(FS_VOLUME_USB,cfgname);
	
			DeleteDirFile(FS_VOLUME_SD);
//			CreateNewFile(0,0,1);

			result = f_mount(&fs, FS_VOLUME_USB, 0);			/* Mount a logical drive */
			if (result != FR_OK)
			{
				printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
			}

			sprintf(path, "%s/", FS_VOLUME_USB);
			result = f_opendir(&DirInf, path); /* ���������������ӵ�ǰĿ¼��ʼ */
			if (result != FR_OK)
			{
				printf("�򿪸�Ŀ¼ʧ��  (%s)\r\n", FR_Table[result]);
				return;
			}
		
			for(i = 0;i < max_pro_num;i++)
			{
				sprintf(path, "%s/%s", FS_VOLUME_USB,cfgname[i]);
				result = f_open(&MyFiles, path, FA_OPEN_EXISTING | FA_READ);
				if (result !=  FR_OK)
				{
					printf("Don't Find File : %s\r\n",path);
					return;
				}
				
				MSC_App3(i);

				/* �ر��ļ�*/
				f_close(&MyFiles);
			}
				
			/* ж���ļ�ϵͳ */
			f_mount(NULL, FS_VOLUME_USB, 0);
	
		vTaskDelay(10);
}

/*
*********************************************************************************************************
*	�� �� ��: ViewRootDir
*	����˵��: ��ʾSD����Ŀ¼�µ��ļ���
*	��    ��: _ucVolume : ���̾�� 0 - 2
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int ViewRootDir(char *_ucVolume,char buf[][16])
{
	/* ������ʹ�õľֲ�����ռ�ý϶࣬���޸������ļ�����֤��ջ�ռ乻�� */
	FRESULT result;
	FATFS fs;
	DIR DirInf;
	FILINFO FileInf;
	uint32_t i,cnt = 0;
	char lfname[32];
	char path[32];

 	/* �����ļ�ϵͳ */
	result = f_mount(&fs, _ucVolume, 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%d)\r\n", result);
	}

	/* �򿪸��ļ��� */
	sprintf(path, "%s/", _ucVolume);	 /* 1: ��ʾ�̷� */
	result = f_opendir(&DirInf, path);
	if (result != FR_OK)
	{
		printf("�򿪸�Ŀ¼ʧ�� (%d)\r\n", result);
		return 0;
	}

	/* ��ȡ��ǰ�ļ����µ��ļ���Ŀ¼ */
	FileInf.lfname = lfname;
	FileInf.lfsize = 32;

	for (i = 0; ;i++)
	{
		result = f_readdir(&DirInf,&FileInf); 		/* ��ȡĿ¼��������Զ����� */
		if (result != FR_OK || FileInf.fname[0] == 0)
		{
			break;
		}

		if (FileInf.fname[0] == '.')
		{
			continue;
		}

		/* �ж����ļ�������Ŀ¼ */
		if (!(FileInf.fattrib & AM_DIR))
		{
			if(strstr(FileInf.fname,".txt"))
			{
//				if(flag==0)
//				{
					strcpy(buf[cnt],FileInf.fname);
					++cnt;
//				} else {
//					strcpy(lastname,FileInf.fname);
//					if(strncmp(lastname,FileInf.fname,4))
//					{
//						strcpy(cfgname[cnt],FileInf.fname);
//						++cnt;
//					}
//				}
			}
		}
	}

	/* ж���ļ�ϵͳ */
	f_mount(NULL,_ucVolume, 0);
	return cnt;
}

void MSC_App3(int pronum)
{	
	uint8_t i,pic_number=1;

	for(i=0;i<pic_number;i++)
	{
		analysisCfgFile();                                     //
		if(	jiexiwan_flag==1)                           
		{                                               
			jiexiwan_flag=0;
			CreateNewFile(pronum,i);		 
		}
		pic_number = cfg_para.picNum +2;
	}
}

void analysisCfgFile(void)
{	
	FRESULT result;
	int i=0,count=0;
	char flag = 1;
	uint32_t byteRead = 0;
	char buf[16]={0},para1[64]={0},para2[64]={0};

	for(;;){
		result = f_read(&MyFiles, &buf, 1, &byteRead);
		if(result  || (byteRead == 0) )
			break;
		
		if (strcmp(para1, "END") == 0)
		{
			jiexiwan_flag=1;
			count = 0;
			break;
		}
		
		if(flag)
		{
			if((buf[0] == ' ') || (buf[0] == '\n') || (buf[0] == '\r') || buf[0] == '#') 
			{
				continue;
			}
			else if(buf[0] == '=')
			{
				i = 0;
				flag = 0;
				continue;
			}
			else
			{
				para1[i++] = buf[0];
			}
		}else{
			if((buf[0] == ' ') || (buf[0] == ';'))
			{
				continue;
			}
			else if((buf[0] == '\n') || (buf[0] == '\r'))
			{
				i = 0;
				flag = 1;
				count++;
				strCompare(para1,para2,count);
				memset(para1,0,64);
				memset(para2,0,64);
			}
			else
			{
				para2[i++] = buf[0];
			}
		}			
	}
}

void strCompare( char *title, char* value,int len)
{	
	switch(len)
	{
		case 1:
			if(memcmp("picname",title,7) == 0)
				strncpy(cfg_para.picName,value,7);
			break;
			
		case 2:
			if(memcmp("picnum",title,6) == 0)
				cfg_para.picNum = atoi(value);
			break;
			
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
			zuCompare(title,value);
			break;
			
		case 15:
			if(memcmp("total_time",title,9) == 0)
				cfg_para.totalTime = atoi(value);
			break;
			
		default:
			chCompare(title,value);
			break;
	}		
}

void zuCompare(char * name,char * value)
{
	char num[2]={0},level[2]={0};
	strncpy(num,name+5,1);
	strncpy(level,name+7,1);
	
	switch(atoi(num))
	{
		case 1:
			if(memcmp("h",level,1)==0)
				cfg_para.cfg_ch_zu.ch_zu1_h = atoi(value);
			else
				cfg_para.cfg_ch_zu.ch_zu1_l = atoi(value);
			break;
		case 2:
			if(memcmp("h",level,1)==0)
				cfg_para.cfg_ch_zu.ch_zu2_h = atoi(value);
			else
				cfg_para.cfg_ch_zu.ch_zu2_l = atoi(value);
			break;
		case 3:
			if(memcmp("h",level,1)==0)
				cfg_para.cfg_ch_zu.ch_zu3_h = atoi(value);
			else
				cfg_para.cfg_ch_zu.ch_zu3_l = atoi(value);
			break;
		case 4:
			if(memcmp("h",level,1)==0)
				cfg_para.cfg_ch_zu.ch_zu4_h = atoi(value);
			else
				cfg_para.cfg_ch_zu.ch_zu4_l = atoi(value);
			break;
		case 5:
			if(memcmp("h",level,1)==0)
				cfg_para.cfg_ch_zu.ch_zu5_h = atoi(value);
			else
				cfg_para.cfg_ch_zu.ch_zu5_l = atoi(value);
			break;
		case 6:
			if(memcmp("h",level,1)==0)
				cfg_para.cfg_ch_zu.ch_zu6_h = atoi(value);
			else
				cfg_para.cfg_ch_zu.ch_zu6_l = atoi(value);
			break;
	}
}

void chCompare(char * name,char * value)
{
	int len=0;
	char channel[5],namedata[8];
	char chnum[3]={0},datanum[3]={0};
	str_split(channel,namedata,name,'_');
	
	strncpy(chnum,channel+2,2);
	len =strlen(namedata);
	
	if(len==3)
		cfg_ch_tmp.ch_num = atoi(value);
	else if(len==4)
		strncpy(cfg_ch_tmp.ch_name,value,7);
	else if(len==5){
		strncpy(datanum,namedata+4,2);
		switch(atoi(datanum))
		{
			case 0:
				cfg_data_tmp.ch_data1 = strtoul(value,0,0);
				break;
			case 1:
				cfg_data_tmp.ch_data2 = strtoul(value,0,0);
				break;
			case 2:
				cfg_data_tmp.ch_data3 = strtoul(value,0,0);
				break;
			case 3:
				cfg_data_tmp.ch_data4 = strtoul(value,0,0);
				break;
			case 4:
				cfg_data_tmp.ch_data5 = strtoul(value,0,0);
				break;
			case 5:
				cfg_data_tmp.ch_data6 = strtoul(value,0,0);
				break;
			case 6:
				cfg_data_tmp.ch_data7 = strtoul(value,0,0);
				break;
			case 7:
				cfg_data_tmp.ch_data8 = strtoul(value,0,0);
				break;
			
			case 8:
				cfg_data_tmp.ch_data9 = strtoul(value,0,0);
				break;
			case 9:
				cfg_data_tmp.ch_data10 = strtoul(value,0,0);
				break;
			case 10:
				cfg_data_tmp.ch_data11 = strtoul(value,0,0);
				break;
			case 11:
				cfg_data_tmp.ch_data12 = strtoul(value,0,0);
				break;
			case 12:
				cfg_data_tmp.ch_data13 = strtoul(value,0,0);
				break;
			case 13:
				cfg_data_tmp.ch_data14 = strtoul(value,0,0);
				break;
			case 14:
				cfg_data_tmp.ch_data15 = strtoul(value,0,0);
				break;
			case 15:
				cfg_data_tmp.ch_data16 = strtoul(value,0,0);
				break;
		}
	}
	cfg_ch_tmp.cfg_data = cfg_data_tmp;
	chSave(atoi(chnum));
}

void chSave(int num)
{
	switch(num)
	{
		case 0:
			cfg_para.cfg_ch.cfg_ch1 = cfg_ch_tmp;	break;
		case 1:
			cfg_para.cfg_ch.cfg_ch2 = cfg_ch_tmp;	break;
		case 2:
			cfg_para.cfg_ch.cfg_ch3 = cfg_ch_tmp;	break;
		case 3:
			cfg_para.cfg_ch.cfg_ch4 = cfg_ch_tmp;	break;
		case 4:
			cfg_para.cfg_ch.cfg_ch5 = cfg_ch_tmp;	break;
		case 5:
			cfg_para.cfg_ch.cfg_ch6 = cfg_ch_tmp;	break;
		case 6:
			cfg_para.cfg_ch.cfg_ch7 = cfg_ch_tmp;	break;
		case 7:
			cfg_para.cfg_ch.cfg_ch8 = cfg_ch_tmp;	break;
		case 8:
			cfg_para.cfg_ch.cfg_ch9 = cfg_ch_tmp;	break;
		case 9:
			cfg_para.cfg_ch.cfg_ch10 = cfg_ch_tmp;	break;
		
		case 10:
			cfg_para.cfg_ch.cfg_ch11 = cfg_ch_tmp;	break;
		case 11:
			cfg_para.cfg_ch.cfg_ch12 = cfg_ch_tmp;	break;
		case 12:
			cfg_para.cfg_ch.cfg_ch13 = cfg_ch_tmp;	break;
		case 13:
			cfg_para.cfg_ch.cfg_ch14 = cfg_ch_tmp;	break;
		case 14:
			cfg_para.cfg_ch.cfg_ch15 = cfg_ch_tmp;	break;
		case 15:
			cfg_para.cfg_ch.cfg_ch16 = cfg_ch_tmp;	break;
		case 16:
			cfg_para.cfg_ch.cfg_ch17 = cfg_ch_tmp;	break;
		case 17:
			cfg_para.cfg_ch.cfg_ch18 = cfg_ch_tmp;	break;
		case 18:
			cfg_para.cfg_ch.cfg_ch19 = cfg_ch_tmp;	break;
		case 19:
			cfg_para.cfg_ch.cfg_ch20 = cfg_ch_tmp;	break;
		
		case 20:
			cfg_para.cfg_ch.cfg_ch21 = cfg_ch_tmp;	break;
		case 21:
			cfg_para.cfg_ch.cfg_ch22 = cfg_ch_tmp;	break;
		case 22:
			cfg_para.cfg_ch.cfg_ch23 = cfg_ch_tmp;	break;
		case 23:
			cfg_para.cfg_ch.cfg_ch24 = cfg_ch_tmp;	break;
		case 24:
			cfg_para.cfg_ch.cfg_ch25 = cfg_ch_tmp;	break;
		case 25:
			cfg_para.cfg_ch.cfg_ch26 = cfg_ch_tmp;	break;
		case 26:
			cfg_para.cfg_ch.cfg_ch27 = cfg_ch_tmp;	break;
		case 27:
			cfg_para.cfg_ch.cfg_ch28 = cfg_ch_tmp;	break;
		case 28:
			cfg_para.cfg_ch.cfg_ch29 = cfg_ch_tmp;	break;
		case 29:
			cfg_para.cfg_ch.cfg_ch30 = cfg_ch_tmp;	break;
		
		case 30:
			cfg_para.cfg_ch.cfg_ch31 = cfg_ch_tmp;	break;
		case 31:
			cfg_para.cfg_ch.cfg_ch32 = cfg_ch_tmp;	break;
		case 32:
			cfg_para.cfg_ch.cfg_ch33 = cfg_ch_tmp;	break;
		case 33:
			cfg_para.cfg_ch.cfg_ch34 = cfg_ch_tmp;	break;
		case 34:
			cfg_para.cfg_ch.cfg_ch35 = cfg_ch_tmp;	break;
		case 35:
			cfg_para.cfg_ch.cfg_ch36 = cfg_ch_tmp;	break;
		case 36:
			cfg_para.cfg_ch.cfg_ch37 = cfg_ch_tmp;	break;		
	}
}

void str_split(char *left,char *right,char *src, char c)
{
    char *p = src;
    char *q = left;
    char *r = right;
    int flag = 0;

    while(*p != '\0'){
        if(*p==c)
            flag = 1;
        else{
            if(flag)
                *(r++) = *p;
            else
                *(q++) = *p;
        }
        p++;
    }
    *(q++)='\0';
    *(r++)='\0';
}

/*
*********************************************************************************************************
*	�� �� ��: CreateNewFile
*	����˵��: ��SD������һ�����ļ����ļ�������д��www.armfly.com��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void CreateNewFile(uint8_t pronum,uint8_t picnum)
{
	/* ������ʹ�õľֲ�����ռ�ý϶࣬���޸�����ջ��С����֤��ջ�ռ乻�� */
	FRESULT result;
	FATFS fs;
	FIL file;
	DIR DirInf;
	uint32_t bw;
	char ProjectFileName[30];
	char FileNamebuf[12]={0};

 	/* �����ļ�ϵͳ */
	result = f_mount(&fs, "0:", 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
	}

	/* �򿪸��ļ��� */
	result = f_opendir(&DirInf, "0:/"); /* ���������������ӵ�ǰĿ¼��ʼ */
	if (result != FR_OK)
	{
		printf("�򿪸�Ŀ¼ʧ��  (%s)\r\n", FR_Table[result]);
		return;
	}

	/* ���ļ� */
//	if(flag==0)
//	{
		strncpy(FileNamebuf,cfgname[pronum],strlen(cfgname[pronum])-4);
		sprintf(ProjectFileName, "0:/%s%02d.txt",FileNamebuf, picnum);
//	}else{
//		sprintf(ProjectFileName, "0:/name.txt");
//	}
	result = f_open(&file, ProjectFileName, FA_CREATE_ALWAYS | FA_WRITE);

		/* дһ������ */
//	if(flag==0)
//	{
		result = f_write(&file, &cfg_para, sizeof(cfg_para), &bw);
//	}else{
//		result = f_write(&file, &cfgname, sizeof(cfgname), &bw);
//	}
	if (result == FR_OK)
	{
		printf("%s �ļ�д��ɹ�\r\n",ProjectFileName);
	}
	else
	{
		printf("%s �ļ�д��ʧ��  (%s)\r\n", ProjectFileName, FR_Table[result]);
	}
	
	f_close(&file);

	/* ж���ļ�ϵͳ */
	 f_mount(NULL, "0:", 0);
}


/*
*********************************************************************************************************
*	�� �� ��: ReadFileData
*	����˵��: ��ȡ�ļ�armfly.txtǰ128���ַ�������ӡ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ReadFileData(char *_ucVolume,char *_fileName,void *buf,uint16_t len)
{
	/* ������ʹ�õľֲ�����ռ�ý϶࣬���޸�����ջ��С����֤��ջ�ռ乻�� */
	FRESULT result;
	FATFS fs;
	FIL file;
	DIR DirInf;
	uint32_t bw;
//	char buf[128];
	char path[5];
	char pathname[32];

 	/* �����ļ�ϵͳ */
	result = f_mount(&fs, _ucVolume, 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
	}

	/* �򿪸��ļ��� */
	sprintf(path, "%s/", _ucVolume);
	result = f_opendir(&DirInf, path); /* ���������������ӵ�ǰĿ¼��ʼ */
	if (result != FR_OK)
	{
		printf("�򿪸�Ŀ¼ʧ��  (%s)\r\n", FR_Table[result]);
		return;
	}

	/* ���ļ� */
	sprintf(pathname, "%s%s.txt", path , _fileName);
	result = f_open(&file, pathname, FA_OPEN_EXISTING | FA_READ);
	if (result !=  FR_OK)
	{
		printf("Don't Find File : %s\r\n",pathname);
		return;
	}

	/* ��ȡ�ļ� */
	result = f_read(&file, buf, len, &bw);
	if (bw > 0)
	{
//		buf[bw] = 0;
		*((char*)buf+bw) = 0;
//		printf("\r\narmfly.txt �ļ����� : \r\n%s\r\n", buf);
		printf("\r\n%s �ļ���ȡ�ɹ� \r\n",pathname);
	}
	else
	{
		printf("\r\n%s �ļ���ȡʧ��\r\n",pathname);
	}

	/* �ر��ļ�*/
	f_close(&file);

	/* ж���ļ�ϵͳ */
	f_mount(NULL, path, 0);
}
static void DeleteDirFile(char *_ucVolume)
{
	FRESULT result;
	FATFS fs;
//	char FileName[13];
	uint32_t cnt = 0;
	DIR DirInf;
	FILINFO FileInf;
	char lfname[32];
	char path[32];

 	/* �����ļ�ϵͳ */
	result = f_mount(&fs, _ucVolume, 0);			/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf("�����ļ�ϵͳʧ�� (%s)\r\n", FR_Table[result]);
	}

	#if 0
	/* �򿪸��ļ��� */
	result = f_opendir(&DirInf, "/"); /* ���������������ӵ�ǰĿ¼��ʼ */
	if (result != FR_OK)
	{
		printf("�򿪸�Ŀ¼ʧ�� (%s)\r\n", FR_Table[result]);
		return;
	}
	#endif
	
	/* �򿪸��ļ��� */
	sprintf(path, "%s/", _ucVolume);	 /* 1: ��ʾ�̷� */
	result = f_opendir(&DirInf, path);
	if (result != FR_OK)
	{
		printf("�򿪸�Ŀ¼ʧ�� (%d)\r\n", result);
		return;
	}

	/* ��ȡ��ǰ�ļ����µ��ļ���Ŀ¼ */
	FileInf.lfname = lfname;
	FileInf.lfsize = 32;
	for (cnt = 0; ;cnt++)
	{
		result = f_readdir(&DirInf,&FileInf); 		/* ��ȡĿ¼��������Զ����� */
		if (result != FR_OK || FileInf.fname[0] == 0)
		{
			break;
		}

		if (FileInf.fname[0] == '.')
		{
			continue;
		}

		/* �ж����ļ�������Ŀ¼ */
		if (!(FileInf.fattrib & AM_DIR))
		{
			if(strstr((char *)FileInf.fname,".txt"))
			{				
				result = f_unlink(FileInf.fname);
				if (result == FR_OK)
				{
					printf("ɾ���ļ�%s�ɹ�\r\n", (char *)FileInf.fname);
				}
			}
		}
	}

	/* ж���ļ�ϵͳ */
	f_mount(NULL, _ucVolume, 0);
}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/


