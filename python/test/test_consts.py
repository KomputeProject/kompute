import kp
import numpy as np
import pytest

from .utils import compile_source

workgroup = (3, 1, 1)

def test_pushconsts_int32_spec_const_int32():
    shader = """
        #version 450
        layout(constant_id = 0) const int spec_val = 42;
        layout(push_constant) uniform PushConsts {
            int value[3];
        } pc;
        layout(set = 0, binding = 0) buffer Output {
            float outData[];
        };
        void main() {
            uint idx = gl_GlobalInvocationID.x;
            outData[idx] = float(pc.value[idx]) + float(spec_val);
        }
    """
    spirv = compile_source(shader)
    mgr = kp.Manager()
    arr_out = np.array([0.0, 0.0, 0.0], dtype=np.float32)
    tensor_out = mgr.tensor_t(arr_out)
    push_consts = np.array([1, 2, 3], dtype=np.int32)
    spec_const = np.array([5], dtype=np.int32)  # Will override spec_val to 5
    algo = mgr.algorithm([tensor_out], spirv, workgroup, spec_const, push_consts)
    (mgr.sequence()
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpSyncLocal([tensor_out]))
        .eval())
    assert np.array_equal(tensor_out.data(), push_consts.astype(np.float32) + spec_const.astype(np.float32)[0])

def test_pushconsts_int32_spec_const_uint32():
    shader = """
        #version 450
        layout(constant_id = 0) const uint spec_val = 0u;
        layout(push_constant) uniform PushConsts {
            int value[3];
        } pc;
        layout(set = 0, binding = 0) buffer Output {
            float outData[];
        };
        void main() {
            uint idx = gl_GlobalInvocationID.x;
            outData[idx] = float(pc.value[idx]) + float(spec_val);
        }
    """
    spirv = compile_source(shader)
    mgr = kp.Manager()
    arr_out = np.array([0.0, 0.0, 0.0], dtype=np.float32)
    tensor_out = mgr.tensor_t(arr_out)
    push_consts = np.array([4, 5, 6], dtype=np.int32)
    spec_const = np.array([7], dtype=np.uint32)
    algo = mgr.algorithm([tensor_out], spirv, workgroup, spec_const, push_consts)
    (mgr.sequence()
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpSyncLocal([tensor_out]))
        .eval())
    assert np.array_equal(tensor_out.data(), push_consts.astype(np.float32) + spec_const.astype(np.float32)[0])

def test_pushconsts_int32_spec_const_float():
    shader = """
        #version 450
        layout(constant_id = 0) const float spec_val = 0.0;
        layout(push_constant) uniform PushConsts {
            int value[3];
        } pc;
        layout(set = 0, binding = 0) buffer Output {
            float outData[];
        };
        void main() {
            uint idx = gl_GlobalInvocationID.x;
            outData[idx] = float(pc.value[idx]) + float(spec_val);
        }
    """
    spirv = compile_source(shader)
    mgr = kp.Manager()
    arr_out = np.array([0.0, 0.0, 0.0], dtype=np.float32)
    tensor_out = mgr.tensor_t(arr_out)
    push_consts = np.array([7, 8, 9], dtype=np.int32)
    spec_const = np.array([3.3], dtype=np.float32)
    algo = mgr.algorithm([tensor_out], spirv, workgroup, spec_const, push_consts)
    (mgr.sequence()
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpSyncLocal([tensor_out]))
        .eval())
    assert np.array_equal(tensor_out.data(), push_consts.astype(np.float32) + spec_const.astype(np.float32)[0])

def test_pushconsts_uint32_spec_const_int32():
    shader = """
        #version 450
        layout(constant_id = 0) const int spec_val = 0;
        layout(push_constant) uniform PushConsts {
            uint value[3];
        } pc;
        layout(set = 0, binding = 0) buffer Output {
            float outData[];
        };
        void main() {
            uint idx = gl_GlobalInvocationID.x;
            outData[idx] = float(pc.value[idx]) + float(spec_val);
        }
    """
    spirv = compile_source(shader)
    mgr = kp.Manager()
    arr_out = np.array([0.0, 0.0, 0.0], dtype=np.float32)
    tensor_out = mgr.tensor_t(arr_out)
    push_consts = np.array([1, 2, 3], dtype=np.uint32)
    spec_const = np.array([4], dtype=np.int32)
    algo = mgr.algorithm([tensor_out], spirv, workgroup, spec_const, push_consts)
    (mgr.sequence()
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpSyncLocal([tensor_out]))
        .eval())
    assert np.array_equal(tensor_out.data(), push_consts.astype(np.float32) + spec_const.astype(np.float32)[0])

