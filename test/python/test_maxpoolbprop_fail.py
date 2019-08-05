# ==============================================================================
#  Copyright 2018-2019 Intel Corporation
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ==============================================================================
"""nGraph TensorFlow bridge MaxPoolBackprop operation test

"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import pytest
import numpy as np

import tensorflow as tf
from tensorflow.python.ops.gen_nn_ops import max_pool_grad

from common import NgraphTest

NHWC_TO_NCHW = (0, 3, 1, 2)
NCHW_TO_NHWC = (0, 2, 3, 1)


class TestMaxPoolBackpropInput(NgraphTest):

    @pytest.mark.parametrize("padding", ("VALID", "SAME"))
    def test_nhwc(self, padding):
        input_nhwc = np.random.rand(128, 224, 224, 3)
        ksize_nhwc = [1, 2, 2, 1]
        strides_nhwc = [1, 1, 1, 1]
        if padding == "VALID":
            output_nhwc = np.random.rand(128, 223, 223, 3)
        elif padding == "SAME":
            output_nhwc = np.random.rand(128, 224, 224, 3)

        grad_nhwc = np.random.rand(128, 224, 224, 3)

        a = max_pool_grad(
            input_nhwc,
            output_nhwc,
            grad_nhwc,
            ksize_nhwc,
            strides_nhwc,
            padding=padding,
            data_format="NHWC")

        sess_fn = lambda sess: sess.run(a)

        expected = self.without_ngraph(sess_fn)
        result = self.with_ngraph(sess_fn)
        print('padding ', padding)
        print('output shape ', np.shape(result))
        assert np.allclose(expected, result)
