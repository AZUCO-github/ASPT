/*
	Storage pool
*/
#include	<windows.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<tchar.h>
#include	"physical_disk.h"
#include	"zlib/zlib.h"

extern	unsigned long	HDD_SECTOR_SIZE;
extern	unsigned long	physical_disk_number_max;

extern	void			disp_get_lasterror(const char *);
extern	HANDLE			physical_disk_handle_open_read(unsigned long);
extern	HANDLE			physical_disk_handle_open_write(unsigned long);
extern	void			physical_disk_number_max_get(void);
extern	unsigned long	physical_disk_set_sector_size(unsigned long);
extern	void			disp_dump(unsigned char *,unsigned long,unsigned long);
extern	void			disp_16byte(unsigned char *);
extern	void			disp_guid(unsigned char *);
extern	void			high_byte_low_byte_swap(char *,char *,unsigned long);
extern	unsigned long	storagepool_check_gpe(gpt_partition_entry_sector,unsigned char);
extern	unsigned long	storagepool_check_sdbc(unsigned long);

		unsigned long	undisplay_sdbb_chain_04;
		unsigned long	undisplay_sdbb_chain_03_name_length_zero;




void	get_length_and_value(unsigned char *buf,unsigned long *len, ULONG64 *val)
{
	unsigned long	tul1;

	*len = *buf;
	if (*len == 0) {
		*val=0;
	}else{
		for(*val=0,tul1=0;*len>tul1;tul1++){
			*val<<= 8;
			*val|=*(buf+1+tul1);
		}
	}
}




void	storagepool_disp_spacedb(unsigned long pdu,unsigned char is_secondary)
{
	HANDLE			hpd;
	unsigned long	partition_per_sector=HDD_SECTOR_SIZE/0x80;
	LARGE_INTEGER	tli1;
	unsigned long	tul1;
	unsigned long	crc32_org;
	unsigned long	crc32_cmp;

	unsigned char	buf[HDD_SECTOR_SIZE_4096];

	hpd=physical_disk_handle_open_read(pdu);

	if(hpd==INVALID_HANDLE_VALUE){
		printf("invalid physical drive number ? %d \n",pdu);
		return;
	}

	struct gpt							gpt_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
	ReadFile(hpd,&gpt_info,HDD_SECTOR_SIZE,&tul1,NULL); 

	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart= HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
	ReadFile(hpd,&gpe_info,HDD_SECTOR_SIZE,&tul1,NULL); 

	if(0!=storagepool_check_gpe(gpe_info,1))	goto	func_close;
	//チェック通る＝gpe_infoの中身を信用してもOKと判断

	if(!is_secondary)	tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba+0x00000);
	else				tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba+0xffff8);
	SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
	ReadFile(hpd,buf,HDD_SECTOR_SIZE,&tul1,NULL); 

	printf("SAPACEDB\tphysical disk number %02d\n",pdu);
	printf("signature\t\t\t%c%c%c%c%c%c%c%c\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
	printf("offset(paragraph unit)\t\t%04X\n",	_byteswap_ushort(*(unsigned short *)(buf+0x008)));	//big endian
	printf("SAPACEDB size\t\t\t%04X\n",			_byteswap_ushort(*(unsigned short *)(buf+0x00a)));	//big endian
	printf("SAPACEDB CRC32\t\t\t%08X\n",		_byteswap_ulong (*(unsigned long  *)(buf+0x00c)));	//big endian
	switch (*(buf + 0x041)) {
		case	0x00:	printf("SDBB\t\t\t\t00 : old or missing\n");				break;
		case	0x01:	printf("SDBB\t\t\t\t01 : current\n");						break;
		default:		printf("SDBB\t\t\t\t%02X : invalid ?\n", *(buf + 0x041));	break;
	}

	printf("storage pool GUID\t\t");
	disp_16byte(	buf+0x10*_byteswap_ushort(*(unsigned short *)(buf+0x008))+0x00);
	printf(" ");
	disp_guid(		buf+0x10*_byteswap_ushort(*(unsigned short *)(buf+0x008))+0x00);
	printf("\n");

	printf("SPACEDB GUID\t\t\t");
	disp_16byte(	buf+0x10*_byteswap_ushort(*(unsigned short *)(buf+0x008))+0x10);
	printf(" ");
	disp_guid(		buf+0x10*_byteswap_ushort(*(unsigned short *)(buf+0x008))+0x10);
	printf("\n");

	disp_dump(buf, 512, 1);
	crc32_org=*(unsigned long  *)(buf+0x00c);
	*(unsigned long *)(buf+0x0c)=0;
	crc32_cmp=crc32(0x00000000,buf+0x00,0x200);

	if(crc32_cmp!=_byteswap_ulong(crc32_org)){
		printf("warning : invalid SPACEDB CRC32 ?\n"); 
		printf("CRC32=%08X / MEM+C=%08X\n",crc32_cmp,_byteswap_ulong(crc32_org));
	}

func_close:
	CloseHandle(hpd);
}



