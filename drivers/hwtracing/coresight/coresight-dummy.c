// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/coresight.h>
#include <linux/of.h>

#define DUMMY_TRACE_ID_START	256

struct dummy_drvdata {
	struct device			*dev;
	struct coresight_device		*csdev;
	int				traceid;
};

static int dummy_source_enable(struct coresight_device *csdev,
			       struct perf_event *event, u32 mode)
{
	struct dummy_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);

	dev_info(drvdata->dev, "Dummy source enabled\n");

	return 0;
}

static void dummy_source_disable(struct coresight_device *csdev,
				 struct perf_event *event)
{
	struct dummy_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);

	dev_info(drvdata->dev, "Dummy source disabled\n");
}

<<<<<<< HEAD
static int dummy_sink_enable(struct coresight_device *csdev, u32 mode)
=======
static int dummy_sink_enable(struct coresight_device *csdev, u32 mode,
			     void *data)
>>>>>>> abf4fbc657532dbe8f302d9ce2d78dbd2a009b82
{
	struct dummy_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);

	dev_info(drvdata->dev, "Dummy sink enabled\n");

	return 0;
}

<<<<<<< HEAD
static void dummy_sink_disable(struct coresight_device *csdev)
=======
static int dummy_sink_disable(struct coresight_device *csdev)
>>>>>>> abf4fbc657532dbe8f302d9ce2d78dbd2a009b82
{
	struct dummy_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);

	dev_info(drvdata->dev, "Dummy sink disabled\n");
<<<<<<< HEAD
=======

	return 0;
>>>>>>> abf4fbc657532dbe8f302d9ce2d78dbd2a009b82
}

static int dummy_trace_id(struct coresight_device *csdev)
{
	struct dummy_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);

	return drvdata->traceid;
}

static const struct coresight_ops_source dummy_source_ops = {
	.trace_id	= dummy_trace_id,
	.enable		= dummy_source_enable,
	.disable	= dummy_source_disable,
};

static const struct coresight_ops_sink dummy_sink_ops = {
	.enable		= dummy_sink_enable,
	.disable	= dummy_sink_disable,
};

static const struct coresight_ops dummy_cs_ops = {
	.source_ops	= &dummy_source_ops,
	.sink_ops	= &dummy_sink_ops,
};

static int dummy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct coresight_platform_data *pdata;
	struct dummy_drvdata *drvdata;
	struct coresight_desc *desc;
	static int traceid = DUMMY_TRACE_ID_START;

	pdata = of_get_coresight_platform_data(dev, pdev->dev.of_node);
	if (IS_ERR(pdata))
		return PTR_ERR(pdata);
	pdev->dev.platform_data = pdata;

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	drvdata->dev = &pdev->dev;
	platform_set_drvdata(pdev, drvdata);

	desc = devm_kzalloc(dev, sizeof(*desc), GFP_KERNEL);
	if (!desc)
		return -ENOMEM;

	drvdata->traceid = traceid++;

	if (of_property_read_bool(pdev->dev.of_node, "qcom,dummy-source")) {
		desc->type = CORESIGHT_DEV_TYPE_SOURCE;
		desc->subtype.source_subtype =
					CORESIGHT_DEV_SUBTYPE_SOURCE_PROC;
	} else if (of_property_read_bool(pdev->dev.of_node,
					 "qcom,dummy-sink")) {
		desc->type = CORESIGHT_DEV_TYPE_SINK;
		desc->subtype.sink_subtype = CORESIGHT_DEV_SUBTYPE_SINK_BUFFER;
	} else {
		dev_info(dev, "Device type not set.\n");
		return -EINVAL;
	}

	desc->ops = &dummy_cs_ops;
	desc->pdata = pdev->dev.platform_data;
	desc->dev = &pdev->dev;
	drvdata->csdev = coresight_register(desc);
	if (IS_ERR(drvdata->csdev))
		return PTR_ERR(drvdata->csdev);

	dev_info(dev, "Dummy device initialized\n");

	return 0;
}

static int dummy_remove(struct platform_device *pdev)
{
	struct dummy_drvdata *drvdata = platform_get_drvdata(pdev);

	coresight_unregister(drvdata->csdev);
	return 0;
}

static const struct of_device_id dummy_match[] = {
	{.compatible = "qcom,coresight-dummy"},
	{}
};

static struct platform_driver dummy_driver = {
	.probe	= dummy_probe,
	.remove	= dummy_remove,
	.driver	= {
		.name   = "coresight-dummy",
		.of_match_table = dummy_match,
	},
};

int __init dummy_init(void)
{
	return platform_driver_register(&dummy_driver);
}
module_init(dummy_init);

void __exit dummy_exit(void)
{
	platform_driver_unregister(&dummy_driver);
}
module_exit(dummy_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("CoreSight dummy source driver");
