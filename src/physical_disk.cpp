/*
	Physical disk
*/
#include	<windows.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	"physical_disk.h"
#include	"zlib/zlib.h"

//extern	unsigned long	crc32(unsigned char *,unsigned long);

unsigned long	physical_disk_number_max=0;
unsigned long	HDD_SECTOR_SIZE	=HDD_SECTOR_SIZE_0512;

unsigned char	null_attribute[8]							={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char	null_guid[16]								={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char	partition_type_guid_efi_system[16]			={0x28,0x73,0x2A,0xC1,0x1F,0xF8,0xD2,0x11,0xBA,0x4B,0x00,0xA0,0xC9,0x3E,0xC9,0x3B};
unsigned char	partition_type_guid_mbr_partition[16]		={0x41,0xee,0x4d,0x02,0xe7,0x33,0xd3,0x11,0x9d,0x69,0x00,0x08,0xC7,0x81,0xf3,0x9f};
unsigned char	partition_type_guid_bios_grub[16]			={0x48,0x61,0x68,0x21,0x49,0x64,0x6f,0x6e,0x74,0x4e,0x65,0x65,0x64,0x45,0x46,0x49};

unsigned char	partition_type_guid_windows_recovery[16]	={0xA4,0xBB,0x94,0xDE,0xD1,0x06,0x40,0x4D,0xA1,0x6A,0xBF,0xD5,0x01,0x79,0xD6,0xAC};
unsigned char	partition_type_guid_microsoft_reserved[16]	={0x16,0xE3,0xC9,0xE3,0x5C,0x0B,0xB8,0x4D,0x81,0x7D,0xF9,0x2D,0xF0,0x02,0x15,0xAE};
unsigned char	partition_type_guid_data_partition[16]		={0xa2,0xa0,0xd0,0xeb,0xe5,0xb9,0x33,0x44,0x87,0xc0,0x68,0xb6,0xb7,0x26,0x99,0xc7};
unsigned char	partition_type_guid_dvmd_partition[16]		={0xaa,0xc8,0x08,0x58,0x8f,0x7e,0xe0,0x42,0x85,0xd2,0xe1,0xe9,0x04,0x34,0xcf,0xb3};
unsigned char	partition_type_guid_dvd_partition[16]		={0xa0,0x60,0x9b,0xaf,0x31,0x14,0x62,0x4f,0xbc,0x68,0x33,0x11,0x71,0x4a,0x69,0xad};
unsigned char	partition_type_guid_storagepool[16]			={0x8f,0xaf,0x5c,0xe7,0x80,0xf6,0xee,0x4c,0xaf,0xa3,0xb0,0x01,0xe5,0x6e,0xfc,0x2d};
//unsigned char	partition_guid_gpt[16]						={0xc2,0xa2,0xe7,0xdf,0x11,0xe3,0xe2,0x11,0xbe,0x9e,0x50,0x46,0x5d,0x90,0xd7,0x9d};

/*
HP-UX データパーティション 75894C1E-3AEB-11D3-B7C1-7B03A0000000 
サービスパーティション E2A1E728-32E3-11D6-A682-7B03A0000000 
Linux データパーティション EBD0A0A2-B9E5-4433-87C0-68B6B72699C7 
RAIDパーティション A19D880F-05FC-4D3B-A006-743F0F84911E 
スワップパーティション 0657FD6D-A4AB-43C4-84E5-0933C84B4F4F 
LVMパーティション E6D6D379-F507-44C2-A23C-238F2A3DF928 
予約済み 8DA63339-0007-60C0-C436-083AC8230908 
FreeBSD データパーティション 516E7CB4-6ECF-11D6-8FF8-00022D09712B 
スワップパーティション 516E7CB5-6ECF-11D6-8FF8-00022D09712B 
UFSパーティション 516E7CB6-6ECF-11D6-8FF8-00022D09712B 
Vinum Volume Managerパーティション 516E7CB8-6ECF-11D6-8FF8-00022D09712B 
Mac OS X HFS (HFS+) パーティション 48465300-0000-11AA-AA11-00306543ECAC 
Apple UFS 55465300-0000-11AA-AA11-00306543ECAC 
ZFS 6A898CC3-1DD2-11B2-99A6-080020736631 
Apple RAIDパーティション 52414944-0000-11AA-AA11-00306543ECAC 
Apple RAIDパーティション、オフライン 52414944-5F4F-11AA-AA11-00306543ECAC 
Appleブートパーティション 426F6F74-0000-11AA-AA11-00306543ECAC 
Appleラベル 4C616265-6C00-11AA-AA11-00306543ECAC 
Apple TVリカバリパーティション 5265636F-7665-11AA-AA11-00306543ECAC 
Solaris ブートパーティション 6A82CB45-1DD2-11B2-99A6-080020736631 
Rootパーティション 6A85CF4D-1DD2-11B2-99A6-080020736631 
スワップパーティション 6A87C46F-1DD2-11B2-99A6-080020736631 
バックアップパーティション 6A8B642B-1DD2-11B2-99A6-080020736631 
/usrパーティション 6A898CC3-1DD2-11B2-99A6-080020736631 
/varパーティション 6A8EF2E9-1DD2-11B2-99A6-080020736631 
/homeパーティション 6A90BA39-1DD2-11B2-99A6-080020736631 
EFI_ALTSCTR 6A9283A5-1DD2-11B2-99A6-080020736631 
予約済みパーティション 6A945A3B-1DD2-11B2-99A6-080020736631 
6A9630D1-1DD2-11B2-99A6-080020736631 
6A980767-1DD2-11B2-99A6-080020736631 
6A96237F-1DD2-11B2-99A6-080020736631 
6A8D2AC7-1DD2-11B2-99A6-080020736631 
*/



void	disp_16byte(unsigned char	*guid)
{
	printf("%02X%02X%02X%02X:%02X%02X%02X%02X:%02X%02X%02X%02X:%02X%02X%02X%02X",
		guid[ 0],guid[ 1],guid[ 2],guid[ 3],
		guid[ 4],guid[ 5],guid[ 6],guid[ 7],
		guid[ 8],guid[ 9],guid[10],guid[11],
		guid[12],guid[13],guid[14],guid[15]);
}

void	disp_guid(unsigned char	*guid)
{
	printf("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid[ 3],guid[ 2],guid[ 1],guid[ 0],
		guid[ 5],guid[ 4],
		guid[ 7],guid[ 6],
		guid[ 8],guid[ 9],
		guid[10],guid[11],guid[12],guid[13],guid[14],guid[15]);
}


void	disp_get_lasterror(const char *msg)
{
	LPVOID	emsg;
	FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM |
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
	    GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
		(LPTSTR)&emsg,
	    0,
	    NULL
	);
	printf("%s : %s\n",msg,emsg);
	LocalFree(emsg);
}

void	disp_dump(unsigned char *addr,unsigned long len,unsigned long guide)
{
	unsigned long	tul1;
	unsigned char	wcs[0x20];
	unsigned char	mbs[0x20];

	for(tul1=0;tul1<len;tul1++){
		if((tul1%0x100==0)&&(guide!=0))
			printf("address  +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F -multibyte char- -wide char------\n");
		if(tul1%16==0){
			printf("%08X ",tul1);
			memset(mbs,0,0x20);
			memset(wcs,0,0x20);
		}

			printf("%02X ",*(addr+tul1));

		if(isprint(*(addr+tul1)))	mbs[tul1%16]=*(addr+tul1);
		else						mbs[tul1%16]=0x20;

		if((tul1%2)==0){
			wcstombs((char *)wcs+(tul1%16),(wchar_t *)(addr+(tul1&0xfffffffe)),0x1);
			if(!isprint(	*(wcs+(tul1%16)+0)))
							*(wcs+(tul1%16)+0)= 0x20;
			if(!isprint(	*(wcs+(tul1%16)+1)))
							*(wcs+(tul1%16)+1)= 0x20;
		}

		if(tul1%16==0xf){
			printf("%s ",mbs);
			printf("%s\n",wcs);
		}
	}
			printf("\n");
}



HANDLE	physical_disk_handle_open_read(unsigned long pdu)
{
	char	pdn[0x100];
	HANDLE	retval;

	sprintf(pdn,"\\\\.\\PHYSICALDRIVE%d",pdu);
	retval = CreateFile(
		(LPCSTR)pdn,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

//	if (INVALID_HANDLE_VALUE != retval) {
//		physical_disk_set_sector_size(pdu);
//	}else{
//		disp_get_lasterror("pdhor");
//	}

	return(retval);
}



HANDLE	physical_disk_handle_open_write(unsigned long pdu)
{
	char	pdn[0x100];
	HANDLE	retval;

	sprintf(pdn, "\\\\.\\PHYSICALDRIVE%d", pdu);
	retval = CreateFile(
		(LPCSTR)pdn,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

//	if (INVALID_HANDLE_VALUE != retval) {
//		physical_disk_set_sector_size(pdu);
//	}else{
//		disp_get_lasterror("pdhow");
//	}

	return(retval);
}



void	physical_disk_sector_read(HANDLE hpd, ULONG64 sector, unsigned char *data, unsigned long sector_size)
{
	unsigned long	tul1;
	LARGE_INTEGER	tli;

	tli.QuadPart = sector * sector_size;
	SetFilePointerEx(hpd, tli, 0, FILE_BEGIN);
	ReadFile(hpd, data, sector_size, &tul1, NULL);
}

void	physical_disk_sector_write(HANDLE hpd, ULONG64 sector, unsigned char *data, unsigned long sector_size)
{
	unsigned long	tul1;
	LARGE_INTEGER	tli;

	tli.QuadPart = sector * sector_size;
	SetFilePointerEx(hpd, tli, 0, FILE_BEGIN);
	WriteFile(hpd, data, sector_size, &tul1, NULL);
}



void	physical_disk_sector_dump(unsigned long pdu,ULONG64 sector)
{
	HANDLE			hpd;
	unsigned char	data[HDD_SECTOR_SIZE_4096];
	unsigned long	tul1;

	hpd=physical_disk_handle_open_read(pdu);
	if(hpd==INVALID_HANDLE_VALUE){
		printf("invalid physical drive number ? %d \n",pdu);
		return;
	}

	for(tul1=0;tul1<HDD_SECTOR_SIZE_4096;tul1+=0x10){
		memcpy(data+tul1,"fail HDD read !!",0x10);
	}

	physical_disk_sector_read(hpd,sector,data, HDD_SECTOR_SIZE);
	printf("sector %016I64X\n", sector);
	disp_dump(data,HDD_SECTOR_SIZE,1);
	CloseHandle(hpd);
}



unsigned long	physical_disk_set_sector_size(unsigned long pdu)
{
	unsigned long	tul1;
	HANDLE			hpd;
	struct mbr		mbr_info;

	hpd=physical_disk_handle_open_read(pdu);
			HDD_SECTOR_SIZE	=0;

	if(hpd!=INVALID_HANDLE_VALUE){
		memset(&mbr_info,0,sizeof(mbr));
		ReadFile(hpd,&mbr_info,HDD_SECTOR_SIZE_0512,&tul1,NULL); 
		if(mbr_info.partition_table_1.start_sector_lba!=0){
			HDD_SECTOR_SIZE	=HDD_SECTOR_SIZE_0512;
			CloseHandle(hpd);
			goto	end_sector_check;
		}

		ReadFile(hpd,&mbr_info,HDD_SECTOR_SIZE_4096,&tul1,NULL); 
		if(mbr_info.partition_table_1.start_sector_lba!=0){
			HDD_SECTOR_SIZE	=HDD_SECTOR_SIZE_4096;
			CloseHandle(hpd);
			goto	end_sector_check;
		}

			HDD_SECTOR_SIZE	=37564;	//unknown
			CloseHandle(hpd);
end_sector_check:
			NULL;
	}
			return(HDD_SECTOR_SIZE);
}




void	physical_disk_number_max_get(void)
{
	unsigned long	tul1;

	for(physical_disk_number_max=0;physical_disk_number_max<99;physical_disk_number_max++){
		tul1=physical_disk_set_sector_size(physical_disk_number_max);

		if(tul1==HDD_SECTOR_SIZE_0512)	goto	end_physical_disk_list;
		if(tul1==HDD_SECTOR_SIZE_4096)	goto	end_physical_disk_list;
		if(tul1==0)						return;	//どれでもない=physicaldiskではない
end_physical_disk_list:
				NULL;
	}
}



void	physical_disk_list(void)
{
	int				x;
	unsigned long	tul1;

	for(x=0;x<99;x++){
		tul1=physical_disk_set_sector_size(x);

		if(tul1==HDD_SECTOR_SIZE_0512){
			printf("physical drive %d -  512 byte sector\n",x);
			goto	end_physical_disk_list;
		}

		if(tul1==HDD_SECTOR_SIZE_4096){
			printf("physical drive %d - 4096 byte sector\n",x);
			goto	end_physical_disk_list;
		}

		if(tul1==0){
			goto	end_physical_disk_list;
		}

			printf("physical drive %d - unknown size sector\n",x);
end_physical_disk_list:
				NULL;
	}
}



void	physical_disk_mbr_disp(unsigned long pdu)
{
	HANDLE			hpd;
	unsigned long	tul1;

	hpd=physical_disk_handle_open_read(pdu);

	if(hpd==INVALID_HANDLE_VALUE){
		printf("invalid physical drive number ? %d \n",pdu);
		return;
	}

	struct mbr	mbr_info;

	ReadFile(hpd,&mbr_info,HDD_SECTOR_SIZE,&tul1,NULL); 

	printf("MBR infomation\tphysical drive number %02d\n",pdu);

	disp_dump(mbr_info.bootstrap_loader,446,1);

	printf("partition table 1 - boot flag\t\t%02X\n",				mbr_info.partition_table_1.boot_flag);
	printf("partition table 1 - start sector CHS\t%02X%02X%02X\n",	mbr_info.partition_table_1.start_sector_chs_1,
																	mbr_info.partition_table_1.start_sector_chs_2,
																	mbr_info.partition_table_1.start_sector_chs_3);
	printf("partition table 1 - partition type\t%02X",				mbr_info.partition_table_1.partition_type);
		if(mbr_info.partition_table_1.partition_type==0xee)		printf(" : System\n");
		else													printf("\n");
	printf("partition table 1 - end sector CHS\t%02X%02X%02X\n",	mbr_info.partition_table_1.end_sector_chs_1,
																	mbr_info.partition_table_1.end_sector_chs_2,
																	mbr_info.partition_table_1.end_sector_chs_3);
	printf("partition table 1 - start sector LBA\t%08X\n",			mbr_info.partition_table_1.start_sector_lba);
	printf("partition table 1 - sector size LBA\t%08X\n",			mbr_info.partition_table_1.sector_size_lba);

	printf("partition table 2 - boot flag\t\t%02X\n",				mbr_info.partition_table_2.boot_flag);
	printf("partition table 2 - start sector CHS\t%02X%02X%02X\n",	mbr_info.partition_table_2.start_sector_chs_1,
																	mbr_info.partition_table_2.start_sector_chs_2,
																	mbr_info.partition_table_2.start_sector_chs_3);
	printf("partition table 2 - partition type\t%02X",				mbr_info.partition_table_2.partition_type);
		if(mbr_info.partition_table_1.partition_type==0xee)		printf(" : System\n");
		else													printf("\n");
	printf("partition table 2 - end sector CHS\t%02X%02X%02X\n",	mbr_info.partition_table_2.end_sector_chs_1,
																	mbr_info.partition_table_2.end_sector_chs_2,
																	mbr_info.partition_table_2.end_sector_chs_3);
	printf("partition table 2 - start sector LBA\t%08X\n",			mbr_info.partition_table_2.start_sector_lba);
	printf("partition table 2 - sector size LBA\t%08X\n",			mbr_info.partition_table_2.sector_size_lba);

	printf("partition table 3 - boot flag\t\t%02X\n",				mbr_info.partition_table_3.boot_flag);
	printf("partition table 3 - start sector CHS\t%02X%02X%02X\n",	mbr_info.partition_table_3.start_sector_chs_1,
																	mbr_info.partition_table_3.start_sector_chs_2,
																	mbr_info.partition_table_3.start_sector_chs_3);
	printf("partition table 3 - partition type\t%02X",				mbr_info.partition_table_3.partition_type);
		if(mbr_info.partition_table_1.partition_type==0xee)		printf(" : System\n");
		else													printf("\n");
	printf("partition table 3 - end sector CHS\t%02X%02X%02X\n",	mbr_info.partition_table_3.end_sector_chs_1,
																	mbr_info.partition_table_3.end_sector_chs_2,
																	mbr_info.partition_table_3.end_sector_chs_3);
	printf("partition table 3 - start sector LBA\t%08X\n",			mbr_info.partition_table_3.start_sector_lba);
	printf("partition table 3 - sector size LBA\t%08X\n",			mbr_info.partition_table_3.sector_size_lba);

	printf("partition table 4 - boot flag\t\t%02X\n",				mbr_info.partition_table_4.boot_flag);
	printf("partition table 4 - start sector CHS\t%02X%02X%02X\n",	mbr_info.partition_table_4.start_sector_chs_1,
																	mbr_info.partition_table_4.start_sector_chs_2,
																	mbr_info.partition_table_4.start_sector_chs_3);
	printf("partition table 4 - partition type\t%02X",				mbr_info.partition_table_4.partition_type);
		if(mbr_info.partition_table_1.partition_type==0xee)		printf(" : System\n");
		else													printf("\n");
	printf("partition table 4 - end sector CHS\t%02X%02X%02X\n",	mbr_info.partition_table_4.end_sector_chs_1,
																	mbr_info.partition_table_4.end_sector_chs_2,
																	mbr_info.partition_table_4.end_sector_chs_3);
	printf("partition table 4 - start sector LBA\t%08X\n",			mbr_info.partition_table_4.start_sector_lba);
	printf("partition table 4 - sector size LBA\t%08X\n",			mbr_info.partition_table_4.sector_size_lba);

	printf("boot signature\t%02X\n",								mbr_info.boot_signature);

	CloseHandle(hpd);
}


void	physical_disk_gpt_disp(unsigned long pdu)
{
	HANDLE			hpd;
	unsigned long	crc32_1st;
	unsigned long	crc32_2nd;
	unsigned long	cmp32_1st;
	unsigned long	cmp32_2nd;
	LARGE_INTEGER	tli1;
	unsigned long	tul1;
	char			signature_d[0x10];
	unsigned char	*peb_1st;
	unsigned char	*peb_2nd;

	hpd=physical_disk_handle_open_read(pdu);

	if(hpd==INVALID_HANDLE_VALUE){
		printf("invalid physical drive number ? %d \n",pdu);
		return;
	}

	peb_1st = new unsigned char[0x4000];
	struct gpt	gpt_info_1st;
	tli1.QuadPart = HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
	ReadFile(hpd,&gpt_info_1st,HDD_SECTOR_SIZE,&tul1,NULL); 
	tli1.QuadPart = HDD_SECTOR_SIZE * gpt_info_1st.partition_entry_lba;
	SetFilePointerEx(hpd, tli1, 0, FILE_BEGIN);
	ReadFile(hpd, peb_1st, 0x4000, &tul1, NULL);

	peb_2nd = new unsigned char[0x4000];
	struct gpt	gpt_info_2nd;
	tli1.QuadPart = HDD_SECTOR_SIZE * gpt_info_1st.gpt_header_2nd_lba;
	SetFilePointerEx(hpd, tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpt_info_2nd, HDD_SECTOR_SIZE, &tul1, NULL);
	tli1.QuadPart = HDD_SECTOR_SIZE * gpt_info_2nd.partition_entry_lba;
	SetFilePointerEx(hpd, tli1, 0, FILE_BEGIN);
	ReadFile(hpd, peb_2nd, 0x4000, &tul1, NULL);

	CloseHandle(hpd);

	printf("gpt infomation\tphysical drive number %02d\n",pdu);

	memset(signature_d,0,0x10);
	memcpy(signature_d,gpt_info_1st.signature,8);
	printf("1st gpt signature\t%s\n",						signature_d);
	printf("version\t\t\t%08X\n",					gpt_info_1st.version);
	printf("header size\t\t%08X\n",					gpt_info_1st.header_size);
	printf("header crc32\t\t%08X\n",				gpt_info_1st.header_crc32);
	printf("1st gpt header LBA\t%016llX\n",			gpt_info_1st.gpt_header_1st_lba);
	printf("2nd gpt header LBA\t%016llX\n",			gpt_info_1st.gpt_header_2nd_lba);
	printf("useable area start LBA\t%016llX\n",		gpt_info_1st.useable_area_start_lba);
	printf("useable area end LBA\t%016llX\n",		gpt_info_1st.useable_area_end_lba);
	printf("disk GUID\t\t");
	disp_16byte(	gpt_info_1st.disk_guid);
	printf(" ");
	disp_guid(		gpt_info_1st.disk_guid);
	printf("\n");
	printf("partition entry LBA\t%016llX\n",		gpt_info_1st.partition_entry_lba);
	printf("partition entry count\t%08X\n",			gpt_info_1st.partition_entry_count);
	printf("partition entry size\t%08X\n",			gpt_info_1st.partition_entry_size);
	printf("partition entry crc32\t%08X\n",			gpt_info_1st.partition_entry_crc32);

	crc32_1st = gpt_info_1st.header_crc32;
	gpt_info_1st.header_crc32 = 0;
	cmp32_1st = crc32(0x00000000, (unsigned char *)(gpt_info_1st.signature), gpt_info_1st.header_size);
	if (crc32_1st != cmp32_1st)
			printf("header crc32\t\tinvalid ?\t%08X\n", cmp32_1st);
	else	printf("header crc32\t\tok\n");

	cmp32_1st = crc32(0x00000000, peb_1st, 0x4000);
	if (gpt_info_1st.partition_entry_crc32 != cmp32_1st)
			printf("partition entry crc32\tinvalid ?\t%08X\n", cmp32_1st);
	else	printf("partition entry crc32\tok\n");

	printf("\n");

	memset(signature_d, 0, 0x10);
	memcpy(signature_d, gpt_info_2nd.signature, 8);
	printf("2nd gpt signature\t%s\n", signature_d);
	printf("version\t\t\t%08X\n", gpt_info_2nd.version);
	printf("header size\t\t%08X\n", gpt_info_2nd.header_size);
	printf("header crc32\t\t%08X\n", gpt_info_2nd.header_crc32);
	printf("1st gpt header LBA\t%016llX\n", gpt_info_2nd.gpt_header_1st_lba);
	printf("2nd gpt header LBA\t%016llX\n", gpt_info_2nd.gpt_header_2nd_lba);
	printf("useable area start LBA\t%016llX\n", gpt_info_2nd.useable_area_start_lba);
	printf("useable area end LBA\t%016llX\n", gpt_info_2nd.useable_area_end_lba);
	printf("disk GUID\t\t");
	disp_16byte(gpt_info_2nd.disk_guid);
	printf(" ");
	disp_guid(gpt_info_2nd.disk_guid);
	printf("\n");
	printf("partition entry LBA\t%016llX\n", gpt_info_2nd.partition_entry_lba);
	printf("partition entry count\t%08X\n", gpt_info_2nd.partition_entry_count);
	printf("partition entry size\t%08X\n", gpt_info_2nd.partition_entry_size);
	printf("partition entry crc32\t%08X\n", gpt_info_2nd.partition_entry_crc32);

	crc32_2nd = gpt_info_2nd.header_crc32;
	gpt_info_2nd.header_crc32 = 0;
	cmp32_2nd = crc32(0x00000000, (unsigned char *)(gpt_info_2nd.signature), gpt_info_2nd.header_size);
	if (crc32_2nd != cmp32_2nd)
			printf("header crc32\ttinvalid ?\t%08X\n", cmp32_2nd);
	else	printf("header crc32\t\tok\n");

	cmp32_2nd = crc32(0x00000000, peb_2nd, 0x4000);
	if (gpt_info_2nd.partition_entry_crc32 != cmp32_2nd)
			printf("partition entry crc32\tinvalid ?\t%08X\n", cmp32_2nd);
	else	printf("partition entry crc32\tok\n");
	printf("\n");

	delete[]	peb_1st;
	delete[]	peb_2nd;
}
/*
	gpt_info.header_crc32について
	offset +0から gpt_info.header_sizeで示される長さまで
	初期値0にてcrc32で計算する
	ただし計算中のgpt_info.header_crc32はゼロとする。

	gpt_info.partition_entry_crc32について
	パーティションエントリはいくつ使用しているかの明示は無いので
	恐らくパーティションエントリ全体（count x size）を対象として
	初期値0にてcrc32で計算している？（未確認）
*/



void	physical_disk_gpt_partition_disp(unsigned long pdu)
{
	char			name_mbs[0x100];
	HANDLE			hpd;
	unsigned long	partition_per_sector=HDD_SECTOR_SIZE/0x80;
	LARGE_INTEGER	tli1;
	unsigned long	tul1;
	unsigned long	tul2;


	hpd=physical_disk_handle_open_read(pdu);

	if(hpd==INVALID_HANDLE_VALUE){
		printf("invalid physical drive number ? %d \n",pdu);
		CloseHandle(hpd);
		return;
	}

	struct gpt							gpt_info;
	struct gpt_partition_entry_sector	gpe_info;
			tli1.QuadPart= HDD_SECTOR_SIZE * 1;
			SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
			ReadFile(hpd,&gpt_info,HDD_SECTOR_SIZE,&tul1,NULL); 


	printf("gpt partition entry infomation\tphysical drive number %02d\n",pdu);
	for(tul2=0;tul2<gpt_info.partition_entry_count;tul2++){
		if(tul2%partition_per_sector==0){
			tli1.QuadPart= HDD_SECTOR_SIZE*(gpt_info.partition_entry_lba + (tul2*partition_per_sector));
			SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
			ReadFile(hpd,&gpe_info,HDD_SECTOR_SIZE,&tul1,NULL); 
		}

		if(gpe_info.entry[tul2%partition_per_sector].end_lba==0)	break;

		printf("entry %d/%d\n",tul2,gpt_info.partition_entry_count);

		printf("partition type GUID\t");
		disp_16byte(	gpe_info.entry[tul2%partition_per_sector].partition_type_guid);
		printf(" ");
		disp_guid(		gpe_info.entry[tul2%partition_per_sector].partition_type_guid);

		if(0==memcmp(						gpe_info.entry[tul2%partition_per_sector].partition_type_guid,	null_guid,16))								printf(" : unused");
		if(0==memcmp(						gpe_info.entry[tul2%partition_per_sector].partition_type_guid,	partition_type_guid_efi_system,16))			printf(" : EFI System");
		if(0==memcmp(						gpe_info.entry[tul2%partition_per_sector].partition_type_guid,	partition_type_guid_mbr_partition,16))		printf(" : MBR partition");
		if(0==memcmp(						gpe_info.entry[tul2%partition_per_sector].partition_type_guid,	partition_type_guid_bios_grub,16))			printf(" : BIOS GRUB");

		if(0==memcmp(						gpe_info.entry[tul2%partition_per_sector].partition_type_guid,	partition_type_guid_windows_recovery,16))	printf(" : Windows Recovery");
		if(0==memcmp(						gpe_info.entry[tul2%partition_per_sector].partition_type_guid,	partition_type_guid_microsoft_reserved,16))	printf(" : Microsoft Reserved");
		if(0==memcmp(						gpe_info.entry[tul2%partition_per_sector].partition_type_guid,	partition_type_guid_data_partition,16))		printf(" : Windows / Linux Data partition");
		if(0==memcmp(						gpe_info.entry[tul2%partition_per_sector].partition_type_guid,	partition_type_guid_dvmd_partition,16))		printf(" : Windows Dynamic Volume Meta Data partition");
		if(0==memcmp(						gpe_info.entry[tul2%partition_per_sector].partition_type_guid,	partition_type_guid_dvd_partition,16))		printf(" : Windows Dynamic Volume Data partition");
		if(0==memcmp(						gpe_info.entry[tul2%partition_per_sector].partition_type_guid,	partition_type_guid_storagepool,16))		printf(" : StoragePool");
		printf("\n");

		printf("partition GUID\t\t");
		disp_16byte(	gpe_info.entry[tul2%partition_per_sector].partition_guid);
		printf(" ");
		disp_guid(		gpe_info.entry[tul2%partition_per_sector].partition_guid);
		printf("\n");

		printf("start LBA\t\t%016llX\n",	gpe_info.entry[tul2%partition_per_sector].start_lba);
		printf("end LBA\t\t\t%016llX\n",	gpe_info.entry[tul2%partition_per_sector].end_lba);
		printf("attribute\t\t%02X%02X%02X%02X:%02X%02X%02X%02X",
											gpe_info.entry[tul2%partition_per_sector].attribute[0],
											gpe_info.entry[tul2%partition_per_sector].attribute[1],
											gpe_info.entry[tul2%partition_per_sector].attribute[2],
											gpe_info.entry[tul2%partition_per_sector].attribute[3],
											gpe_info.entry[tul2%partition_per_sector].attribute[4],
											gpe_info.entry[tul2%partition_per_sector].attribute[5],
											gpe_info.entry[tul2%partition_per_sector].attribute[6],
											gpe_info.entry[tul2%partition_per_sector].attribute[7]	);

		if(gpe_info.entry[tul2%partition_per_sector].attribute[7]&0x80)	printf(" : System");
		if(gpe_info.entry[tul2%partition_per_sector].attribute[7]&0x40)	printf(" : EFI ignore");
		if(gpe_info.entry[tul2%partition_per_sector].attribute[7]&0x20)	printf(" : Legacy BIOS bootable");
		if(gpe_info.entry[tul2%partition_per_sector].attribute[0]&0x01)	printf(" : not mount");
		if(gpe_info.entry[tul2%partition_per_sector].attribute[0]&0x02)	printf(" : hidden");
		if(gpe_info.entry[tul2%partition_per_sector].attribute[0]&0x08)	printf(" : read only");
																		printf("\n");

		memset(name_mbs, 0, 0x100);
		wcstombs(name_mbs,					gpe_info.entry[tul2%partition_per_sector].name,0x48);
		printf("name\t\t\t%s\n",			name_mbs);
	}
	CloseHandle(hpd);
	printf("\n");

}





/*
void	physical_disk_export_reserve_area_sector(unsigned long pdu)
{
	HANDLE			hpd;
	HANDLE			hra;
	unsigned long	partition_per_sector = HDD_SECTOR_SIZE / 0x80;
	unsigned long	tul1;
	unsigned long	tul2;
	LARGE_INTEGER	tli1;
	char			buf[HDD_SECTOR_SIZE_4096];


	hpd = physical_disk_handle_open_read(pdu);

	if (hpd == INVALID_HANDLE_VALUE) {
		printf("invalid physical drive number ? %d \n", pdu);
		return;
	}

	struct gpt							gpt_info;
	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpt_info, HDD_SECTOR_SIZE, &tul1, NULL);


	printf("physical drive number %02d\texport reserve area sector\n", pdu);

	tli1.QuadPart= HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpe_info, HDD_SECTOR_SIZE, &tul1, NULL);

	if (0 != memcmp(gpe_info.entry[0%partition_per_sector].partition_type_guid, partition_type_guid_microsoft_reserved, 16))	goto	func_end;
	if (0 != memcmp(gpe_info.entry[1%partition_per_sector].partition_type_guid, partition_type_guid_storagepool, 16))			goto	func_end;

	sprintf(buf, "reserve_%02d.bin", pdu);
	hra = CreateFile((LPCSTR)buf, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hra == INVALID_HANDLE_VALUE) {
		printf("can't create reserve.bin file\n");
		CloseHandle(hpd);
		return;
	}

	for (tul2 = gpe_info.entry[0 % partition_per_sector].start_lba; tul2 != gpe_info.entry[1 % partition_per_sector].start_lba; tul2++) {
		tli1.QuadPart= HDD_SECTOR_SIZE*tul2;
		SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
		ReadFile(hpd, buf, HDD_SECTOR_SIZE, &tul1, NULL);
		WriteFile(hra, buf, HDD_SECTOR_SIZE, &tul1, NULL);
	}
	printf("done ... \n");
	CloseHandle(hra);
	CloseHandle(hpd);
	return;
func_end:
	printf("not storage pool disk ?\n");
	CloseHandle(hpd);
}





void	physical_disk_sector_write_signature(unsigned long pdu, ULONG64 sector)
{
	HANDLE			hpd;
	unsigned char	data[HDD_SECTOR_SIZE_4096];
	unsigned long	tul1;

	hpd = physical_disk_handle_open_write(pdu);
	if (hpd == INVALID_HANDLE_VALUE) {
		printf("invalid physical drive number ? %d \n", pdu);
		return;
	}

	for (tul1 = 0; tul1<HDD_SECTOR_SIZE_4096; tul1 += 0x10) {
		memcpy(data + tul1, "-AZUCO- -AZUCO- ", 0x10);
	}

	physical_disk_sector_write(hpd, sector, data, HDD_SECTOR_SIZE);
	printf("sector %016I64X\n", sector);
	CloseHandle(hpd);
}
*/
