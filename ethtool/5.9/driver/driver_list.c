
static const struct {
	const char *name;
	int (*func)(struct ethtool_drvinfo *info, struct ethtool_regs *regs);

} driver_list[] = {
#ifdef ETHTOOL_ENABLE_PRETTY_DUMP
	{ "8139cp", realtek_dump_regs },
	{ "8139too", realtek_dump_regs },
	{ "r8169", realtek_dump_regs },
	{ "de2104x", de2104x_dump_regs },
	{ "e1000", e1000_dump_regs },
	{ "e1000e", e1000_dump_regs },
	{ "igb", igb_dump_regs },
	{ "ixgb", ixgb_dump_regs },
	{ "ixgbe", ixgbe_dump_regs },
	{ "ixgbevf", ixgbevf_dump_regs },
	{ "natsemi", natsemi_dump_regs },
	{ "e100", e100_dump_regs },
	{ "amd8111e", amd8111e_dump_regs },
	{ "pcnet32", pcnet32_dump_regs },
	{ "fec_8xx", fec_8xx_dump_regs },
	{ "ibm_emac", ibm_emac_dump_regs },
	{ "tg3", tg3_dump_regs },
	{ "skge", skge_dump_regs },
	{ "sky2", sky2_dump_regs },
	{ "vioc", vioc_dump_regs },
	{ "smsc911x", smsc911x_dump_regs },
	{ "at76c50x-usb", at76c50x_usb_dump_regs },
	{ "sfc", sfc_dump_regs },
	{ "st_mac100", st_mac100_dump_regs },
	{ "st_gmac", st_gmac_dump_regs },
	{ "et131x", et131x_dump_regs },
	{ "altera_tse", altera_tse_dump_regs },
	{ "vmxnet3", vmxnet3_dump_regs },
	{ "fjes", fjes_dump_regs },
	{ "lan78xx", lan78xx_dump_regs },
	{ "dsa", dsa_dump_regs },
	{ "fec", fec_dump_regs },
	{ "igc", igc_dump_regs },
	{ "bnxt_en", bnxt_dump_regs },
#endif
};
