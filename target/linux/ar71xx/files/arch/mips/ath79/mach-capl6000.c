/*
 *  Hanbit Electronics CAPL-6000 board support
 *
 *  Copyright (C) 2020 Yoonji Park <koreapyj@dcmys.kr>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define CAPL_6000_CALDATA_OFFSET	0x1100

static void __init capl6000_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1fff110c);

	ath79_register_m25p80(NULL);

	ath79_register_mdio(0, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 1);

	/* WAN port */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth1_data.phy_mask = BIT(4);
	ath79_register_eth(1);

	/* LAN ports */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth0_data.speed = SPEED_1000;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy4_mii_en = 1;
	ath79_register_eth(0);

	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_CAPL_6000, "CAPL-6000", "Hanbit Electronics CAPL-6000",
	     capl6000_setup);
