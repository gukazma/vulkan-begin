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
}