/*
 *  Copyright (C) 2020 Yoonji Park <koreapyj@dcmys.kr>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/byteorder/generic.h>

#include "mtdsplit.h"

#define HANBIT_NR_PARTS		2

struct hanbit_fw_header {
    uint32_t    magic;              /* magic 0x13972174 */
    uint32_t    kernel_ofs;         /* kernel offset */
    uint32_t    rootfs_ofs;         /* rootfs offset */
    uint8_t     version_major;
    uint8_t     version_minor;
    uint8_t     version_build;
    uint8_t     version_revision;

    uint32_t    created_at;         /* build time */
    uint32_t    kernel_len;         /* kernel length */
    uint32_t    usrfs_ofs;          /* usrfs offset */
    uint32_t    fw_length;          /* total length of firmware */

    uint32_t    fw_crc32;           /* file crc32 except header */
    char        unk1[4];
    char        unk2[8];

    char        fw_name[64];        /* original file name / ex) capl-6000_v1.8.18.bin */
    char        hw_name[48];        /* hardware name / ex) CAPL-6000 */
    char        unk3[8];
    char        unk4[32];
    char        unk5[56];
} __attribute__ ((packed));

static int mtdsplit_parse_hanbit(struct mtd_info *master,
				 const struct mtd_partition **pparts,
				 struct mtd_part_parser_data *data)
{
	struct hanbit_fw_header hdr;
	size_t hdr_len, retlen, kernel_size;
	size_t rootfs_offset;
	struct mtd_partition *parts;
	int err;

	hdr_len = sizeof(hdr);
	err = mtd_read(master, 0, hdr_len, &retlen, (void *) &hdr);
	if (err)
		return err;

	if (retlen != hdr_len)
		return -EIO;

    if (be32_to_cpu(hdr.kernel_ofs) != sizeof(hdr))
        return -EINVAL;

    kernel_size = sizeof(hdr) + be32_to_cpu(hdr.kernel_len);
    rootfs_offset = be32_to_cpu(hdr.rootfs_ofs);

	if (kernel_size > master->size)
		return -EINVAL;

	/* Find the rootfs */
	err = mtd_check_rootfs_magic(master, rootfs_offset, NULL);
	if (err)
		return err;

	parts = kzalloc(HANBIT_NR_PARTS * sizeof(*parts), GFP_KERNEL);
	if (!parts)
		return -ENOMEM;

	parts[0].name = KERNEL_PART_NAME;
	parts[0].offset = 0;
	parts[0].size = kernel_size;

	parts[1].name = ROOTFS_PART_NAME;
	parts[1].offset = rootfs_offset;
	parts[1].size = master->size - rootfs_offset;

	*pparts = parts;
	return HANBIT_NR_PARTS;
}

static const struct of_device_id mtdsplit_hanbit_of_match_table[] = {
	{ .compatible = "hanbit,firmware" },
	{},
};

static struct mtd_part_parser mtdsplit_hanbit_parser = {
	.owner = THIS_MODULE,
	.name = "hanbit-fw",
	.of_match_table = mtdsplit_hanbit_of_match_table,
	.parse_fn = mtdsplit_parse_hanbit,
	.type = MTD_PARSER_TYPE_FIRMWARE,
};

static int __init mtdsplit_hanbit_init(void)
{
	register_mtd_parser(&mtdsplit_hanbit_parser);

	return 0;
}

subsys_initcall(mtdsplit_hanbit_init);