void	storagepool_disp_sdbc(unsigned long pdu,unsigned char is_secondary)
{
	HANDLE			hpd;
	unsigned long	partition_per_sector=HDD_SECTOR_SIZE/0x80;
	unsigned long	tul1;
	LARGE_INTEGER	tli1;
	unsigned long	crc32_org;
	unsigned long	crc32_cmp;
	unsigned long	pos;
	unsigned long	v1;
	unsigned long	v2;

	unsigned char	buf[HDD_SECTOR_SIZE_4096];

	hpd=physical_disk_handle_open_read(pdu);

	if(hpd==INVALID_HANDLE_VALUE){
		printf("invalid physical drive number ? %d \n",pdu);
		return;
	}

	struct gpt							gpt_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
	ReadFile(hpd,&gpt_info,HDD_SECTOR_SIZE,&tul1,NULL); 

	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart= HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
	ReadFile(hpd,&gpe_info,HDD_SECTOR_SIZE,&tul1,NULL); 

	if (0 != storagepool_check_gpe(gpe_info,1)) {
		CloseHandle(hpd);
		return;
	}

	//チェック通る＝gpe_infoの中身を信用してもOKと判断
	if(!is_secondary)	tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8 +0x00000);
	else				tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8 +0xffff8);
	SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
	ReadFile(hpd,buf,HDD_SECTOR_SIZE,&tul1,NULL); 
	CloseHandle(hpd);

	if(	(*(unsigned long *)(buf+4) != 0x20202020) ||
		(*(unsigned long *)(buf+0) != 0x43424453)		){
		printf("SDBC missing or invalid\n");
		return;
	}

	pos = _byteswap_ushort(*(unsigned short *)(buf + 0x008)) * 0x10;

	printf("SDBC\tphysical disk number %02d\n",pdu);
	printf("signature\t\t\t%c%c%c%c%c%c%c%c\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
	printf("offset(paragraph unit)\t\t%04X\n",	_byteswap_ushort(*(unsigned short *)(buf+0x008)));	//big endian
	printf("SDBC size\t\t\t%04X\n",				_byteswap_ushort(*(unsigned short *)(buf+0x00a)));	//big endian
	printf("SDBC CRC32\t\t\t%08X\n",			_byteswap_ulong (*(unsigned long  *)(buf+0x00c)));	//big endian

	printf("storage pool GUID\t\t");
	disp_16byte(	buf+pos);
	printf(" ");
	disp_guid(		buf+pos);
	printf("\n");

	v1 = _byteswap_ulong(*(unsigned long *)(buf + pos + 0x14));
	v2 = _byteswap_ulong(*(unsigned long *)(buf + pos + 0x18));
	
	printf("SDBB start position\t\t%08X\tstart sector\t= %08X\n",v1,	gpe_info.entry[1].start_lba+1+(v1>>3));
	printf("SDBB length\t\t\t%08X\tend sector\t= %08X\n",v2,			gpe_info.entry[1].start_lba+1+(v1>>3)+(v2>>3)-1);

	printf("command serial number\t\t%016I64X\n", 
		_byteswap_uint64(*(ULONG64 *)(buf + pos + 0x28)));
	printf("command serial number\t\t%016I64X\n",
		_byteswap_uint64(*(ULONG64 *)(buf + pos + 0x30)));

	printf("SDBB CRC32\t\t\t%08X\n", _byteswap_ulong(*(unsigned long  *)(buf + pos + 0x3c)));	//big endian

	disp_dump(buf, 512, 1);
	crc32_org=*(unsigned long  *)(buf+0x00c);
	*(unsigned long *)(buf+0x0c)=0;
	crc32_cmp=crc32(0x00000000,buf+0x00,0x200);

	if(crc32_cmp!=_byteswap_ulong(crc32_org)){
		printf("warning : invalid SDBC CRC32 ?\n"); 
		printf("CRC32=%08X / MEM+C=%08X\n",crc32_cmp,_byteswap_ulong(crc32_org));
	}
}



