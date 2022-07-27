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

TEST(vulkanSeries, j_shader_modules)
{
	EXPECT_EQ(Series::j_shader_modules::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, k_fixed_functions)
{
	EXPECT_EQ(Series::k_fixed_functions::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, l_render_passes)
{
	EXPECT_EQ(Series::l_render_passes::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, m_conclusion)
{
	EXPECT_EQ(Series::m_conclusion::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, n_create_frame_buffers)
{
	EXPECT_EQ(Series::n_create_frame_buffers::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, o_command_buffers)
{
	EXPECT_EQ(Series::o_command_buffers::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, p_rendering_presentation)
{
	EXPECT_EQ(Series::p_rendering_presentation::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, q_swap_chain_recreation)
{
	EXPECT_EQ(Series::q_swap_chain_recreation::main(), EXIT_SUCCESS);
}

TEST(vulkanSeries, r_descriptor_layout_buffer)
{
	EXPECT_EQ(Series::r_descriptor_layout_buffer::main(), EXIT_SUCCESS);
}