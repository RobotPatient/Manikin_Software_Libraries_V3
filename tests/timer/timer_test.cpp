#include "catch2/catch_all.hpp"
#include <catch2/catch_session.hpp>
#include "sample_timer/sample_timer.h"
#include "fake_timer_functions.h"

TEST_CASE("Manikin_timer_init", "![init_param]")
{
}

int
main (int argc, char *argv[])
{
    // your setup ...

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}