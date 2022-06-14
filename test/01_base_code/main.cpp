#include <gtest/gtest.h>

#include <Series/index.h>
#include <stdlib.h>
TEST(vulkanSeries, b_base_code)
{
	EXPECT_EQ(Series::b_base_code::main(), EXIT_SUCCESS);
}