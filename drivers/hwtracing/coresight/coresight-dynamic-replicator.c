// SPDX-License-Identifier: GPL-2.0
/*
<<<<<<< HEAD
 * Copyright (c) 2011-2015, The Linux Foundation. All rights reserved.
=======
 * Copyright (c) 2011-2015, 2020 The Linux Foundation. All rights reserved.
>>>>>>> abf4fbc657532dbe8f302d9ce2d78dbd2a009b82
 */

#include <linux/amba/bus.h>
#include <linux/clk.h>
#include <linux/coresight.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>

#include "coresight-priv.h"

#define REPLICATOR_IDFILTER0		0x000
#define REPLICATOR_IDFILTER1		0x004

/**
 * struct replicator_state - specifics associated to a replicator component
 * @base:	memory mapped base address for this component.
 * @dev:	the device entity associated with this component
 * @atclk:	optional clock for the core parts of the replicator.
 * @csdev:	component vitals needed by the framework
<<<<<<< HEAD
=======
 * @spinlock:	serialize enable/disable operations.
>>>>>>> abf4fbc657532dbe8f302d9ce2d78dbd2a009b82
 */
struct replicator_state {
	void __iomem		*base;
	struct device		*dev;
	struct clk		*atclk;
	struct coresight_device	*csdev;
<<<<<<< HEAD
};

static int replicator_enable(struct coresight_device *csdev, int inport,
			      int outport)
{
	struct replicator_state *drvdata = dev_get_drvdata(csdev->dev.parent);

	CS_UNLOCK(drvdata->base);

	/*
	 * Ensure that the other port is disabled
	 * 0x00 - passing through the replicator unimpeded
	 * 0xff - disable (or impede) the flow of ATB data
	 */
	if (outport == 0) {
		writel_relaxed(0x00, drvdata->base + REPLICATOR_IDFILTER0);
		writel_relaxed(0xff, drvdata->base + REPLICATOR_IDFILTER1);
	} else {
		writel_relaxed(0x00, drvdata->base + REPLICATOR_IDFILTER1);
		writel_relaxed(0xff, drvdata->base + REPLICATOR_IDFILTER0);
	}

	CS_LOCK(drvdata->base);

	dev_info(drvdata->dev, "REPLICATOR enabled\n");
	return 0;
=======
	spinlock_t		spinlock;
};

/*
 * replicator_reset : Reset the replicator configuration to sane values.
 */
static void replicator_reset(struct replicator_state *drvdata)
{
	CS_UNLOCK(drvdata->base);

	if (!coresight_claim_device_unlocked(drvdata->base)) {
		writel_relaxed(0xff, drvdata->base + REPLICATOR_IDFILTER0);
		writel_relaxed(0xff, drvdata->base + REPLICATOR_IDFILTER1);
		coresight_disclaim_device_unlocked(drvdata->base);
	}

	CS_LOCK(drvdata->base);
}

static int dynamic_replicator_enable(struct replicator_state *drvdata,
				     int inport, int outport)
{
	int rc = 0;
	u32 reg;

	switch (outport) {
	case 0:
		reg = REPLICATOR_IDFILTER0;
		break;
	case 1:
		reg = REPLICATOR_IDFILTER1;
		break;
	default:
		WARN_ON(1);
		return -EINVAL;
	}

	CS_UNLOCK(drvdata->base);

	if ((readl_relaxed(drvdata->base + REPLICATOR_IDFILTER0) == 0xff) &&
	    (readl_relaxed(drvdata->base + REPLICATOR_IDFILTER1) == 0xff))
		rc = coresight_claim_device_unlocked(drvdata->base);

	/* Ensure that the outport is enabled. */
	if (!rc)
		writel_relaxed(0x00, drvdata->base + reg);
	CS_LOCK(drvdata->base);

	return rc;
}

static bool is_replicator_disabled(struct coresight_device *csdev)
{
	int i;

	for (i = 0; i < csdev->nr_outport; i++) {
		if (atomic_read(&csdev->refcnt[i]) > 0)
			return false;
	}

	return true;
}

static int replicator_enable(struct coresight_device *csdev, int inport,
			     int outport)
{
	int rc = 0;
	struct replicator_state *drvdata = dev_get_drvdata(csdev->dev.parent);
	unsigned long flags;
	bool first_enable = false;

	spin_lock_irqsave(&drvdata->spinlock, flags);

	if (is_replicator_disabled(csdev))
		replicator_reset(drvdata);

	if (atomic_read(&csdev->refcnt[outport]) == 0) {
		rc = dynamic_replicator_enable(drvdata, inport, outport);
		if (!rc)
			first_enable = true;
	}
	if (!rc)
		atomic_inc(&csdev->refcnt[outport]);
	spin_unlock_irqrestore(&drvdata->spinlock, flags);

	if (first_enable)
		dev_dbg(&csdev->dev, "REPLICATOR enabled\n");
	return rc;
}

static void dynamic_replicator_disable(struct replicator_state *drvdata,
				       int inport, int outport)
{
	u32 reg;

	switch (outport) {
	case 0:
		reg = REPLICATOR_IDFILTER0;
		break;
	case 1:
		reg = REPLICATOR_IDFILTER1;
		break;
	default:
		WARN_ON(1);
		return;
	}

	CS_UNLOCK(drvdata->base);

	/* disable the flow of ATB data through port */
	writel_relaxed(0xff, drvdata->base + reg);

	if ((readl_relaxed(drvdata->base + REPLICATOR_IDFILTER0) == 0xff) &&
	    (readl_relaxed(drvdata->base + REPLICATOR_IDFILTER1) == 0xff))
		coresight_disclaim_device_unlocked(drvdata->base);
	CS_LOCK(drvdata->base);
>>>>>>> abf4fbc657532dbe8f302d9ce2d78dbd2a009b82
}

