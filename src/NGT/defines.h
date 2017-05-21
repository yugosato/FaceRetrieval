//
// Copyright (C) 2015-2017 Yahoo Japan Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

// Begin of cmake defines
/* #undef NGT_SHARED_MEMORY_ALLOCATOR */
/* #undef NGT_GRAPH_CHECK_VECTOR */
/* #undef NGT_AVX_DISABLED */
// End of cmake defines

//////////////////////////////////////////////////////////////////////////
// Release Definitions

#define		NGT_CREATION_EDGE_SIZE			10
#define		NGT_EXPLORATION_COEFFICIENT		1.1
#define		NGT_INSERTION_EXPLORATION_COEFFICIENT	1.1
#define		NGT_FORCED_REMOVE
#define		NGT_SHARED_MEMORY_MAX_SIZE		1024	// MB

#define		NGT_GRAPH_CHECK_VECTOR		// use vector to check whether nodes were accessed.
#define		NGT_GRAPH_CHECK_BOOLEANSET	// use original booleanset to check whether nodes were accessed.

