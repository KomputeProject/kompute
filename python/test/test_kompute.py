import os

import kp
import numpy as np
import logging
import pyshader as ps

DIRNAME = os.path.dirname(os.path.abspath(__file__))

def test_opalgobase_file():
    """
    Test basic OpMult operation
    """

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr = kp.Manager()
    mgr.rebuild([tensor_in_a, tensor_in_b, tensor_out])

    shader_path = os.path.join(DIRNAME, "../../shaders/glsl/opmult.comp.spv")

    mgr.eval_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shader_path)

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]


params = [kp.Tensor([2, 2, 2]), kp.Tensor([1, 2, 3]), kp.Tensor([0, 0, 0])]

mgr = kp.Manager()
op_ct = kp.OpTensorCreate(params)
op_ct = mgr.rebuild(op_ct)
mgr.eval_op(op_ct)

algo = kp.Algo(params, spirv)
op_ac = kp.OpAlgoCreate(algo)
op_ac = mgr.rebuild(op_ac)
mgr.eval_op(op_ac)

op_ac = kp.OpAlgoCreate(kp.Algo(params, spirv))
mgr.eval_op(kp.OpAlgoCreate(algo))


mgr = kp.Manager()

op_ct = kp.OpTensorCreate(mgr, params) # This initialises operation
op_ct.eval()

algo = kp.Algo(params, spirv)
op_ac = kp.OpAlgoCreate(mgr, algo)
op_ct.eval()

op_tsd = kp.OpTensorSyncDevice(mgr, params)
op_ad = kp.OpAlgoDispatch(mgr, algo)
op_tsl = kp.OpTensorSyncLocal(mgr, params)

sq = kp.Sequence(mgr, "newSeq")
sq.record([op_tsd, op_ad, op_tsl])
sq.eval()
sq.destroy()

# Explore consistent interface:
op_tsd = kp.OpTensorSyncDevice(sq, params)
op_ad = kp.OpAlgoDispatch(sq, algo)
op_tsl = kp.OpTensorSyncLocal(sq, params)

op_tsd.record()
op_ad.record()
op_tsl.record()

sq.eval()



auto params = ...;
std::string shader = "...";
std::vector<uint32_t> spirv = kp::Shader::compile_source(shader);

// Example passing mgr
kp::Manager mgr;

kp::OpTensorCreate op_tc(mgr, params);
op_tc.eval()

kp::Algorithm algo(params, spirv);
kp::OpAlgoCreate op_ac(mgr, algo);
op_ac.eval()

op_ac.destroy()
op_tc.destroy()

kp::OpTensorAlgoCreate op_c(mgr, params, algo);
op_c.eval()

kp::Sequence sq(mgr);

kp::OpTensorSyncDevice op_tsd(mgr, params);
kp::OpAlgoDispatch op_ad(mgr, algo);
kp::OpTensorSyncLocal op_tsl(mgr, params);

sq.record({op_tsd, op_ad, op_tsl})

for(...) {
    sq.eval();

    tensorA...
}

######
#######
#######
#######
#######
######
// Example not passing mgr
kp::Manager mgr;

std::shared_ptr<kp::OpTensorCreate> op_tc_1{ new kp::OpTensorCreate(params) };
auto sq_1 = mgr.eval(op_tc_1); // Initialises and stores op as part of new sequence
mgr.eval(op_tc_1); // Fails as this op can only be "initialised" once
mgr.destroy(op_tc_1);
mgr.eval(op_tc_1); // This works as it's a new setup
mgr.eval<kp::OpTensorCreate>(params); // Fails as tensors already created
// NOT ALLOED TO DELETE JUST TENSORS ANYMORE - SEE BELOW
mgr.destroy(params); // Sends to inconsistent state as op_tc_1 will still destroy these parameters
mgr.destroy(op_tc_1, recursive=false); // Destroys only operation, which is useful when you need to ensure another operation owns the parameters
auto op_tc_2 = mgr.eval<kp::OpTensorCreate>(params);
std::shared_ptr<kp::OpTensorCreate> op_tc_2{ new kp::OpTensorCreate(params) }; // fails as tensors already created
op_tc_2.destroy(); // Manager still holds dangling reference so requires explicit termination in manager
mgr.destroy(op_tc_2);
auto op_tc_3 = mgr.eval({ new kp::OpTensorCreate(params) });

std::shared_ptr<kp::Algorithm> algo{ new kp::Algorithm(params, spirv, kp::Workgroup(), kp::SpecConst(), kp::PushConst()) };
std::shared_ptr<kp::OpAlgoCreate> op_ac_1{ new kp::OpAlgoCreate(algo) };
mgr.eval(op_ac_1); // Initialises and stores op as part of manager
mgr.eval(op_ac_1); // Fails as this op can only be "initialised" once
mgr.destroy(op_ac_1);

