void	sector_dump(unsigned long addr)
{
	unsigned long	x,y;
	char			asc1[0x100];
	char			asc2[0x100];

	printf("    +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F 0123456789ABCDEF\n");

	for(y=0;y!=32;y++){
		printf("%03X ",y*16);

		printf("%02X ",*(addr+y*16+ 0));
		printf("%02X ",*(addr+y*16+ 1));
		printf("%02X ",*(addr+y*16+ 2));
		printf("%02X,",*(addr+y*16+ 3));
		printf("%02X ",*(addr+y*16+ 4));
		printf("%02X ",*(addr+y*16+ 5));
		printf("%02X ",*(addr+y*16+ 6));
		printf("%02X,",*(addr+y*16+ 7));
		printf("%02X ",*(addr+y*16+ 8));
		printf("%02X ",*(addr+y*16+ 9));
		printf("%02X ",*(addr+y*16+10));
		printf("%02X,",*(addr+y*16+11));
		printf("%02X ",*(addr+y*16+12));
		printf("%02X ",*(addr+y*16+13));
		printf("%02X ",*(addr+y*16+14));
		printf("%02X ",*(addr+y*16+15));

		memcpy(asc1,addr+y*16,17);
		asc1[17]=NULL;
		wsprintf(asc2,"%s",asc1);
		printf("%s\n",asc2);
	}
}



void	sector_dump_40(unsigned long addr)
{
	unsigned long	x,y;
	char			asc1[0x100];
	char			asc2[0x100];

	printf("    +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F 0123456789ABCDEF\n");

	for(y=0;y!=32;y+=4){
		printf("%03X ",y*16);

		printf("%02X ",*(addr+y*16+ 0));
		printf("%02X ",*(addr+y*16+ 1));
		printf("%02X ",*(addr+y*16+ 2));
		printf("%02X,",*(addr+y*16+ 3));
		printf("%02X ",*(addr+y*16+ 4));
		printf("%02X ",*(addr+y*16+ 5));
		printf("%02X ",*(addr+y*16+ 6));
		printf("%02X,",*(addr+y*16+ 7));
		printf("%02X ",*(addr+y*16+ 8));
		printf("%02X ",*(addr+y*16+ 9));
		printf("%02X ",*(addr+y*16+10));
		printf("%02X,",*(addr+y*16+11));
		printf("%02X ",*(addr+y*16+12));
		printf("%02X ",*(addr+y*16+13));
		printf("%02X ",*(addr+y*16+14));
		printf("%02X,",*(addr+y*16+15));

		printf("%02X ",*(addr+y*16+16));
		printf("%02X ",*(addr+y*16+17));
		printf("%02X ",*(addr+y*16+18));
		printf("%02X,",*(addr+y*16+19));
		printf("%02X ",*(addr+y*16+20));
		printf("%02X ",*(addr+y*16+21));
		printf("%02X ",*(addr+y*16+22));
		printf("%02X,",*(addr+y*16+23));
		printf("%02X ",*(addr+y*16+24));
		printf("%02X ",*(addr+y*16+25));
		printf("%02X ",*(addr+y*16+26));
		printf("%02X,",*(addr+y*16+27));
		printf("%02X ",*(addr+y*16+28));
		printf("%02X ",*(addr+y*16+29));
		printf("%02X ",*(addr+y*16+30));
		printf("%02X,",*(addr+y*16+31));

		printf("%02X ",*(addr+y*16+32));
		printf("%02X ",*(addr+y*16+33));
		printf("%02X ",*(addr+y*16+34));
		printf("%02X,",*(addr+y*16+35));
		printf("%02X ",*(addr+y*16+36));
		printf("%02X ",*(addr+y*16+37));
		printf("%02X ",*(addr+y*16+38));
		printf("%02X,",*(addr+y*16+39));
		printf("%02X ",*(addr+y*16+40));
		printf("%02X ",*(addr+y*16+41));
		printf("%02X ",*(addr+y*16+42));
		printf("%02X,",*(addr+y*16+43));
		printf("%02X ",*(addr+y*16+44));
		printf("%02X ",*(addr+y*16+45));
		printf("%02X ",*(addr+y*16+46));
		printf("%02X,",*(addr+y*16+47));

		printf("%02X ",*(addr+y*16+48));
		printf("%02X ",*(addr+y*16+49));
		printf("%02X ",*(addr+y*16+50));
		printf("%02X,",*(addr+y*16+51));
		printf("%02X ",*(addr+y*16+52));
		printf("%02X ",*(addr+y*16+53));
		printf("%02X ",*(addr+y*16+54));
		printf("%02X,",*(addr+y*16+55));
		printf("%02X ",*(addr+y*16+56));
		printf("%02X ",*(addr+y*16+57));
		printf("%02X ",*(addr+y*16+58));
		printf("%02X,",*(addr+y*16+59));
		printf("%02X ",*(addr+y*16+60));
		printf("%02X ",*(addr+y*16+61));
		printf("%02X ",*(addr+y*16+62));
		printf("%02X,",*(addr+y*16+63));

		memcpy(asc1,addr+y*16,17);
		asc1[17]=NULL;
		wsprintf(asc2,"%s",asc1);
		printf("%s\n",asc2);
	}
}



