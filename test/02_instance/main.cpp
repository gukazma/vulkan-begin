#include <gtest/gtest.h>

#include <Series/index.h>
#include <stdlib.h>
TEST(vulkanSeries, c_instance)
{
	EXPECT_EQ(Series::c_instance::main(), EXIT_SUCCESS);
}