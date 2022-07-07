// Copyright 2022 ISP RAS
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//###############################################################################

#include <sstream>
#include <string>
#include <unistd.h>

#include <ATen/core/jit_type.h>
#include <c10/core/ScalarType.h>
#include <torch/csrc/jit/backends/backend.h>
#include <torch/csrc/jit/backends/backend_detail.h>
#include <torch/csrc/jit/backends/backend_preprocess.h>
#include <torch/csrc/jit/mobile/nnc/aot_compiler.h>
#include <torch/csrc/jit/passes/freeze_module.h>
#include <torch/csrc/jit/serialization/export.h>
#include <torch/csrc/jit/serialization/import.h>
#include <torch/csrc/jit/tensorexpr/graph_opt.h>
#include <torch/csrc/jit/tensorexpr/kernel.h>
#include <torch/script.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    char name[] = "/tmp/torch-fuzz-XXXXXX";
    char *dir = mktemp(name);

    std::ofstream fp;
    fp.open(dir, std::ios::out | std::ios::binary);
    fp.write((char *) data, size);
    fp.close();

    try {
        auto m = torch::jit::load(dir);
    } catch (const c10::Error &e) {
        unlink(dir);
        return 0;
    } catch (const torch::jit::ErrorReport &e) {
        unlink(dir);
        return 0;
    } catch(const std::runtime_error &e) {
        unlink(dir);
        return 0;
    }

    unlink(dir);

    return 0;
}
