/*
	Storage pool	inport/export
*/
#include	<windows.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<tchar.h>
#include	"physical_disk.h"
#include	"zlib/zlib.h"

extern	unsigned long	HDD_SECTOR_SIZE;

extern	HANDLE			physical_disk_handle_open_read(unsigned long);
extern	HANDLE			physical_disk_handle_open_write(unsigned long);

extern	unsigned long	storagepool_check_gpe(gpt_partition_entry_sector, unsigned char);
extern	unsigned long	physical_disk_set_sector_size(unsigned long);





void	storagepool_export_sdbfa(unsigned long pdu)		//SpaceDB Full Area
{
	HANDLE			hpd;
	HANDLE			hdb;
	unsigned long	tul1,tul2,tul3;
	LARGE_INTEGER	tli1;
	char			buf[HDD_SECTOR_SIZE_4096];

	hpd = physical_disk_handle_open_read(pdu);
	if (hpd == INVALID_HANDLE_VALUE) {
		printf("invalid physical drive number ? %d \n", pdu);
		return;
	}

	struct gpt							gpt_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpt_info, HDD_SECTOR_SIZE, &tul1, NULL);

	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart = HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpe_info, HDD_SECTOR_SIZE, &tul1, NULL);

	if (0 != storagepool_check_gpe(gpe_info, 1)) {
		CloseHandle(hpd);
		return;
	}

	for(tul3=0;tul3<0x8;tul3++){
		sprintf(buf, "sdbfa_%02d_%02d.bin", pdu,tul3);
		hdb = CreateFile((LPCSTR)buf, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (hdb == INVALID_HANDLE_VALUE) {
			printf("can't create sdbfa_xx_xx.bin file\n");
			CloseHandle(hdb);
			return;
		}

		//SPACEDB から 0x200000セクタ = Full Area ?出力
		for(tul2=0;tul2<0x200000;tul2++){
//			tli1.QuadPart = HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba+tul2+0x200000*tul3);
			tli1.QuadPart = HDD_SECTOR_SIZE*(tul2+0x200000*tul3);
			SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
			ReadFile(hpd, buf, HDD_SECTOR_SIZE, &tul1, NULL);
			WriteFile(hdb, buf, HDD_SECTOR_SIZE, &tul1, NULL);
		}
		CloseHandle(hdb);
	}

	CloseHandle(hpd);
}





void	storagepool_export_spacedb(unsigned long pdu)
{
	HANDLE			hpd;
	HANDLE			hdb;
	unsigned long	tul1;
	LARGE_INTEGER	tli1;
	char			buf[HDD_SECTOR_SIZE_4096];

	hpd = physical_disk_handle_open_read(pdu);
	if (hpd == INVALID_HANDLE_VALUE) {
		printf("invalid physical drive number ? %d \n", pdu);
		return;
	}

	struct gpt							gpt_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpt_info, HDD_SECTOR_SIZE, &tul1, NULL);

	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart = HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpe_info, HDD_SECTOR_SIZE, &tul1, NULL);

	if (0 != storagepool_check_gpe(gpe_info, 1)) {
		CloseHandle(hpd);
		return;
	}

	sprintf(buf, "spacedb_%02d.bin", pdu);
	hdb = CreateFile((LPCSTR)buf, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hdb == INVALID_HANDLE_VALUE) {
		printf("can't create spacedb_xx.bin file\n");
		CloseHandle(hdb);
		return;
	}

	//SPACEDB分
	tli1.QuadPart = HDD_SECTOR_SIZE*gpe_info.entry[1].start_lba;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, buf, HDD_SECTOR_SIZE * 8, &tul1, NULL);
	WriteFile(hdb, buf, HDD_SECTOR_SIZE * 8, &tul1, NULL);

	CloseHandle(hpd);
	CloseHandle(hdb);
}





