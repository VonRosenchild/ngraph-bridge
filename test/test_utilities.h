/*******************************************************************************
 * Copyright 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/
#ifndef NGRAPH_TF_BRIDGE_TESTUTILITIES_H_
#define NGRAPH_TF_BRIDGE_TESTUTILITIES_H_

#include "gtest/gtest.h"

#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor_types.h"
#include "tensorflow/core/platform/env.h"

#include "ngraph/ngraph.hpp"

using namespace std;
namespace ng = ngraph;

namespace tensorflow {

namespace ngraph_bridge {

namespace testing {

// Activate and Deactivate NGraph
void ActivateNGraph();
void DeactivateNGraph();

// Store/Restore Env Variables
const unordered_map<string, string> StoreEnv();
void RestoreEnv(const unordered_map<string, string>& map);

// EnvVariable Utilities
bool IsEnvVariableSet(const string& env_var_name);
string GetEnvVariable(const string& env_var_name);
void UnsetEnvVariable(const string& env_var_name);
void SetEnvVariable(const string& env_var_name, const string& env_var_val);

// NGRAPH_TF_BACKEND related
bool IsNGraphTFBackendSet();
const string GetNGraphTFBackend();
void UnsetNGraphTFBackend();
void SetNGraphTFBackend(const string& bname);

// Print Functions
void PrintTensor(const Tensor& T1);
void PrintTensorAllValues(
    const Tensor& T1,
    int64 max_entries);  // print max_entries of elements in the Tensor

// Generating Random Seed
const unsigned int GetSeedForRandomFunctions();

// Assignment Functions
// TODO : Retire AssignInputValuesAnchor and AssignInputValuesRandom
void AssignInputValuesAnchor(Tensor& A, float x);  // value assigned = x * index
void AssignInputValuesRandom(Tensor& A);

// Assigns value x to all the elements of the tensor
template <typename T>
void AssignInputValues(Tensor& A, T x) {
  auto A_flat = A.flat<T>();
  auto A_flat_data = A_flat.data();
  for (int i = 0; i < A_flat.size(); i++) {
    A_flat_data[i] = x;
  }
}

// Assigns values from the vector x to the Tensor
template <typename T>
void AssignInputValues(Tensor& A, vector<T> x) {
  auto A_flat = A.flat<T>();
  auto A_flat_data = A_flat.data();
  assert(A_flat.size() == x.size());
  for (int i = 0; i < A_flat.size(); i++) {
    A_flat_data[i] = x[i];
  }
}

// Assigns random values in range [min, max] to the Tensor
// Randomly generate data with specified type to populate the Tensor
// Random data is generated within range (min, max)
template <typename T>
void AssignInputValuesRandom(Tensor& A, T min, T max) {
  auto A_flat = A.flat<T>();
  auto A_flat_data = A_flat.data();
  srand(GetSeedForRandomFunctions());
  for (int i = 0; i < A_flat.size(); i++) {
    T value =
        // randomly generate a number between 0 and (max-min) inclusive
        static_cast<T>(rand()) / static_cast<T>(RAND_MAX / (max - min + 1));
    value = value + min;  // transform the range to (min, max) inclusive
    A_flat_data[i] = value;
  }
}

// Compares two Tensor vectors
void Compare(const vector<Tensor>& v1, const vector<Tensor>& v2,
             float rtol = static_cast<float>(1e-05),
             float atol = static_cast<float>(1e-08));

// TODO: Compares two Tensor vectors considering tolerance
void Compare(const vector<Tensor>& v1, const vector<Tensor>& v2,
             float tolerance);

// Compares two individual values in corresponding tensors
template <typename T>
bool Compare(T arg0, T arg1, T rtol, T atol) {
  return arg0 == arg1;
}

template <>
bool Compare(float arg0, float arg1, float rtol, float atol);

// Compares two Tensors
// Right now only tensors contain float values will modify the tolerance
// parameters
template <typename T>
void Compare(const Tensor& T1, const Tensor& T2,
             float rtol = static_cast<float>(1e-05),
             float atol = static_cast<float>(1e-08)) {
  // Assert rank
  ASSERT_EQ(T1.dims(), T2.dims())
      << "Ranks unequal for T1 and T2. T1.shape = " << T1.shape()
      << " T2.shape = " << T2.shape();

  // Assert each dimension
  for (int i = 0; i < T1.dims(); i++) {
    ASSERT_EQ(T1.dim_size(i), T2.dim_size(i))
        << "T1 and T2 shapes do not match in dimension " << i
        << ". T1.shape = " << T1.shape() << " T2.shape = " << T2.shape();
  }

  // Assert type
  ASSERT_EQ(T1.dtype(), T2.dtype()) << "Types of T1 and T2 did not match";
  auto T_size = T1.flat<T>().size();
  auto T1_data = T1.flat<T>().data();
  auto T2_data = T2.flat<T>().data();
  for (int k = 0; k < T_size; k++) {
    auto a = T1_data[k];
    auto b = T2_data[k];
    bool rt = Compare<T>(a, b, rtol, atol);
    EXPECT_TRUE(rt) << " TF output " << a << endl << " NG output " << b;
  }
}

// Compares Tensors considering tolerance
void Compare(Tensor& T1, Tensor& T2, float tol);

}  // namespace testing

}  // namespace ngraph_bridge

}  // namespace tensorflow

#endif  // NGRAPH_TF_BRIDGE_TESTUTILITIES_H_