std::shared_ptr<kp::OpAlgoCreate> op_ac_2 =
    mgr.eval({ new kp::OpAlgoCreate(params, { new kp::Algorithm(spirv) }) });

std::shared_ptr<kp::OpAlgoMultCreate> op_amc{ new kp::OpAlgoMultCreate(params) };
mgr.eval(op_amc);

std::shared_ptr<kp::Algorithm> algo_mult = op_amc.algorithm()
std::vector<std::shared_ptr<kp::Tensor>> params = op_amc.tensors()

auto op_tsd = std::make_shared<kp::OpTensorSyncDevice>(params);
auto op_ad = std::make_shared<kp::OpAlgoSetPushConst>(algo);
auto op_ad = std::make_shared<kp::OpAlgoDispatch>(algo);
auto op_tsl = std::make_shared<kp::OpTensorSyncLocal>(mgr, params);

op_params = {op_tsd, op_ad, op_tsl};

mgr.record(op_params);
mgr.eval(); // Runs recorded default sequence

mgr.record(op_params, clear=false); // Non-create ops ok if rerun
mgr.eval(); // Runs twice the recorded paams

mgr.record("namedSeq", op_params);
mgr.eval("namedSeq");

kp::Manager mgrAsync(0, {0, 2});
mgr.sequence("namedSeq2", 0); // Create named sequence with queue in index 0
mgr.sequence("namedSeq3", 1);

mgr.eval_async("namedSeq2", op_params); // Clear, record params and eval
mgr.eval_async("namedSeq3", op_params); // Clear, record params and eval

mgr.eval_await("namedSeq2");
mgr.eval_await("namedSeq3");

mgr.destroy("namedSeq"); // Destroy named sequence
mgr.destroy({"namedSeq2", "namedSeq3"}); // Destroy multiple named sequences
mgr.destroy("namedSeq"); // Error




mgr = kp.Manager(0, [0, 2])

// Manager does not need to manage seq anymore
sq_1 = kp.Sequence(mgr, 0)

t1 = kp.Tensor(sq_1, [0, 0, 0])
t2 = kp.Tensor(sq_1, [0, 1, 2])

algo = kp.Algorithm(sq_1)

op_tc = kp.OpTensorCreate(sq_1, params)
op_tsd = kp.OpTensorSyncDevice(sq_1, params)
op_ac = kp.OpAlgoCreate(sq_1, algo)
op_ad = kp.OpAlgoDispatch(sq_1, algo)

sq_1.clear()

op_tc.record()
op_tsd.record()
op_ac.record()
op_ad.record()
op_ad.record()
op_ad.record()

sq_1.eval()


std::shared_ptr<kp::Manager> mgr = kp::ManagerSP(0, {0, 1});

std::shared_ptr<kp::Sequence> sq_2 = kp::SequenceSP(mgr, 1)

std::shared_ptr<kp::Tensor> t1 = kp::TensorSP(sq_2, {1, 2, 3});
std::shared_ptr<kp::Tensor> t2 = kp::TensorSP(sq_2, {2, 3, 4});

auto params = ...

std::shared_ptr<kp::Algorithm> algo2 = kp::AlgorithmSP(sq_2, params, spirv, workgroup);

// How do we deal with this?
{
    auto op_1 = kp::OpTensorSyncDevice(sq_2, params)
    auto op_2 = kp::OpAlgoDispatch(sq_2, algo)
}

sq_2.eval()


// HEAP ONLY - This would fail

kp::Manager mgr = kp::Manager(0, {0, 1});

kp::Sequence sq_2 = kp::Sequence(mgr, 1)

kp::Tensor t1 = kp::Tensor(sq_2, {1, 2, 3});
kp::Tensor t2 = kp::Tensor(sq_2, {2, 3, 4});

auto params = ...

kp::Algorithm algo2 = kp::AlgorithmSP(sq_2, params, spirv, workgroup);

// How do we deal with this?
{
    auto op_1 = kp::OpTensorSyncDevice(sq_2, params)
    auto op_2 = kp::OpAlgoDispatch(sq_2, algo)
}

sq_2.eval()





kp::Manager mgr = kp::Manager(0, {0, 1});

kp::Sequence sq_2 = kp::Sequence(mgr, 1)

kp::Tensor t1 = kp::Tensor(sq_2, {1, 2, 3});
kp::Tensor t2 = kp::Tensor(sq_2, {2, 3, 4});

auto params = ...

kp::Algorithm* algo2 = new kp::Algorithm(sq_2, params, spirv, workgroup);

// How do we deal with this?
{
    auto op_1 = kp::OpTensorSyncDevice(sq_2, params)
    auto op_2 = kp::OpAlgoDispatch(sq_2, algo)
}

sq_2.eval()






kp::Manager mgr = kp::Manager;

auto sq_2 = mgr.sequence()

