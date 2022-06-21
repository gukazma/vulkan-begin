#include <gtest/gtest.h>
#include <Series/index.h>


TEST(vulkanSeries, a_dev_envir)
{
	Series::a_div_envir::main();
}

TEST(vulkanSeries, b_base_code)
{
	EXPECT_EQ(Series::b_base_code::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, c_instance)
{
	EXPECT_EQ(Series::c_instance::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, d_validation_layers)
{
	EXPECT_EQ(Series::d_validation_layers::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, e_physical_devices_and_queue_families)
{
	EXPECT_EQ(Series::e_physical_devices_and_queue_families::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, f_logical_device_and_queues)
{
	EXPECT_EQ(Series::f_logical_device_and_queues::main(), EXIT_SUCCESS);
}
