
#include <stdio.h>
#include <stdint.h>

#define TEST_HEADER 1


#define EXIT_FAILURE -1
#define EXIT_SUCCESS 0
#define ARRAY_SIZE(a)  (sizeof(a)/sizeof(a[0]))
#define HEADER_SIZE 52


struct str_pmecc {
	int use_pmecc;
	int sector_per_page;
	int spare_size;
	int ecc_bits;
	int sector_size;
	int ecc_offset;
};


int pmecc_add_header(uint32_t * header , struct str_pmecc * pmecc)
{
	uint32_t tmp;
	size_t pos;

	tmp = 0xC << 28;

	tmp |= (pmecc->ecc_offset & 0x1ff) << 18;

	switch (pmecc->sector_size) {
	case 512:
		tmp |= 0 << 16;
		break;
	case 1024:
		tmp |= 1 << 16;
		break;

	default:
		printf("Wrong sectorSize (%i) for PMECC header\n",
		       pmecc->sector_size);
		return EXIT_FAILURE;
	}

	switch (pmecc->ecc_bits) {
	case 2:
		tmp |= 0 << 13;
		break;
	case 4:
		tmp |= 1 << 13;
		break;
	case 8:
		tmp |= 2 << 13;
		break;
	case 12:
		tmp |= 3 << 13;
		break;
	case 24:
		tmp |= 4 << 13;
		break;

	default:
		printf("Wrong eccBits (%i) for PMECC header\n",
		       pmecc->ecc_bits);
		 return EXIT_FAILURE;
	}

	tmp |= (pmecc->spare_size & 0x1ff) << 4;

	switch (pmecc->sector_per_page) {
	case 1:
		tmp |= 0 << 1;
		break;
	case 2:
		tmp |= 1 << 1;
		break;
	case 4:
		tmp |= 2 << 1;
		break;
	case 8:
		tmp |= 3 << 1;
		break;

	default:
		printf("Wrong sectorPerPage (%i) for PMECC header\n",
		       pmecc->sector_per_page);
		return EXIT_FAILURE;
	}

	if (pmecc->use_pmecc)
		tmp |= 1;

	for (pos = 0; pos < HEADER_SIZE; pos++)
		header[pos] = tmp;

	printf("PMECC header filled 52 times with 0x%08X\n", tmp);

	//tparams->header_size = sizeof(nand_pmecc_header);
	//tparams->hdr = nand_pmecc_header;

	return EXIT_SUCCESS;
}

void pmecc_print_header(const uint32_t word)
{
	int val;

	printf("PMECC header:\n");

	printf("\t\t====================\n");

	val = (word >> 18) & 0x1ff;
	printf("\t\teccOffset: %9i\n", val);

	val = (((word >> 16) & 0x3) == 0) ? 512 : 1024;
	printf("\t\tsectorSize: %8i\n", val);

	if (((word >> 13) & 0x7) <= 2)
		val = (2 << ((word >> 13) & 0x7));
	else
		val = (12 << (((word >> 13) & 0x7) - 3));
	printf("\t\teccBitReq: %9i\n", val);

	val = (word >> 4) & 0x1ff;
	printf("\t\tspareSize: %9i\n", val);

	val = (1 << ((word >> 1) & 0x3));
	printf("\t\tnbSectorPerPage: %3i\n", val);

	printf("\t\tusePmecc: %10i\n", word & 0x1);
	printf("\t\t====================\n");
}

#if TEST_HEADER

FILE *fp;
FILE *ifp;
uint32_t nand_pmecc_header[52];


int main(int argc, char ** argp)
{
struct str_pmecc pmecc=
{
        .use_pmecc=1,
        .sector_per_page=4,
        .spare_size=64,
        .ecc_bits=4,
        .sector_size=512,
        .ecc_offset=36

};
uint32_t header[52];
char * filename;
char * ifilename;
char buffer[1024];
int size=0;
int n=0;
if((argc!=8)&&(argc!=3))
{
printf("Use:%s <eccOffset:36> <sectorSize:512> <eccBitReq:4> <spareSize:64> <nbSectorPerPage:4> <infilename> <outfilename>\n" \
	"or Use:%s <infilename> <outfilename>  for default parameters\n",argp[0],argp[0]);
return -1;
}

if(argc==3)
{
filename=argp[2];
ifilename=argp[1];
}
else if(argc==8)
{
	filename=argp[7];
	ifilename=argp[6];
        pmecc.use_pmecc=1;
        pmecc.sector_per_page=strtol(argp[5],0,0);
        pmecc.spare_size=strtol(argp[4],0,0);
        pmecc.ecc_bits=strtol(argp[3],0,0);
        pmecc.sector_size=strtol(argp[2],0,0);
        pmecc.ecc_offset=strtol(argp[1],0,0);

}
int ret = pmecc_add_header(header , &pmecc);
if(!ret)
{
	pmecc_print_header(header[0]);
	fp = fopen(filename, "w");
	if(!fp)
	{
		printf("Error open file: %s for writing\n",filename);
		return -1;
	}
	ifp = fopen(ifilename, "r");
	if(!ifp)
        {
                printf("Error open file: %s\n",ifilename);
                return -1;
        }

	fwrite(header, sizeof(uint32_t),ARRAY_SIZE(header),fp);
	do
	{
		size=fread(buffer,1,1024, ifp);
		if(n==0)
		{
			printf("Correct datalen to:%d",buffer[21]*256+buffer[20]);
			buffer[22]=0;
			buffer[23]=0;
		}
		fwrite(buffer, 1,size,fp);
		n+=size;
	}while(size);
}
exit:
fclose(fp);
fclose(ifp);


return ret;
}

#endif