void	storagepool_disp_sdbb(unsigned long pdu,unsigned char is_secondary)
{
	char			mbs[0x100];
	char			wcs[0x100];
	HANDLE			hpd;
	unsigned long	partition_per_sector=HDD_SECTOR_SIZE/0x80;
	unsigned long	tul1;
	LARGE_INTEGER	tli1;

	unsigned char	buf1[HDD_SECTOR_SIZE_4096];
	unsigned long	buff_pos1;
	unsigned long	sdbb_lcnt1;	//loop count
	unsigned long	sdbb_npos1;	//+4	SDBBのカレント位置
	unsigned long	sdbb_cfrm1;	//+8	現在のSDBBチェーンがどのSDBB由来か
	unsigned long	sdbb_cpos1;	//+c	現在のSDBBチェーン位置
	unsigned long	sdbb_cmax1;	//+e	現在のSDBBチェーンの長さ

	unsigned char	buf2[HDD_SECTOR_SIZE_4096];
	unsigned long	buff_pos2;
	unsigned long	sdbb_lcnt2;	//loop count
	unsigned long	sdbb_npos2;	//+4	SDBBのカレント位置
	unsigned long	sdbb_cfrm2;	//+8	現在のSDBBチェーンがどのSDBB由来か
	unsigned long	sdbb_cpos2;	//+c	現在のSDBBチェーン位置
	unsigned long	sdbb_cmax2;	//+e	現在のSDBBチェーンの長さ

	unsigned long	sdbb_ccnt2;	//		現在のSDBBチェーンを書いたカウント

	unsigned long	sdbb_pmax;	//SDBBのnposのmax値+1
	unsigned long	sdbb_cmax;	//SDBBのcmaxの最大値
	unsigned char	*bufb;

	unsigned long	sdbb_chain_npos;	//現在の位置
	unsigned long	sdbb_chain_tlen;	//長さを入れるための物
	ULONG64			sdbb_chain_tval;	//値を入れるための物

	hpd=physical_disk_handle_open_read(pdu);
	if(hpd==INVALID_HANDLE_VALUE){
		printf("invalid physical drive number ? %d \n",pdu);
		return;
	}

	struct gpt							gpt_info;
	tli1.QuadPart= HDD_SECTOR_SIZE * 1;
	SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
	ReadFile(hpd,&gpt_info,HDD_SECTOR_SIZE,&tul1,NULL); 

	struct gpt_partition_entry_sector	gpe_info;
	tli1.QuadPart= HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
	ReadFile(hpd,&gpe_info,HDD_SECTOR_SIZE,&tul1,NULL); 

	if (0 != storagepool_check_gpe(gpe_info,1)) {
		CloseHandle(hpd);
		return;
	}

	if (1 != storagepool_check_sdbc(pdu)) {
		CloseHandle(hpd);
		printf("SDBC missing or invalid\n");
		return;
	}



	for(sdbb_npos1=8,sdbb_cmax=0,sdbb_pmax=0;;sdbb_npos1++){	//csize_max/sdbb_max check
		if(sdbb_npos1%8==0){
			if(!is_secondary)	tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8 + (sdbb_npos1 >> 3)+0x00000);
			else				tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8 + (sdbb_npos1 >> 3)+0xffff8);
			SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
			ReadFile(hpd,buf1,HDD_SECTOR_SIZE,&tul1,NULL);
		}

		buff_pos1=(sdbb_npos1%8)*0x40;

		if(	(buf1[buff_pos1+0]!='S')	|
			(buf1[buff_pos1+1]!='D')	|
			(buf1[buff_pos1+2]!='B')	|
			(buf1[buff_pos1+3]!='B')		){
			sdbb_pmax=sdbb_npos1;		//sdbb_maxは既にsdbbではない、-1まではsdbb
			break;
		}
		sdbb_cfrm1=_byteswap_ulong(*(unsigned long *)(buf1+buff_pos1+0x8));
		if(0!=sdbb_cfrm1){
			sdbb_cmax1=_byteswap_ushort(*(unsigned short *)(buf1+buff_pos1+0xe));			//big endian
			if(sdbb_cmax<sdbb_cmax1)
				sdbb_cmax=sdbb_cmax1;
		}
	}
	printf("SDBB\tphysical disk number %02d\n",pdu);
	printf("SDBB MAX=%08X Chain_MAX=%04X\n",sdbb_pmax,sdbb_cmax);