static void replicator_disable(struct coresight_device *csdev, int inport,
				int outport)
{
	struct replicator_state *drvdata = dev_get_drvdata(csdev->dev.parent);
<<<<<<< HEAD

	CS_UNLOCK(drvdata->base);

	/* disable the flow of ATB data through port */
	if (outport == 0)
		writel_relaxed(0xff, drvdata->base + REPLICATOR_IDFILTER0);
	else
		writel_relaxed(0xff, drvdata->base + REPLICATOR_IDFILTER1);

	CS_LOCK(drvdata->base);

	dev_info(drvdata->dev, "REPLICATOR disabled\n");
=======
	unsigned long flags;
	bool last_disable = false;

	spin_lock_irqsave(&drvdata->spinlock, flags);
	if (atomic_dec_return(&csdev->refcnt[outport]) == 0) {
		dynamic_replicator_disable(drvdata, inport, outport);
		last_disable = true;
	}
	spin_unlock_irqrestore(&drvdata->spinlock, flags);

	if (last_disable)
		dev_dbg(drvdata->dev, "REPLICATOR disabled\n");
>>>>>>> abf4fbc657532dbe8f302d9ce2d78dbd2a009b82
}

static const struct coresight_ops_link replicator_link_ops = {
	.enable		= replicator_enable,
	.disable	= replicator_disable,
};

static const struct coresight_ops replicator_cs_ops = {
	.link_ops	= &replicator_link_ops,
};

#define coresight_replicator_reg(name, offset) \
	coresight_simple_reg32(struct replicator_state, name, offset)

coresight_replicator_reg(idfilter0, REPLICATOR_IDFILTER0);
coresight_replicator_reg(idfilter1, REPLICATOR_IDFILTER1);

static struct attribute *replicator_mgmt_attrs[] = {
	&dev_attr_idfilter0.attr,
	&dev_attr_idfilter1.attr,
	NULL,
};

static const struct attribute_group replicator_mgmt_group = {
	.attrs = replicator_mgmt_attrs,
	.name = "mgmt",
};

static const struct attribute_group *replicator_groups[] = {
	&replicator_mgmt_group,
	NULL,
};

static int replicator_probe(struct amba_device *adev, const struct amba_id *id)
{
	int ret;
	struct device *dev = &adev->dev;
	struct resource *res = &adev->res;
	struct coresight_platform_data *pdata = NULL;
	struct replicator_state *drvdata;
	struct coresight_desc desc = { 0 };
	struct device_node *np = adev->dev.of_node;
	void __iomem *base;

	if (np) {
		pdata = of_get_coresight_platform_data(dev, np);
		if (IS_ERR(pdata))
			return PTR_ERR(pdata);
		adev->dev.platform_data = pdata;
	}

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	drvdata->dev = &adev->dev;
	drvdata->atclk = devm_clk_get(&adev->dev, "atclk"); /* optional */
	if (!IS_ERR(drvdata->atclk)) {
		ret = clk_prepare_enable(drvdata->atclk);
		if (ret)
			return ret;
	}

	/* Validity for the resource is already checked by the AMBA core */
	base = devm_ioremap_resource(dev, res);
	if (IS_ERR(base))
		return PTR_ERR(base);

	drvdata->base = base;
	dev_set_drvdata(dev, drvdata);
	pm_runtime_put(&adev->dev);

<<<<<<< HEAD
=======
	spin_lock_init(&drvdata->spinlock);
>>>>>>> abf4fbc657532dbe8f302d9ce2d78dbd2a009b82
	desc.type = CORESIGHT_DEV_TYPE_LINK;
	desc.subtype.link_subtype = CORESIGHT_DEV_SUBTYPE_LINK_SPLIT;
	desc.ops = &replicator_cs_ops;
	desc.pdata = adev->dev.platform_data;
	desc.dev = &adev->dev;
	desc.groups = replicator_groups;
	drvdata->csdev = coresight_register(&desc);

<<<<<<< HEAD
	return PTR_ERR_OR_ZERO(drvdata->csdev);
=======
	if (!IS_ERR(drvdata->csdev)) {
		replicator_reset(drvdata);
		return 0;
	}
	return PTR_ERR(drvdata->csdev);
>>>>>>> abf4fbc657532dbe8f302d9ce2d78dbd2a009b82
}

#ifdef CONFIG_PM
static int replicator_runtime_suspend(struct device *dev)
{
	struct replicator_state *drvdata = dev_get_drvdata(dev);

	if (drvdata && !IS_ERR(drvdata->atclk))
		clk_disable_unprepare(drvdata->atclk);

	return 0;
}

static int replicator_runtime_resume(struct device *dev)
{
	struct replicator_state *drvdata = dev_get_drvdata(dev);

	if (drvdata && !IS_ERR(drvdata->atclk))
		clk_prepare_enable(drvdata->atclk);

	return 0;
}
#endif

static const struct dev_pm_ops replicator_dev_pm_ops = {
	SET_RUNTIME_PM_OPS(replicator_runtime_suspend,
			   replicator_runtime_resume,
			   NULL)
};

static const struct amba_id replicator_ids[] = {
	{
		.id     = 0x000bb909,
		.mask   = 0x000fffff,
	},
	{
		/* Coresight SoC-600 */
		.id     = 0x000bb9ec,
		.mask   = 0x000fffff,
	},
	{ 0, 0 },
};

static struct amba_driver replicator_driver = {
	.drv = {
		.name	= "coresight-dynamic-replicator",
		.pm	= &replicator_dev_pm_ops,
		.suppress_bind_attrs = true,
	},
	.probe		= replicator_probe,
	.id_table	= replicator_ids,
};
builtin_amba_driver(replicator_driver);
