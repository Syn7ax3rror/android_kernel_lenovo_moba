/*
 * intel_soc_pmic.h - Intel SoC PMIC Driver
 *
 * Copyright (C) 2012-2014 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Author: Yang, Bin <bin.yang@intel.com>
 * Author: Zhu, Lejun <lejun.zhu@linux.intel.com>
 */

#ifndef __INTEL_SOC_PMIC_H__
#define __INTEL_SOC_PMIC_H__

#include <linux/regmap.h>

struct intel_soc_pmic {
	int irq;
	struct regmap *regmap;
	struct regmap_irq_chip_data *irq_chip_data;
<<<<<<< HEAD
=======
	struct regmap_irq_chip_data *irq_chip_data_pwrbtn;
>>>>>>> abf4fbc657532dbe8f302d9ce2d78dbd2a009b82
	struct regmap_irq_chip_data *irq_chip_data_tmu;
	struct regmap_irq_chip_data *irq_chip_data_bcu;
	struct regmap_irq_chip_data *irq_chip_data_adc;
	struct regmap_irq_chip_data *irq_chip_data_chgr;
	struct regmap_irq_chip_data *irq_chip_data_crit;
	struct device *dev;
};

#endif	/* __INTEL_SOC_PMIC_H__ */
