#! /bin/sh

cmake -DBOX2D_BUILD_EXAMPLES=false -DBOX2D_BUILD_UNITTESTS=false -DNP_ENGINE_BUILD_TESTER=true -S .. -B ../out/build