def test_pushconsts_uint32_spec_const_uint32():
    shader = """
        #version 450
        layout(constant_id = 0) const uint spec_val = 0u;
        layout(push_constant) uniform PushConsts {
            uint value[3];
        } pc;
        layout(set = 0, binding = 0) buffer Output {
            float outData[];
        };
        void main() {
            uint idx = gl_GlobalInvocationID.x;
            outData[idx] = float(pc.value[idx]) + float(spec_val);
        }
    """
    spirv = compile_source(shader)
    mgr = kp.Manager()
    arr_out = np.array([0.0, 0.0, 0.0], dtype=np.float32)
    tensor_out = mgr.tensor_t(arr_out)
    push_consts = np.array([4, 5, 6], dtype=np.uint32)
    spec_const = np.array([8], dtype=np.uint32)
    algo = mgr.algorithm([tensor_out], spirv, workgroup, spec_const, push_consts)
    (mgr.sequence()
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpSyncLocal([tensor_out]))
        .eval())
    assert np.array_equal(tensor_out.data(), push_consts.astype(np.float32) + spec_const.astype(np.float32)[0])

def test_pushconsts_uint32_spec_const_float():
    shader = """
        #version 450
        layout(constant_id = 0) const float spec_val = 0.0;
        layout(push_constant) uniform PushConsts {
            uint value[3];
        } pc;
        layout(set = 0, binding = 0) buffer Output {
            float outData[];
        };
        void main() {
            uint idx = gl_GlobalInvocationID.x;
            outData[idx] = float(pc.value[idx]) + float(spec_val);
        }
    """
    spirv = compile_source(shader)
    mgr = kp.Manager()
    arr_out = np.array([0.0, 0.0, 0.0], dtype=np.float32)
    tensor_out = mgr.tensor_t(arr_out)
    push_consts = np.array([7, 8, 9], dtype=np.uint32)
    spec_const = np.array([3.3], dtype=np.float32)
    algo = mgr.algorithm([tensor_out], spirv, workgroup, spec_const, push_consts)
    (mgr.sequence()
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpSyncLocal([tensor_out]))
        .eval())
    assert np.array_equal(tensor_out.data(), push_consts.astype(np.float32) + spec_const.astype(np.float32)[0])

def test_pushconsts_float_spec_const_int32():
    shader = """
        #version 450
        layout(constant_id = 0) const int spec_val = 42;
        layout(push_constant) uniform PushConsts {
            float value[3];
        } pc;
        layout(set = 0, binding = 0) buffer Output {
            float outData[];
        };
        void main() {
            uint idx = gl_GlobalInvocationID.x;
            outData[idx] = float(pc.value[idx]) + float(spec_val);
        }
    """
    spirv = compile_source(shader)
    mgr = kp.Manager()
    arr_out = np.array([0.0, 0.0, 0.0], dtype=np.float32)
    tensor_out = mgr.tensor_t(arr_out)
    push_consts = np.array([1.1, 2.2, 3.3], dtype=np.float32)
    spec_const = np.array([11], dtype=np.int32)
    algo = mgr.algorithm([tensor_out], spirv, workgroup, spec_const, push_consts)
    (mgr.sequence()
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpSyncLocal([tensor_out]))
        .eval())
    assert np.array_equal(tensor_out.data(), push_consts.astype(np.float32) + spec_const.astype(np.float32)[0])

def test_pushconsts_float_spec_const_uint32():
    shader = """
        #version 450
        layout(constant_id = 0) const uint spec_val = 0u;
        layout(push_constant) uniform PushConsts {
            float value[3];
        } pc;
        layout(set = 0, binding = 0) buffer Output {
            float outData[];
        };
        void main() {
            uint idx = gl_GlobalInvocationID.x;
            outData[idx] = float(pc.value[idx]) + float(spec_val);
        }
    """
    spirv = compile_source(shader)
    mgr = kp.Manager()
    arr_out = np.array([0.0, 0.0, 0.0], dtype=np.float32)
    tensor_out = mgr.tensor_t(arr_out)
    push_consts = np.array([4.4, 5.5, 6.6], dtype=np.float32)
    spec_const = np.array([2], dtype=np.uint32)
    algo = mgr.algorithm([tensor_out], spirv, workgroup, spec_const, push_consts)
    (mgr.sequence()
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpSyncLocal([tensor_out]))
        .eval())
    assert np.array_equal(tensor_out.data(), push_consts.astype(np.float32) + spec_const.astype(np.float32)[0])

def test_pushconsts_float_spec_const_float():
    shader = """
        #version 450
        layout(constant_id = 0) const float spec_val = 0.0;
        layout(push_constant) uniform PushConsts {
            float value[3];
        } pc;
        layout(set = 0, binding = 0) buffer Output {
            float outData[];
        };
        void main() {
            uint idx = gl_GlobalInvocationID.x;
            outData[idx] = float(pc.value[idx]) + float(spec_val);
        }
    """
    spirv = compile_source(shader)
    mgr = kp.Manager()
    arr_out = np.array([0.0, 0.0, 0.0], dtype=np.float32)
    tensor_out = mgr.tensor_t(arr_out)
    push_consts = np.array([7.7, 8.8, 9.9], dtype=np.float32)
    spec_const = np.array([1.1], dtype=np.float32)
    algo = mgr.algorithm([tensor_out], spirv, workgroup, spec_const, push_consts)
    (mgr.sequence()
        .record(kp.OpAlgoDispatch(algo))
        .record(kp.OpSyncLocal([tensor_out]))
        .eval())
    assert np.array_equal(tensor_out.data(), push_consts.astype(np.float32) + spec_const.astype(np.float32)[0])
