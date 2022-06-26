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

TEST(vulkanSeries, g_window_surface)
{
	EXPECT_EQ(Series::g_window_surface::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, h_swap_chain)
{
	EXPECT_EQ(Series::h_swap_chain::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, i_image_views)
{
	EXPECT_EQ(Series::i_image_views::main(), EXIT_SUCCESS);
}
