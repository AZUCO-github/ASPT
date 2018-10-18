clp++;	TL1=0;	TL2=0;

switch(sscanf(&clb[clp]," %X %X",&TL1,&TL2)){
	case	1:	mdc[ccn]=TL1;	TL2=1;	break;	//パラメータなし
	case	2:	mdc[ccn]=TL1;			break;	//全部ある
	default:					TL2=1;	break;	}

for(;TL2!=0;TL2--){
	switch(ccn){
		case	CPU_SH2_MASTER:	pas->sh2m.mem_dump(mdc[ccn]);	break;
		case	CPU_SH2_SLAVE:	pas->sh2s.mem_dump(mdc[ccn]);	break;
		case	CPU_MC68K:		pas->m68k.mem_dump(mdc[ccn]);	break;
		case	CPU_SCUDSP:		pas->sdsp.mem_dump(mdc[ccn]);	break;
	}
	mdc[ccn]+=0x100;
}
goto	COM_EXIT;