//	SDBB chainの調査
	bufb=new unsigned char[sdbb_cmax*0x30];

	for(sdbb_lcnt1=8;sdbb_lcnt1!=sdbb_pmax;sdbb_lcnt1++){
		if(sdbb_lcnt1%8==0){
			if(!is_secondary)	tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8 + (sdbb_lcnt1 >> 3)+0x00000);
			else				tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8 + (sdbb_lcnt1 >> 3)+0xffff8);
			SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
			ReadFile(hpd,buf1,HDD_SECTOR_SIZE,&tul1,NULL);
		}

		buff_pos1=(sdbb_lcnt1%8)*0x40;
		sdbb_npos1=_byteswap_ulong (*(unsigned long  *)(buf1+buff_pos1+0x4));
		sdbb_cfrm1=_byteswap_ulong (*(unsigned long  *)(buf1+buff_pos1+0x8));
		sdbb_cpos1=_byteswap_ushort(*(unsigned short *)(buf1+buff_pos1+0xc));
		sdbb_cmax1=_byteswap_ushort(*(unsigned short *)(buf1+buff_pos1+0xe));
//printf("SDBB L1 CNT=%08X NOW=%08X 0!=%08X && 0==%4X\n",sdbb_lcnt1,sdbb_npos1, sdbb_cfrm1, sdbb_cpos1);
		if((0!=sdbb_cfrm1)&&(0==sdbb_cpos1)){
			if(1==sdbb_cmax1){	//chain maxが1なら、現在の物で完結なので探す必要が無いので
				memcpy(bufb,buf1+buff_pos1+0x10,0x30);
			}else{
				for(sdbb_lcnt2=8,sdbb_ccnt2=0;sdbb_lcnt2!=sdbb_pmax;sdbb_lcnt2++){	//sbdd_lc2=fromが同じものを先頭からピックアップ
					if(sdbb_lcnt2%8==0){
						if(!is_secondary)	tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8 + (sdbb_lcnt2 >> 3)+0x00000);
						else				tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8 + (sdbb_lcnt2 >> 3)+0xffff8);
						SetFilePointerEx(hpd,tli1,0,FILE_BEGIN);
						ReadFile(hpd,buf2,HDD_SECTOR_SIZE,&tul1,NULL);
					}
	
					buff_pos2=(sdbb_lcnt2%8)*0x40;
					sdbb_npos2=_byteswap_ulong (*(unsigned long  *)(buf2+buff_pos2+0x4));
					sdbb_cfrm2=_byteswap_ulong (*(unsigned long  *)(buf2+buff_pos2+0x8));
					sdbb_cpos2=_byteswap_ushort(*(unsigned short *)(buf2+buff_pos2+0xc));
					sdbb_cmax2=_byteswap_ushort(*(unsigned short *)(buf2+buff_pos2+0xe));

//printf("SDBB L2 CNT=%08X NOW=%08X\n",sdbb_lcnt2,sdbb_npos2);
					if(sdbb_cfrm2==sdbb_cfrm1){		//fromが同一のchainを集める
						memcpy(bufb+0x30*sdbb_cpos2,buf2+buff_pos2+0x10,0x30);
						sdbb_ccnt2++;
						if(sdbb_ccnt2==sdbb_cmax1){	//想定回数を埋めたら
							sdbb_lcnt2=sdbb_pmax-1;	//強制終了
						}
					}
				}
			}

			switch(*bufb){		//chainの内容の表示
				case	0x01:	//記憶域の情報
					printf("SDBB bpos=%08X from=%08X cpos=%04X size=%04X\n",sdbb_npos1,sdbb_cfrm1,sdbb_cpos1,sdbb_cmax1);

					printf("Command 01 : storage pool\n");

					sdbb_chain_npos = 0;
					sdbb_chain_npos += 1;	//Command number
					sdbb_chain_npos += 3;	//謎

					sdbb_chain_tlen = _byteswap_ulong(*(unsigned long *)(bufb + sdbb_chain_npos));
					printf("L1 chain data length\t\t\t%08X\n", sdbb_chain_tlen);
					sdbb_chain_npos += 4;

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D2
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D2 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D3 command serial number\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					printf("Storage Pool GUID\t\t\t");
					disp_16byte(bufb + sdbb_chain_npos);
					printf(" ");
					disp_guid(bufb + sdbb_chain_npos);
					printf("\n");
					sdbb_chain_npos += 0x10;

					sdbb_chain_tlen = _byteswap_ushort(*(unsigned short *)(bufb + sdbb_chain_npos));
					printf("L4 storage pool friendly name length\t%04X\n", sdbb_chain_tlen);
					sdbb_chain_npos += 2;

					memset(mbs, 0, 0x100);
					memset(wcs, 0, 0x100);
					high_byte_low_byte_swap(wcs, (char *)bufb + sdbb_chain_npos, 2 * sdbb_chain_tlen);	//必要
					WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, (LPCWCH)wcs, sdbb_chain_tlen, mbs, 0x100, NULL, NULL);
					printf("D4 storage pool friendly name\t\t%s\n", mbs);
					sdbb_chain_npos += sdbb_chain_tlen*2;

					sdbb_chain_npos += 3;		//謎	00 00 00

					sdbb_chain_tval = *(unsigned char *)(bufb + sdbb_chain_npos);
					printf("D5 \t\t\t\t\t%02llX\n", sdbb_chain_tval);
					sdbb_chain_npos += 1;

					sdbb_chain_npos += 6;		//謎	13 09 0C 46 01 01

					sdbb_chain_tval = *(unsigned char *)(bufb + sdbb_chain_npos);
					switch (sdbb_chain_tval) {
						case	0x01:	printf("D6 ProvisioningTypeDefault\t\t01 : thin\n");						break;
						case	0x02:	printf("D6 ProvisioningTypeDefault\t\t02 : fixed\n");						break;
						default:		printf("D6 ProvisioningTypeDefault\t\t%02llX : invalid ?\n",sdbb_chain_tval);	break;
					}
					sdbb_chain_npos += 1;

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D7
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D7 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					sdbb_chain_npos += 4;		//謎	00 01 01 02

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D8
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D8 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					sdbb_chain_npos += 2;		//謎	00 00

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D9
					printf("D9 \t\t\t\t%02X\tso long\n", sdbb_chain_tlen);			//長いので表示しない
					sdbb_chain_npos += 1 + sdbb_chain_tlen;

					sdbb_chain_npos += 6;		//謎	01 00 01 01 01 01

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//DA
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("DA \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					sdbb_chain_npos += 8;		//謎	12 02 01 01 01 02 01 01

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//DB
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("DB \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					sdbb_chain_npos += 6;		//謎	12 03 01 01 01 01

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//DC
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("DC \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//DD
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("DD \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					sdbb_chain_npos += 6;		//謎	12

					disp_dump(bufb,0x30*sdbb_cmax1,0);
					break;

				case	0x02:	//storage poolに参加しているphysical diskの情報
					printf("SDBB bpos=%08X from=%08X cpos=%04X size=%04X\n",sdbb_npos1,sdbb_cfrm1,sdbb_cpos1,sdbb_cmax1);
					printf("Command 02 : physical disk\n");

					sdbb_chain_npos = 0;
					sdbb_chain_npos += 1;	//Command number
					sdbb_chain_npos += 3;	//謎

					sdbb_chain_tlen = _byteswap_ulong(*(unsigned long *)(bufb + sdbb_chain_npos));
					printf("L1 chain data length\t\t\t%08X\n", sdbb_chain_tlen);
					sdbb_chain_npos += 4;

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D2
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D2 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D3 command serial number\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					printf("SPACEDB GUID\t\t\t\t");
					disp_16byte(bufb + sdbb_chain_npos);
					printf(" ");
					disp_guid(bufb + sdbb_chain_npos);
					printf("\n");
					sdbb_chain_npos += 0x10;

					sdbb_chain_tlen = _byteswap_ushort(*(unsigned short *)(bufb + sdbb_chain_npos));
					printf("L4 physical disk friendly name length\t%04X\n", sdbb_chain_tlen);
					sdbb_chain_npos += 2;

					memset(mbs, 0, 0x100);
					memset(wcs, 0, 0x100);
					high_byte_low_byte_swap(wcs, (char *)bufb + sdbb_chain_npos, 2 * sdbb_chain_tlen);	//必要
					WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, (LPCWCH)wcs, sdbb_chain_tlen, mbs, 0x100, NULL, NULL);
					printf("D4 physical disk friendly name\t\t%s\n", mbs);
					sdbb_chain_npos += sdbb_chain_tlen * 2;

					sdbb_chain_npos += 3;		//謎

					sdbb_chain_tval = *(unsigned char *)(bufb + sdbb_chain_npos);
					switch (sdbb_chain_tval) {
						case	0x00:	printf("D5 SDBB\t\t\t\t\t00 : old or missing\n");				break;
						case	0x02:	printf("D5 SDBB\t\t\t\t\t02 : current\n");						break;
						default:		printf("D5 SDBB\t\t\t\t\t%02llX : invalid ?\n", sdbb_chain_tval);	break;
					}
					sdbb_chain_npos += 1;

					sdbb_chain_tval = *(unsigned char *)(bufb + sdbb_chain_npos);
					switch (sdbb_chain_tval) {
						case	0x01:	printf("D6 Usage\t\t\t\t01 : Auto-Select (default)\n");			break;
						case	0x02:	printf("D6 Usage\t\t\t\t02 : Manual-Select\n");					break;
						case	0x03:	printf("D6 Usage\t\t\t\t03 : Hot Spare\n");						break;
						case	0x04:	printf("D6 Usage\t\t\t\t04 : Journal\n");						break;
						case	0x05:	printf("D6 Usage\t\t\t\t05 : Retired\n");						break;
						default:		printf("D6 Usage\t\t\t\t%02llX : invalid ?\n", sdbb_chain_tval);	break;
					}
					sdbb_chain_npos += 1;

					sdbb_chain_tval = *(unsigned char *)(bufb + sdbb_chain_npos);
					switch (sdbb_chain_tval) {
						case	0x00:	printf("D7 Media type\t\t\t\t00 : Unspecified\n");					break;
						case	0x01:	printf("D7 Media type\t\t\t\t01 : HDD\n");							break;
						default:		printf("D7 Media type\t\t\t\t%02llX : invalid ?\n", sdbb_chain_tval);	break;
					}
					sdbb_chain_npos += 1;

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D8 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D9 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					disp_dump(bufb, 0x30 * sdbb_cmax1, 0);
					break;

				case	0x03:	//ボリュームの情報
					if(	undisplay_sdbb_chain_03_name_length_zero	&&				//長さゼロは表示しないスイッチ
						(0==_byteswap_ushort(*(unsigned short *)(bufb+0x01e))))		//長さがゼロか？
						break;
					printf("SDBB bpos=%08X from=%08X cpos=%04X size=%04X\n",sdbb_npos1,sdbb_cfrm1,sdbb_cpos1,sdbb_cmax1);

					printf("Command 03 : virtual disk\n");

					sdbb_chain_npos = 0;
					sdbb_chain_npos += 1;	//Command number
					sdbb_chain_npos += 3;	//謎

					sdbb_chain_tlen = _byteswap_ulong(*(unsigned long *)(bufb + sdbb_chain_npos));
					printf("L1 chain data length\t\t\t%08X\n", sdbb_chain_tlen);
					sdbb_chain_npos += 4;

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D2
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D2 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D3 command serial number\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					printf("Storage Pool GUID\t\t\t");
					disp_16byte(bufb + sdbb_chain_npos);
					printf(" ");
					disp_guid(bufb + sdbb_chain_npos);
					printf("\n");
					sdbb_chain_npos += 0x10;

					sdbb_chain_tlen = _byteswap_ushort(*(unsigned short *)(bufb + sdbb_chain_npos));
					printf("L4 virtual disk friendly name length\t%04X\n", sdbb_chain_tlen);
					sdbb_chain_npos += 2;

					memset(mbs, 0, 0x100);
					memset(wcs, 0, 0x100);
					high_byte_low_byte_swap(wcs, (char *)bufb + sdbb_chain_npos, 2 * sdbb_chain_tlen);	//必要
					WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, (LPCWCH)wcs, sdbb_chain_tlen, mbs, 0x100, NULL, NULL);
					printf("D4 virtual disk friendly name\t\t%s\n", mbs);
					sdbb_chain_npos += sdbb_chain_tlen * 2;

					printf("unknown\t\t\t\t\t%02X %02X %02X %02X %02X\n",
						*(bufb + sdbb_chain_npos + 0),
						*(bufb + sdbb_chain_npos + 1),
						*(bufb + sdbb_chain_npos + 2),
						*(bufb + sdbb_chain_npos + 3),
						*(bufb + sdbb_chain_npos + 4)
					);
					sdbb_chain_npos += 5;		//謎

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D5
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D5 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D6
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D6 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					sdbb_chain_tval = *(unsigned char *)(bufb + sdbb_chain_npos);
					switch (sdbb_chain_tval) {
						case	0x01:	printf("D7 ProvisioningType\t\t\t01 : thin\n");								break;
						case	0x02:	printf("D7 ProvisioningType\t\t\t02 : fixed\n");							break;
						default:		printf("D7 ProvisioningType\t\t\t%02llX : invalid ?\n", sdbb_chain_tval);	break;
					}
					sdbb_chain_npos += 1;

					printf("unknown\t\t\t\t\t%02X %02X %02X %02X %02X %02X %02X %02X %02X\n",
						*(bufb + sdbb_chain_npos + 0),
						*(bufb + sdbb_chain_npos + 1),
						*(bufb + sdbb_chain_npos + 2),
						*(bufb + sdbb_chain_npos + 3),
						*(bufb + sdbb_chain_npos + 4),
						*(bufb + sdbb_chain_npos + 5),
						*(bufb + sdbb_chain_npos + 6),
						*(bufb + sdbb_chain_npos + 7),
						*(bufb + sdbb_chain_npos + 8)
					);
					sdbb_chain_npos += 9;		//謎

					sdbb_chain_tval = *(unsigned char *)(bufb + sdbb_chain_npos);
					switch (sdbb_chain_tval) {
						case	0x01:	printf("D8 ResiliencySettingName\t\t01 : Simple\n");							break;
						case	0x02:	printf("D8 ResiliencySettingName\t\t02 : Mirror\n");							break;
						case	0x03:	printf("D8 ResiliencySettingName\t\t03 : Parity\n");							break;
						default:		printf("D8 ResiliencySettingName\t\t%02llX : invalid ?\n", sdbb_chain_tval);	break;
					}
					sdbb_chain_npos += 1;

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D9
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D9 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D10
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D10 number of copies\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);	//D11
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D11 number of columns\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					printf("unknown\t\t\t\t\t%02X %02X %02X %02X\n",
						*(bufb + sdbb_chain_npos + 0),
						*(bufb + sdbb_chain_npos + 1),
						*(bufb + sdbb_chain_npos + 2),
						*(bufb + sdbb_chain_npos + 3)
					);

					disp_dump(bufb,0x30*sdbb_cmax1,0);
					break;



				case	0x04:
					if(undisplay_sdbb_chain_04)	break;
					printf("SDBB bpos=%08X from=%08X cpos=%04X size=%04X\n",sdbb_npos1,sdbb_cfrm1,sdbb_cpos1,sdbb_cmax1);

					printf("Command 04 : slab\n");

					sdbb_chain_npos = 0;
					sdbb_chain_npos += 1;	//Command number
					sdbb_chain_npos += 3;	//謎

					sdbb_chain_tlen = _byteswap_ulong(*(unsigned long *)(bufb + sdbb_chain_npos));
					printf("L1 chain data length\t\t\t%08X\n", sdbb_chain_tlen);
					sdbb_chain_npos += 4;

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D2 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D3 command serial number\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					sdbb_chain_npos += 6;	//謎

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D4 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D5 virtual disk number\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D6 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D7 \t\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb + sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D8 physical disk number\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					get_length_and_value(bufb+ sdbb_chain_npos, &sdbb_chain_tlen, &sdbb_chain_tval);
					sdbb_chain_npos += 1 + sdbb_chain_tlen;
					printf("D9 slab number\t\t\t%02X\t%08llX\n", sdbb_chain_tlen, sdbb_chain_tval);

					disp_dump(bufb,0x30*sdbb_cmax1,0);
					break;



				default:
					printf("SDBB bpos=%08X from=%08X cpos=%04X size=%04X\n",sdbb_npos1,sdbb_cfrm1,sdbb_cpos1,sdbb_cmax1);
					printf("unknown command !\n");
					disp_dump(bufb,0x30*sdbb_cmax1,0);
					break;
			}
		}
	}

	CloseHandle(hpd);
}





