// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#define ARROW_VERSION_MAJOR 6
#define ARROW_VERSION_MINOR 0
#define ARROW_VERSION_PATCH 1
#define ARROW_VERSION ((ARROW_VERSION_MAJOR * 1000) + ARROW_VERSION_MINOR) * 1000 + ARROW_VERSION_PATCH

#define ARROW_VERSION_STRING "6.0.1"

#define ARROW_SO_VERSION "600"
#define ARROW_FULL_SO_VERSION "600.1.0"

#define ARROW_CXX_COMPILER_ID "Clang"
#define ARROW_CXX_COMPILER_VERSION "13.0.1"
#define ARROW_CXX_COMPILER_FLAGS " --gcc-toolchain=/home/lzq/home/gkk/usr/local/ClickHouse/cmake/linux/../../contrib/sysroot/linux-x86_64 -std=c++20 -fdiagnostics-color=always -fsized-deallocation  -pipe -mssse3 -msse4.1 -msse4.2 -mpclmul -mpopcnt -fasynchronous-unwind-tables -ffile-prefix-map=/home/lzq/home/gkk/usr/local/ClickHouse=. -falign-functions=32 -mbranches-within-32B-boundaries -fdiagnostics-absolute-paths -fstrict-vtable-pointers -fexperimental-new-pass-manager -w"

#define ARROW_GIT_ID ""
#define ARROW_GIT_DESCRIPTION ""

#define ARROW_PACKAGE_KIND ""

/* #undef ARROW_COMPUTE */
/* #undef ARROW_CSV */
/* #undef ARROW_DATASET */
/* #undef ARROW_FILESYSTEM */
/* #undef ARROW_FLIGHT */
/* #undef ARROW_IPC */
/* #undef ARROW_JSON */

/* #undef ARROW_S3 */
/* #undef ARROW_USE_NATIVE_INT128 */

/* #undef GRPCPP_PP_INCLUDE */
