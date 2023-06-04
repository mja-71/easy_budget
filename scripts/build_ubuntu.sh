#!/bin/bash

set -Eeuox pipefail

CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null&& pwd )"
CMD_CMAKE="cmake"
CMD_CPACK="cpack"

echo "Build AccountTool"
cd $CUR_DIR
$CMD_CMAKE --version
[ ! -d $CUR_DIR/../build_account ] && mkdir -p $CUR_DIR/../build_account
cd $CUR_DIR/../build_account

$CMD_CMAKE -G"Unix Makefiles" $CUR_DIR/.. 
$CMD_CMAKE --build . -- -j 8

#Generate packaging
$CMD_CPACK --config "$CUR_DIR/../setup/cpack_config.cmake" -D CPACK_PROJECT_CONFIG_FILE="$CUR_DIR/../setup/cpack_generator.cmake" -G DEB .
cd $CUR_DIR

echo "Everything is good"





