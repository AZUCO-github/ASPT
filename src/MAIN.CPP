/*
	AZUCO Storage Pool Tool
*/
#include	<windows.h>
#include	<stdio.h>
#include	<locale.h>
#include	"physical_disk.h"

extern	unsigned long	physical_disk_number_max;
extern	unsigned long	HDD_SECTOR_SIZE;

extern	void			physical_disk_sector_dump(unsigned long,ULONG64);
extern	unsigned long	physical_disk_set_sector_size(unsigned long);
extern	void			physical_disk_number_max_get(void);
extern	void			physical_disk_list(void);
extern	void			physical_disk_mbr_disp(unsigned long);
extern	void			physical_disk_gpt_disp(unsigned long);
extern	void			physical_disk_gpt_partition_disp(unsigned long);

extern	void			storagepool_disp_spacedb(unsigned long,unsigned char);
extern	void			storagepool_disp_sdbc(unsigned long,unsigned char);
extern	void			storagepool_disp_sdbb(unsigned long,unsigned char);
extern	void			storagepool_check_sdbb(void);

extern	void			storagepool_export_sdbfa(unsigned long);
extern	void			storagepool_export_spacedb(unsigned long);
extern	void			storagepool_inport_spacedb(unsigned long);
extern	void			storagepool_export_sdbc(unsigned long);
extern	void			storagepool_inport_sdbc(unsigned long);
extern	void			storagepool_clear_sdbc(unsigned long);

extern	unsigned long	undisplay_sdbb_chain_04;
extern	unsigned long	undisplay_sdbb_chain_03_name_length_zero;


