/*
	Storage pool helper
*/
#include	<windows.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<tchar.h>
#include	"physical_disk.h"
#include	"zlib/zlib.h"

extern	unsigned long	HDD_SECTOR_SIZE;
extern	unsigned char	partition_type_guid_storagepool[16];
extern	unsigned char	null_guid[16];
extern	unsigned char	null_attribute[8];

extern	HANDLE			physical_disk_handle_open_read(unsigned long);



void	high_byte_low_byte_swap(char *dst, char *src, unsigned long len)
{
	unsigned long	cnt;

	for (cnt = 0; cnt<len; cnt += 2) {
		dst[cnt + 0] = src[cnt + 1];
		dst[cnt + 1] = src[cnt + 0];
	}
}



//Storage Poolとしてのパーティション情報のチェック
unsigned long	storagepool_check_gpe(gpt_partition_entry_sector	gpe_info,unsigned char	disp)
{
	unsigned long	check_count = 0;

	if (0 != memcmp(gpe_info.entry[1].partition_type_guid, partition_type_guid_storagepool, 16)) {
		check_count++;
		if(disp)	printf("warning : GPT partition check : invalid partition type GUID ?\n");
	}

	if (0 == memcmp(gpe_info.entry[1].partition_guid, null_guid, 16)) {
		check_count++;
		if(disp)	printf("warning : GPT partition check : invalid partition GUID ?\n");
	}

	if (0 == gpe_info.entry[1].start_lba) {
		check_count++;
		if(disp)	printf("warning : GPT partition check : invalid start LBA ?\n");
	}

	if (0 == gpe_info.entry[1].end_lba) {
		check_count++;
		if(disp)	printf("warning : GPT partition check : invalid end LBA ?\n");
	}

	if (0 != memcmp(gpe_info.entry[1].attribute, null_attribute, 8)) {
		check_count++;
		if(disp)	printf("warning : GPT partition check : invalid attribute ?\n");
	}

	return(check_count);
}



//SDBCの存在チェック
unsigned long	storagepool_check_sdbc(unsigned long pdu)
{
	HANDLE			hpd;
	unsigned long	partition_per_sector = HDD_SECTOR_SIZE / 0x80;
	unsigned long	tul1;
	LARGE_INTEGER	tli1;
	unsigned long	retval = 0;

	unsigned char	buf[HDD_SECTOR_SIZE_4096];

	hpd = physical_disk_handle_open_read(pdu);

	if (hpd == INVALID_HANDLE_VALUE) {
		printf("invalid physical drive number ? %d \n", pdu);
		return(retval);
	}

	struct gpt							gpt_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpt_info, HDD_SECTOR_SIZE, &tul1, NULL);

	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart= HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpe_info, HDD_SECTOR_SIZE, &tul1, NULL);

	if (0 != storagepool_check_gpe(gpe_info,1))	goto	func_close;
	//チェック通る＝gpe_infoの中身を信用してもOKと判断

	tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8);
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, buf, HDD_SECTOR_SIZE, &tul1, NULL);

	if ((*(unsigned long *)(buf + 4) == 0x20202020) &&
		(*(unsigned long *)(buf + 0) == 0x43424453))	retval = 1;

func_close:
	CloseHandle(hpd);
	return(retval);
}



/*
このように事前に長さはわかるが、SDBB chain maxを同時に調べて、
メモリを効率化しているので、敢えて使わない。
unsigned long	storagepool_get_sdbb_length(unsigned long pdu)
{
HANDLE			hpd;
unsigned long	partition_per_sector = HDD_SECTOR_SIZE / 0x80;
unsigned long	tul1;
unsigned long	retval = 0;

unsigned char	buf[HDD_SECTOR_SIZE_4096];

hpd = physical_disk_handle_open_read(pdu);

if (hpd == INVALID_HANDLE_VALUE) {
printf("invalid physical drive number ? %d \n", pdu);
return(0);
}

struct gpt							gpt_info;
SetFilePointerEx(hpd, HDD_SECTOR_SIZE * 1, 0, FILE_BEGIN);
ReadFile(hpd, &gpt_info, HDD_SECTOR_SIZE, &tul1, NULL);

struct gpt_partition_entry_sector	gpe_info;
SetFilePointerEx(hpd, HDD_SECTOR_SIZE*gpt_info.partition_entry_lba, 0, FILE_BEGIN);
ReadFile(hpd, &gpe_info, HDD_SECTOR_SIZE, &tul1, NULL);

if (0 != storagepool_check_gpe(gpe_info,1)) {
CloseHandle(hpd);
return(0);
}
//チェック通る＝gpe_infoの中身を信用してもOKと判断

SetFilePointerEx(hpd, HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8), 0, FILE_BEGIN);
ReadFile(hpd, buf, HDD_SECTOR_SIZE, &tul1, NULL);
CloseHandle(hpd);

if ((*(unsigned long *)(buf + 4) != 0x20202020) ||
(*(unsigned long *)(buf + 0) != 0x43424453)) {
printf("SDBC missing or invalid\n");
return(0);
}

return(
_byteswap_ulong(	*(unsigned long *)	(buf+(
_byteswap_ushort(	*(unsigned short *)	(buf+	0x008		)	)
*0x10)+0x18	)	)
);
//返り値-1までが有効なSDBB、つまり返り値をmaxとしてfor opplを扱えばよい
}
*/


