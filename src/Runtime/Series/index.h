#pragma once

namespace Series
{
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
	namespace a_div_envir
	{
		int main();
	}

	namespace b_base_code
	{
		int main();
	}

	namespace c_instance
	{
		int main();
	}
	namespace d_validation_layers
	{
		int main();
	}
	namespace e_physical_devices_and_queue_families
	{
		int main();
	}
	namespace f_logical_device_and_queues
	{
		int main();
	}
	namespace g_window_surface
	{
		int main();
	}
	namespace h_swap_chain
	{
		int main();
	}
	namespace i_image_views
	{
		int main();
	}
	namespace j_shader_modules
	{
		int main();
	}
	namespace k_fixed_functions
	{
		int main();
	}
	namespace l_render_passes
	{
		int main();
	}
	namespace m_conclusion
	{
		int main();
	}
	namespace n_create_frame_buffers
	{
		int main();
	}
	namespace o_command_buffers
	{
		int main();
	}
	namespace p_rendering_presentation
	{
		int main();
	}
	namespace q_swap_chain_recreation
	{
		int main();
	}
	namespace r_descriptor_layout_buffer
	{
		int main();
	}
	namespace s_staging_buffer
	{
		int main();
	}
}