int	main(int argc,char *argv[])
{
	unsigned long	fnargc=0;
	unsigned long	tul1,tul2,tul3,tul4;
	ULONG64			tuq1;

	setlocale(LC_ALL, "jpn");

	//何もない場合
	if(argc==1){
		printf("aspt /<option>,<data>,...\n");
		printf("\t/a\t\t\t\t\t\tphysical disk list\n");
		printf("\t/b<physical drive number>\t\t\tview MBR\n");
		printf("\t/c<physical drive number>\t\t\tview GPT\n");
		printf("\t/d<physical drive number>\t\t\tview GPT partition table\n");
		printf("\t/e<physical drive number>\t\t\tview Storage Pool SPACEDB with structure validation\n");
		printf("\t/E<physical drive number>\t\t\t\tsecondary\n");
		printf("\t/f<physical drive number>\t\t\tview Storage Pool SDBC\n");
		printf("\t/F<physical drive number>\t\t\t\tsecondary\n");
		printf("\t/g<physical drive number>\t\t\tview Storage Pool SDBB\n");
		printf("\t/G<physical drive number>\t\t\t\tsecondary\n");
		printf("\n");

		printf("\t/h\t\t\t\t\t\tcheck Storage Pool SDBB\n");
		printf("\n");

		printf("\t/q<physical drive number>\t\t\texport SPACEDB\n");
		printf("\t/r<physical drive number>\t\t\tinport SPACEDB (need sc config spaceport start=disabled)\n");
		printf("\t/s<physical drive number>\t\t\texport sdbc\n");
		printf("\t/t<physical drive number>\t\t\tinport sdbc    (need sc config spaceport start=disabled)\n");
		printf("\t/u<physical drive number>\t\t\tclear  sdbc    (need sc config spaceport start=disabled)\n");
		printf("\t/v\t\t\t\t\t\tundisplay SDBB 03 noname\n");
		printf("\t/w\t\t\t\t\t\tundisplay SDBB 04 command\n");
		printf("\t/x<physical drive number>,<sector:HEX unit>\tsector dump\n");
		printf("\t/y<physical drive number>\t\t\tauto select HDD sector size\n");
		printf("\t/z<sector size switch>\t\t\t\tforce select HDD sector size. 4=4096/other=512\n");
		exit(-1);
	}

	//option処理
	for(tul1=1;tul1<argc;tul1++){
		if(argv[tul1][0]=='/'){
			switch(argv[tul1][1]){
				default:
					printf("invalid command\n");
					exit(-1);
					break;
				case	'a':
					physical_disk_list();
					break;
				case	'b':
					if(argv[tul1][2]=='*'){	//ワイルドカード指定
						physical_disk_number_max_get();
						for(tul2=0;tul2<physical_disk_number_max;tul2++){
							physical_disk_mbr_disp(tul2);
						}
					}else{
						sscanf(&argv[tul1][2],"%d",&tul2);
						physical_disk_mbr_disp(tul2);
					}
					break;
				case	'c':
					if(argv[tul1][2]=='*'){	//ワイルドカード指定
						physical_disk_number_max_get();
						for(tul2=0;tul2<physical_disk_number_max;tul2++){
							physical_disk_gpt_disp(tul2);
						}
					}else{
						sscanf(&argv[tul1][2],"%d",&tul2);
						physical_disk_gpt_disp(tul2);
					}
					break;
				case	'd':
					if(argv[tul1][2]=='*'){	//ワイルドカード指定
						physical_disk_number_max_get();
						for(tul2=0;tul2<physical_disk_number_max;tul2++){
							physical_disk_gpt_partition_disp(tul2);
						}
					}else{
						sscanf(&argv[tul1][2],"%d",&tul2);
						physical_disk_gpt_partition_disp(tul2);
					}
					break;



				case	'e':
					if(argv[tul1][2]=='*'){	//ワイルドカード指定
						physical_disk_number_max_get();
						for(tul2=0;tul2<physical_disk_number_max;tul2++){
							storagepool_disp_spacedb(tul2,0);
						}
					}else{
						sscanf(&argv[tul1][2],"%d",&tul2);
						storagepool_disp_spacedb(tul2,0);
					}
					break;
				case	'E':	//secondary
					if(argv[tul1][2]=='*'){	//ワイルドカード指定
						physical_disk_number_max_get();
						for(tul2=0;tul2<physical_disk_number_max;tul2++){
							storagepool_disp_spacedb(tul2,1);
						}
					}else{
						sscanf(&argv[tul1][2],"%d",&tul2);
						storagepool_disp_spacedb(tul2,1);
					}
					break;
				case	'f':
					if(argv[tul1][2]=='*'){	//ワイルドカード指定
						physical_disk_number_max_get();
						for(tul2=0;tul2<physical_disk_number_max;tul2++){
							storagepool_disp_sdbc(tul2,0);
						}
					}else{
						sscanf(&argv[tul1][2],"%d",&tul2);
						storagepool_disp_sdbc(tul2,0);
					}
					break;
				case	'F':	//secondary
					if(argv[tul1][2]=='*'){	//ワイルドカード指定
						physical_disk_number_max_get();
						for(tul2=0;tul2<physical_disk_number_max;tul2++){
							storagepool_disp_sdbc(tul2,1);
						}
					}else{
						sscanf(&argv[tul1][2],"%d",&tul2);
						storagepool_disp_sdbc(tul2,1);
					}
					break;
				case	'g':
					if(argv[tul1][2]=='*'){	//ワイルドカード指定
						physical_disk_number_max_get();
						for(tul2=0;tul2<physical_disk_number_max;tul2++){
							storagepool_disp_sdbb(tul2,0);
						}
					}else{
						sscanf(&argv[tul1][2],"%d",&tul2);
						storagepool_disp_sdbb(tul2,0);
					}
					break;
				case	'G':	//secondary
					if(argv[tul1][2]=='*'){	//ワイルドカード指定
						physical_disk_number_max_get();
						for(tul2=0;tul2<physical_disk_number_max;tul2++){
							storagepool_disp_sdbb(tul2,1);
						}
					}else{
						sscanf(&argv[tul1][2],"%d",&tul2);
						storagepool_disp_sdbb(tul2,1);
					}
					break;
				case	'h':
					storagepool_check_sdbb();
					break;







				case	'p':
					sscanf(&argv[tul1][2], "%d", &tul2);
					storagepool_export_sdbfa(tul2);
					break;
				case	'q':
					sscanf(&argv[tul1][2], "%d", &tul2);
					storagepool_export_spacedb(tul2);
					break;
				case	'r':
					sscanf(&argv[tul1][2], "%d", &tul2);
					storagepool_inport_spacedb(tul2);
					break;
				case	's':
					sscanf(&argv[tul1][2], "%d", &tul2);
					storagepool_export_sdbc(tul2);
					break;
				case	't':
					sscanf(&argv[tul1][2], "%d", &tul2);
					storagepool_inport_sdbc(tul2);
					break;
				case	'u':
					sscanf(&argv[tul1][2], "%d", &tul2);
					storagepool_clear_sdbc(tul2);
					break;


				case	'v':
					undisplay_sdbb_chain_03_name_length_zero=1;
					break;
				case	'w':
					undisplay_sdbb_chain_04=1;
					break;
				case	'x':
					sscanf(&argv[tul1][2],"%d,%I64x,%x",&tul2,&tuq1,&tul3);
					if(tul3==0)	tul3=1;
					for(tul4=0;tul4<tul3;tul4++)
						physical_disk_sector_dump(tul2,tuq1+tul4);
					break;
				case	'y':	//自動セット
					sscanf(&argv[tul1][2],"%d",&tul2);
					physical_disk_set_sector_size(tul2);
					break;
				case	'z':	//手動セット
					if(argv[tul1][2]=='4'){
						HDD_SECTOR_SIZE=HDD_SECTOR_SIZE_4096;
						printf("Set HDD SECTOR SIZE=4096\n");
					}else{
						HDD_SECTOR_SIZE=HDD_SECTOR_SIZE_0512;
						printf("Set HDD SECTOR SIZE=512\n");
					}
					break;
			}
		}else{
			if(fnargc==0)	fnargc=tul1;
		}
	}
	exit(-1);
}