{
    // What if we want to use tensor in a different sequence?
    auto t1 = sq_2.tensor({1, 2, 3});
    auto t2 = sq_2.tensor({1, 2, 3});

    auto algo2 = sq_2.algorithm();

    sq_2.record(kp::OpTensorRebuild({ t1 }))
    sq_2.record(kp::OpAlgoRebuild(params, algo2, spirv))
    sq_2.record(kp::OpTensorSyncDevice(prams))
    sq_2.record(kp::OpAlgoDispatch(prams, algo2))
}

sq_2.eval()



kp::Manager mgr = kp::Manager;

auto t1 = mgr.tensor({1, 2, 3}); // Held as weak ptr but passed as shared
auto t2 = mgr.tensor({1, 2, 3});

auto algo2 = mgr.algorithm();

{
    auto sq_2 = mgr.sequence()

    {
        sq_2.record(kp::OpTensorRebuild({ t1 })) // record only supports move operator &&
        sq_2.record(kp::OpAlgoRebuild(params, algo2, spirv))
        sq_2.record(kp::OpTensorSyncDevice(prams))
        sq_2.record(kp::OpAlgoDispatch(prams, algo2))
    }

    sq_2.eval()
}



// What about only tensors being init with it


{
    kp::Manager mgr = kp::Manager;

    auto t0 = mgr.tensor({0, 0, 0})

    {
        auto t1 = mgr.tensor({1, 2, 3}); // Held as weak ptr but passed as shared (refc 1)

        {
            auto sq_2 = mgr.sequence()

            {

                auto t2 = mgr.tensor({1, 2, 3}); // Held as weak ptr but passed as shared (refc 1)
                auto algo2 = mgr.algorithm(); // Held as weak ptr but passed as shared (refc 1)

                params = {t1, t2}

                sq_2.record(kp::OpTensorRebuild(params, {1, 2, 3, 4})) // Refc is now 2 for 3 for params
                sq_2.record(kp::OpAlgoRebuild(params, algo2, spirv)) // refc is now 2 for algo2, 3 for parms
                sq_2.record(kp::OpTensorSyncDevice(prams)) // refc for params 4
                sq_2.record(kp::OpAlgoDispatch(prams, algo2)) // refc for params 5, 3 for algo2
            }

            sq_2.eval() // all refcs stil valid
        } // seq destroyed so refc for algo2 and t2 drops to 0, gets destroyed, t1 has 1
    } // t1 refc drops to 0, gets destroyed
    // refc of t0 is still 1

    mgr.gc() // Iterates through all tensor, sequence and algo weak_ptr and removes unused

    // can we have something like
    mgr.sequence()
        .record(kp::OpTensorRebuild(params, {1, 2, 3, 4}))
        .record(kp::OpAlgoDispatch(params, algo2))
        .eval();

}// refc is destroyed by manager manually, the rest are empty shells so ignored




kp::Manager mgr = kp::Manager(0, {0, 1});

std::shared_ptr<kp::Tensor> t1 = mgr.tensor({1, 2, 3});
std::shared_ptr<kp::Tensor> t2 = mgr.tensor({1, 2, 3});

auto params = ...

std::shared_ptr<kp::Algorithm> algo2 = mgr.algorithm(params, spirv, workgroup);

sq_2.record<kp::OpTensorSyncDevice>(prams)
sq_2.record<kp::OpAlgoDispatch>(algo)


// WHY NO MORE DETROY TENSORS:

     * std::shared_ptr<kp::OpTensorCreate> op_tc1{ kp::OpTensorCreate(params) };
     * {
     *     std::shared_ptr<kp::OpTensorCreate> op_tc2{ kp::OpTensorCreate(params) };
     *     mgr.eval(op_tc2);
     *     mgr.destroy(params);
     *
     *     mgr.eval(op_tc1);
     *
     * } // op_tc1 is destroyed and all parameters are freed



// NO LONGER ALLOWED:  Mainly as manager now needs to regsiter ops
// If we still want it, then sequence wil have to hold ref to manager
auto sq = mgr.sequence();

auto op_tsd = std::make_shared<kp::OpTensorSyncDevice>(params);
auto op_ad = std::make_shared<kp::OpAlgoDispatch>(algo);
auto op_tsl = std::make_shared<kp::OpTensorSyncLocal>(mgr, params);

sq.record({op_tsd, op_ad, op_tsl}); // Clear and record
sq.eval();
sq.record({op_tsd, op_ad, op_tsl}, clear=false); // record on top
sq.eval();
sq.clear(); // explicitly clear





mgr = kp.Manager()

op_ct = kp.OpTensorCreate(params)
mgr.eval(op_ct)

algo = kp.Algo(params, spirv)
op_ac = kp.OpAlgoCreate(algo)
mgr.eval(op_ac) # Runs init on operator function (below shows explicit steps)

