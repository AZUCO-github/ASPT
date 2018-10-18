#ifndef aspt_physical_disk
#define aspt_physical_disk

#define	HDD_SECTOR_SIZE_0512	512
#define	HDD_SECTOR_SIZE_4096	4096

#pragma	pack(push,1)
struct partition_table
{
	unsigned char	boot_flag;
	unsigned char	start_sector_chs_1;
	unsigned char	start_sector_chs_2;
	unsigned char	start_sector_chs_3;
	unsigned char	partition_type;
	unsigned char	end_sector_chs_1;
	unsigned char	end_sector_chs_2;
	unsigned char	end_sector_chs_3;
	unsigned long	start_sector_lba;
	unsigned long	sector_size_lba;
};

struct mbr
{
	unsigned char	bootstrap_loader[446];
	struct partition_table	partition_table_1;
	struct partition_table	partition_table_2;
	struct partition_table	partition_table_3;
	struct partition_table	partition_table_4;
	unsigned short	boot_signature;
	unsigned char	pading_4k_sector[0xe00];
};

struct gpt
{
	unsigned char		signature[8];
	unsigned long		version;
	unsigned long		header_size;
	unsigned long		header_crc32;
	unsigned char		reserved1[4];
	unsigned long long	gpt_header_1st_lba;
	unsigned long long	gpt_header_2nd_lba;
	unsigned long long	useable_area_start_lba;
	unsigned long long	useable_area_end_lba;
	unsigned char		disk_guid[16];
	unsigned long long	partition_entry_lba;
	unsigned long		partition_entry_count;
	unsigned long		partition_entry_size;
	unsigned long		partition_entry_crc32;
	unsigned char		reserved2[420];
	unsigned char		pading_4k_sector[0xe00];
};

struct gpt_partition_entry_unit
{
	unsigned char		partition_type_guid[16];
	unsigned char		partition_guid[16];
	unsigned long long	start_lba;
	unsigned long long	end_lba;
	unsigned char		attribute[8];
	wchar_t				name[72>>1];
};

struct gpt_partition_entry_sector
{
	struct gpt_partition_entry_unit	entry[32];
};

#pragma	pack(pop)

#endif // !aspt_physical_disk