void	storagepool_inport_spacedb(unsigned long pdu)
{
	HANDLE			hpd;
	HANDLE			hdb;
	unsigned long	tul1;
	LARGE_INTEGER	tli1;
	char			fname[0x100];
	unsigned char	buf[HDD_SECTOR_SIZE_4096];

	hpd = physical_disk_handle_open_write(pdu);
	if (hpd == INVALID_HANDLE_VALUE) {
		printf("invalid physical drive number ? %d \n", pdu);
		return;
	}

	struct gpt							gpt_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpt_info, HDD_SECTOR_SIZE, &tul1, NULL);

	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart = HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpe_info, HDD_SECTOR_SIZE, &tul1, NULL);

	if (0 != storagepool_check_gpe(gpe_info, 1)) {
		CloseHandle(hpd);
		return;
	}

	sprintf(fname, "spacedb_%02d.bin", pdu);
	hdb = CreateFile((LPCSTR)fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hdb == INVALID_HANDLE_VALUE) {
		printf("can't open spacedb_xx.bin file\n");
		CloseHandle(hdb);
		return;
	}

	//SPACEDB分
	tli1.QuadPart=HDD_SECTOR_SIZE*gpe_info.entry[1].start_lba;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hdb, buf, HDD_SECTOR_SIZE * 8, &tul1, NULL);
	*(unsigned long *)(buf + 0x0c) = 0;														//crc32 zero clear
	*(unsigned long *)(buf + 0x0c) = _byteswap_ulong(crc32(0x00000000, buf + 0x00, 0x200));	//再計算
	WriteFile(hpd, buf, HDD_SECTOR_SIZE * 8, &tul1, NULL);

	CloseHandle(hpd);
	CloseHandle(hdb);
}





void	storagepool_export_sdbc(unsigned long pdu)
{
	HANDLE			hpd;
	HANDLE			hdb;
	unsigned long	tul1;
	LARGE_INTEGER	tli1;
	unsigned long	v1, v2, pos;
	unsigned long	sector_start;
	unsigned long	sector_end;
	unsigned long	sector_pos;
	char			buf[HDD_SECTOR_SIZE_4096];

	hpd = physical_disk_handle_open_read(pdu);
	if (hpd == INVALID_HANDLE_VALUE) {
		printf("invalid physical drive number ? %d \n", pdu);
		return;
	}

	struct gpt							gpt_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpt_info, HDD_SECTOR_SIZE, &tul1, NULL);

	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart= HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpe_info, HDD_SECTOR_SIZE, &tul1, NULL);

	if (0 != storagepool_check_gpe(gpe_info, 1)) {
		CloseHandle(hpd);
		return;
	}

	sprintf(buf, "sdbc_%02d.bin", pdu);
	hdb = CreateFile((LPCSTR)buf, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hdb == INVALID_HANDLE_VALUE) {
		printf("can't create sdbc.bin file\n");
		CloseHandle(hdb);
		return;
	}

	//SDBC
	tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8);
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, buf, HDD_SECTOR_SIZE, &tul1, NULL);

	pos = _byteswap_ushort(*(unsigned short *)(buf + 0x008));
	v1 = _byteswap_ulong(*(unsigned long  *)(buf + (pos * 0x10) + 0x14));
	v2 = _byteswap_ulong(*(unsigned long  *)(buf + (pos * 0x10) + 0x18));
	if (v1 & 7)	v1 += 8;
	if (v2 & 7)	v2 += 8;
	v1 >>= 3;
	v2 >>= 3;

	sector_start = gpe_info.entry[1].start_lba + v1;
	sector_end = sector_start + v2;

	printf("export SDBC %08X to %08X\n", sector_start, sector_end);
	for (sector_pos = sector_start; sector_pos != (sector_end + 1); sector_pos++) {
		tli1.QuadPart= HDD_SECTOR_SIZE*sector_pos;
		SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
		ReadFile(hpd, buf, HDD_SECTOR_SIZE, &tul1, NULL);
		WriteFile(hdb, buf, HDD_SECTOR_SIZE, &tul1, NULL);
	}

	CloseHandle(hpd);
	CloseHandle(hdb);
}