op_tsd = kp.OpTensorSyncDevice(params)
op_ad = kp.OpAlgoDispatch(algo)
op_tsl = kp.OpTensorSyncLocal(params)

sq = mgr.sequence()
sq.record([op_tsd, op_ad, op_tsl])
sq.eval()
sq.eval()
sq.eval()

mgr.eval(op_ac) # Would fail as algo is initialised
mgr.destroy(op_ac) # Destroys Op and Algo owned object
mgr.eval(op_ac) # Succeeds with new
mgr.destroy(op_ac)
mgr.init(op_ac)
mgr.eval(op_ac, init=False)











def test_shader_str():
    """
    Test basic OpAlgoBase operation
    """

    shader = """
#version 450
layout(set = 0, binding = 0) buffer tensorLhs {float valuesLhs[];};
layout(set = 0, binding = 1) buffer tensorRhs {float valuesRhs[];};
layout(set = 0, binding = 2) buffer tensorOutput { float valuesOutput[];};
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main()
{
    uint index = gl_GlobalInvocationID.x;
    valuesOutput[index] = valuesLhs[index] * valuesRhs[index];
}
    """

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr = kp.Manager()
    mgr.rebuild([tensor_in_a, tensor_in_b, tensor_out])

    spirv = kp.Shader.compile_source(shader)

    mgr.eval_algo_data_def([tensor_in_a, tensor_in_b, tensor_out], spirv)

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]

def test_sequence():
    """
    Test basic OpAlgoBase operation
    """
    mgr = kp.Manager(0, [2])

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr.rebuild([tensor_in_a, tensor_in_b, tensor_out])

    shader_path = os.path.abspath(os.path.join(DIRNAME, "../../shaders/glsl/opmult.comp.spv"))
    mgr.eval_async_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shader_path)

    mgr.eval_await_def()

    seq = mgr.sequence("op")
    seq.begin()
    seq.record_tensor_sync_local([tensor_in_a])
    seq.record_tensor_sync_local([tensor_in_b])
    seq.record_tensor_sync_local([tensor_out])
    seq.end()
    seq.eval()

    mgr.destroy("op")

    assert seq.is_init() == False

    assert tensor_out.data() == [2.0, 4.0, 6.0]
    assert np.all(tensor_out.numpy() == [2.0, 4.0, 6.0])

    mgr.destroy(tensor_in_a)
    mgr.destroy([tensor_in_b, tensor_out])

    assert tensor_in_a.is_init() == False
    assert tensor_in_b.is_init() == False
    assert tensor_out.is_init() == False

def test_workgroup():
    mgr = kp.Manager(0)

    tensor_a = kp.Tensor(np.zeros([16,8]))
    tensor_b = kp.Tensor(np.zeros([16,8]))

    mgr.rebuild([tensor_a, tensor_b])

    @ps.python2shader
    def compute_shader_wg(gl_idx=("input", "GlobalInvocationId", ps.ivec3),
                          gl_wg_id=("input", "WorkgroupId", ps.ivec3),
                          gl_wg_num=("input", "NumWorkgroups", ps.ivec3),
                          data1=("buffer", 0, ps.Array(ps.f32)),
                          data2=("buffer", 1, ps.Array(ps.f32))):
        i = gl_wg_id.x * gl_wg_num.y + gl_wg_id.y
        data1[i] = f32(gl_idx.x)
        data2[i] = f32(gl_idx.y)

    seq = mgr.sequence("new")
    seq.begin()
    seq.record_algo_data([tensor_a, tensor_b], compute_shader_wg.to_spirv(), workgroup=(16,8,1))
    seq.end()
    seq.eval()

    mgr.destroy(seq)

    assert seq.is_init() == False

    mgr.eval_tensor_sync_local_def([tensor_a, tensor_b])

    print(tensor_a.numpy())
    print(tensor_b.numpy())

    assert np.all(tensor_a.numpy() == np.stack([np.arange(16)]*8, axis=1).ravel())
    assert np.all(tensor_b.numpy() == np.stack([np.arange(8)]*16, axis=0).ravel())

    mgr.destroy([tensor_a, tensor_b])

    assert tensor_a.is_init() == False
    assert tensor_b.is_init() == False


def test_tensor_rebuild_backwards_compat():
    """
    Test basic OpMult operation
    """

    tensor_in_a = kp.Tensor([2, 2, 2])
    tensor_in_b = kp.Tensor([1, 2, 3])
    tensor_out = kp.Tensor([0, 0, 0])

    mgr = kp.Manager()

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    shader_path = os.path.abspath(os.path.join(DIRNAME, "../../shaders/glsl/opmult.comp.spv"))
    mgr.eval_async_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shader_path)
    mgr.eval_await_def()

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]
    assert np.all(tensor_out.numpy() == [2.0, 4.0, 6.0])


