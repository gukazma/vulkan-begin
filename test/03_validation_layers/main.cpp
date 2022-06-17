#include <gtest/gtest.h>

#include <Series/index.h>
#include <stdlib.h>
TEST(vulkanSeries, d_validation_layers)
{
	EXPECT_EQ(Series::d_validation_layers::main(), EXIT_SUCCESS);
}