void	storagepool_check_sdbb(void)
{
	HANDLE			hpd[101];
	unsigned char	status[101];
	ULONG64			serial[101];
	unsigned long	sector[101];

	unsigned char	buf[HDD_SECTOR_SIZE_4096];
	unsigned char	src[HDD_SECTOR_SIZE_4096];
	unsigned long	pns;
	unsigned long	sector_start;
	unsigned long	sector_end;
	unsigned long	tul1,tul2,tul3;
	LARGE_INTEGER	tli1;

	struct gpt							gpt_info;
	struct gpt_partition_entry_sector	gpe_info;


	physical_disk_number_max_get();

	for (serial[physical_disk_number_max]=0,tul2 = 0; tul2 < physical_disk_number_max; tul2++) {
		hpd[tul2]		=physical_disk_handle_open_read(tul2);
		status[tul2]	=0;		//missing or invalid
		serial[tul2]	=0;

		if (hpd[tul2] == INVALID_HANDLE_VALUE) {
			hpd[tul2] = 0;
			goto	loop_1_end;
		}

		sector[tul2]=physical_disk_set_sector_size(tul2);

		tli1.QuadPart= HDD_SECTOR_SIZE * 1;
		SetFilePointerEx(hpd[tul2],tli1, 0, FILE_BEGIN);
		ReadFile(hpd[tul2], &gpt_info, HDD_SECTOR_SIZE, &tul1, NULL);

		tli1.QuadPart= HDD_SECTOR_SIZE*gpt_info.partition_entry_lba;
		SetFilePointerEx(hpd[tul2],tli1, 0, FILE_BEGIN);
		ReadFile(hpd[tul2], &gpe_info, HDD_SECTOR_SIZE, &tul1, NULL);

		if(0!= storagepool_check_gpe(gpe_info,0)){
			CloseHandle(hpd[tul2]);
			hpd[tul2] =0;
			goto	loop_1_end;
		}

		//チェック通る＝gpe_infoの中身を信用してもOKと判断
		tli1.QuadPart= HDD_SECTOR_SIZE*(gpe_info.entry[1].start_lba + 8);
		SetFilePointerEx(hpd[tul2],tli1, 0, FILE_BEGIN);
		ReadFile(hpd[tul2], buf, HDD_SECTOR_SIZE, &tul1, NULL);

		if ((*(unsigned long *)(buf + 4) != 0x20202020) ||
			(*(unsigned long *)(buf + 0) != 0x43424453)) {
			CloseHandle(hpd[tul2]);
			hpd[tul2] = 0;
			goto	loop_1_end;
		}

		status[tul2] = 1;	//old
		serial[tul2]=	_byteswap_uint64(*(ULONG64 *)		(buf+(
						_byteswap_ushort(*(unsigned short *)(buf+0x008))*0x10
																		)+0x28));
//		CloseHandle(hpd[tul2]);

		if (serial[physical_disk_number_max] < serial[tul2])
			serial[physical_disk_number_max] = serial[tul2];
	loop_1_end:
		NULL;
	}

	printf("SDBB check\n");
	printf("PD\tserial value\t\t\t\tsector\tstatus\n");
	for (tul2 = 0; tul2 < physical_disk_number_max; tul2++) {
		printf("%02d\t", tul2);
		printf("%032I64X\t", serial[tul2]);
		if (serial[physical_disk_number_max] == serial[tul2])	status[tul2] = 2;
		printf("%04d\t", sector[tul2]);
		switch (status[tul2]) {
			case	0:	printf("missing or invalid\n");	break;
			case	1:	printf("old\n");				break;
			case	2:	printf("current\n");			break;
			default:	printf("invalid status\n");		break;
		}
	}
	printf("\n");

	printf("SDBB coherence check\n");
	for (tul2 = 0; tul2 < physical_disk_number_max; tul2++) {
		if (status[tul2] == 2) {
			pns = tul2;		//一番最初のcurrentのPhysical drive Numberを記録する。以降これがsrcになる
			break;
		}
	}

	tli1.QuadPart= sector[tul2] * 1;
	SetFilePointerEx(hpd[tul2],tli1, 0, FILE_BEGIN);
	ReadFile(hpd[tul2], &gpt_info, sector[tul2], &tul1, NULL);

	tli1.QuadPart= sector[tul2] * gpt_info.partition_entry_lba;
	SetFilePointerEx(hpd[tul2],tli1, 0, FILE_BEGIN);
	ReadFile(hpd[tul2], &gpe_info, sector[tul2], &tul1, NULL);

	tli1.QuadPart= sector[tul2] * (gpe_info.entry[1].start_lba + 8);
	SetFilePointerEx(hpd[tul2],tli1, 0, FILE_BEGIN);
	ReadFile(hpd[tul2], buf, sector[tul2], &tul1, NULL);

	sector_start=	gpe_info.entry[1].start_lba	+	1	+
					(	(_byteswap_ulong (*(unsigned long  *)(buf+ 
						(_byteswap_ushort(*(unsigned short *)(buf+0x008	))*0x10	)+0x14
																		))		) >> 3);
	sector_end	=	sector_start	+
					(	(_byteswap_ulong (*(unsigned long  *)(buf+ 
						(_byteswap_ushort(*(unsigned short *)(buf+0x008	))*0x10	)+0x18
																		))		) >> 3);

	printf("PD\tPD\tincoherence sector\n");
	for (tul3 = sector_start; tul3 < sector_end; tul3++) {
		for (tul2 = pns; tul2 < physical_disk_number_max; tul2++) {
			if (tul2 == pns) {
				tli1.QuadPart= sector[pns] * tul3;
				SetFilePointerEx(hpd[pns],tli1, 0, FILE_BEGIN);
				ReadFile(hpd[pns], src, sector[pns], &tul1, NULL);
				continue;
			}

			if( (sector[pns] != sector[tul2]	) ||		//セクタサイズが違う（多分vd）
				(status[tul2]!= 2				)		)	//currentではない
				continue;

			tli1.QuadPart= sector[tul2] * tul3;
			SetFilePointerEx(hpd[tul2],tli1, 0, FILE_BEGIN);
			ReadFile(hpd[tul2], buf, sector[tul2], &tul1, NULL);

			if (0 != memcmp(src, buf, sector[pns])) {
				printf("%02d\t%02d\t%08X\n", pns, tul2, tul3);
			}
		}
	}
	printf("done...\n");

	for (tul2 = 0; tul2 < physical_disk_number_max; tul2++) {
		CloseHandle(hpd[tul2]);
	}
}