void	storagepool_inport_sdbc(unsigned long pdu)
{
	HANDLE			hpd;
	HANDLE			hdb;
	unsigned long	tul1;
	LARGE_INTEGER	tli1;
	unsigned long	sector_pos;
	char			buf[HDD_SECTOR_SIZE_4096];

	physical_disk_set_sector_size(pdu);

	hpd = physical_disk_handle_open_write(pdu);
	if (hpd == INVALID_HANDLE_VALUE) {
		printf("invalid physical drive number ? %d \n", pdu);
		return;
	}

	struct gpt							gpt_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpt_info, HDD_SECTOR_SIZE, &tul1, NULL);

	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart= HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpe_info, HDD_SECTOR_SIZE, &tul1, NULL);

	if (0 != storagepool_check_gpe(gpe_info, 1)) {
		CloseHandle(hpd);
		return;
	}

	sprintf(buf, "sdbc_%02d.bin", pdu);
	hdb = CreateFile((LPCSTR)buf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hdb == INVALID_HANDLE_VALUE) {
		printf("can't open sdbc.bin file\n");
		CloseHandle(hdb);
		return;
	}

	tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8);
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	for (sector_pos = 0;; sector_pos++) {
		ReadFile(hdb, buf, HDD_SECTOR_SIZE, &tul1, NULL);
		if (tul1 != HDD_SECTOR_SIZE)	break;
		WriteFile(hpd, buf, HDD_SECTOR_SIZE, &tul1, NULL);
	}
	printf("inport SDBC %08IX to %08IX done ...\n", gpe_info.entry[1].start_lba + 8, gpe_info.entry[1].start_lba + 8 + sector_pos - 1);

	CloseHandle(hpd);
	CloseHandle(hdb);
}





void	storagepool_clear_sdbc(unsigned long pdu)
{
	HANDLE			hpd;
	unsigned long	tul1;
	LARGE_INTEGER	tli1;
	unsigned long	v1, v2, pos;
	unsigned long	sector_start;
	unsigned long	sector_end;
	unsigned long	sector_pos;
	char			buf[HDD_SECTOR_SIZE_4096];

	hpd = physical_disk_handle_open_write(pdu);
	if (hpd == INVALID_HANDLE_VALUE) {
		printf("invalid physical drive number ? %d \n", pdu);
		return;
	}

	struct gpt							gpt_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpt_info, HDD_SECTOR_SIZE, &tul1, NULL);

	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart= HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, &gpe_info, HDD_SECTOR_SIZE, &tul1, NULL);

	if (0 != storagepool_check_gpe(gpe_info, 1)) {
		CloseHandle(hpd);
		return;
	}

	//SDBC
	tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8);
	SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
	ReadFile(hpd, buf, HDD_SECTOR_SIZE, &tul1, NULL);

	pos = _byteswap_ushort(*(unsigned short *)(buf + 0x008));
	v1 = _byteswap_ulong(*(unsigned long  *)(buf + (pos * 0x10) + 0x14));
	v2 = _byteswap_ulong(*(unsigned long  *)(buf + (pos * 0x10) + 0x18));
	if (v1 & 7)	v1 += 8;
	if (v2 & 7)	v2 += 8;
	v1 >>= 3;
	v2 >>= 3;

	sector_start = gpe_info.entry[1].start_lba + v1;
	sector_end = sector_start + v2;

	printf("clear SDBC %08X to %08X\n", sector_start, sector_end);
	for (sector_pos = sector_start; sector_pos != (sector_end + 1); sector_pos++) {
		tli1.QuadPart= HDD_SECTOR_SIZE*sector_pos;
		SetFilePointerEx(hpd,tli1, 0, FILE_BEGIN);
		memset(buf, 0, HDD_SECTOR_SIZE);
		WriteFile(hpd, buf, HDD_SECTOR_SIZE, &tul1, NULL);
	}

	CloseHandle(hpd);
}
