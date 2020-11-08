
from pyshader import python2shader, f32, ivec3, Array
from pyshader.stdlib import exp, log

from kp import Tensor, Manager, Sequence

def test_opmult():
    """
    Test basic OpMult operation
    """

    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr = Manager()

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_algo_mult_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]

def test_opalgobase_data():
    """
    Test basic OpAlgoBase operation
    """

    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr = Manager()

    shaderData = """
        #version 450

        layout (local_size_x = 1) in;

        // The input tensors bind index is relative to index in parameter passed
        layout(set = 0, binding = 0) buffer bina { float tina[]; };
        layout(set = 0, binding = 1) buffer binb { float tinb[]; };
        layout(set = 0, binding = 2) buffer bout { float tout[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            tout[index] = tina[index] * tinb[index];
        }
    """

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_algo_str_def([tensor_in_a, tensor_in_b, tensor_out], list(shaderData))

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]


def test_opalgobase_file():
    """
    Test basic OpAlgoBase operation
    """

    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr = Manager()

    shaderFilePath = "../../shaders/glsl/opmult.comp"

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    mgr.eval_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shaderFilePath)

    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]

def test_sequence():
    """
    Test basic OpAlgoBase operation
    """
    mgr = Manager(0, [2])
    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])
    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])
    seq = mgr.create_sequence("op")
    shaderFilePath = "../../shaders/glsl/opmult.comp"
    mgr.eval_async_algo_file_def([tensor_in_a, tensor_in_b, tensor_out], shaderFilePath)
    mgr.eval_await_def()
    seq.begin()
    seq.record_tensor_sync_local([tensor_in_a])
    seq.record_tensor_sync_local([tensor_in_b])
    seq.record_tensor_sync_local([tensor_out])
    seq.end()
    seq.eval()
    assert tensor_out.data() == [2.0, 4.0, 6.0]

def test_pyshader_pyshader():

    @python2shader
    def compute_shader_multiply(index=("input", "GlobalInvocationId", ivec3),
                                data1=("buffer", 0, Array(f32)),
                                data2=("buffer", 1, Array(f32)),
                                data3=("buffer", 2, Array(f32))):
        i = index.x
        data3[i] = data1[i] * data2[i]

    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr = Manager()

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])
    mgr.eval_algo_data_def([tensor_in_a, tensor_in_b, tensor_out], compute_shader_multiply.to_spirv())
    mgr.eval_tensor_sync_local_def([tensor_out])

    assert tensor_out.data() == [2.0, 4.0, 6.0]

def test_logistic_regression_pyshader():
    @python2shader
    def compute_shader(
            index   = ("input", "GlobalInvocationId", ivec3),
            x_i     = ("buffer", 0, Array(f32)),
            x_j     = ("buffer", 1, Array(f32)),
            y       = ("buffer", 2, Array(f32)),
            w_in    = ("buffer", 3, Array(f32)),
            w_out_i = ("buffer", 4, Array(f32)),
            w_out_j = ("buffer", 5, Array(f32)),
            b_in    = ("buffer", 6, Array(f32)),
            b_out   = ("buffer", 7, Array(f32)),
            l_out   = ("buffer", 8, Array(f32)),
            M       = ("buffer", 9, Array(f32))):

        i = index.x

        m = M[0]

        w_curr = vec2(w_in[0], w_in[1])
        b_curr = b_in[0]

        x_curr = vec2(x_i[i], x_j[i])
        y_curr = y[i]

        z_dot = w_curr @ x_curr
        z = z_dot + b_curr
        y_hat = 1.0 / (1.0 + exp(-z))

        d_z = y_hat - y_curr
        d_w = (1.0 / m) * x_curr * d_z
        d_b = (1.0 / m) * d_z

        loss = -((y_curr * log(y_hat)) + ((1.0 + y_curr) * log(1.0 - y_hat)))

        w_out_i[i] = d_w.x
        w_out_j[i] = d_w.y
        b_out[i] = d_b
        l_out[i] = loss


    # First we create input and ouput tensors for shader
    tensor_x_i = Tensor([0.0, 1.0, 1.0, 1.0, 1.0])
    tensor_x_j = Tensor([0.0, 0.0, 0.0, 1.0, 1.0])

    tensor_y = Tensor([0.0, 0.0, 0.0, 1.0, 1.0])

    tensor_w_in = Tensor([0.001, 0.001])
    tensor_w_out_i = Tensor([0.0, 0.0, 0.0, 0.0, 0.0])
    tensor_w_out_j = Tensor([0.0, 0.0, 0.0, 0.0, 0.0])

    tensor_b_in = Tensor([0.0])
    tensor_b_out = Tensor([0.0, 0.0, 0.0, 0.0, 0.0])

    tensor_l_out = Tensor([0.0, 0.0, 0.0, 0.0, 0.0])

    tensor_m = Tensor([ 5.0 ])

    # We store them in an array for easier interaction
    params = [tensor_x_i, tensor_x_j, tensor_y, tensor_w_in, tensor_w_out_i,
        tensor_w_out_j, tensor_b_in, tensor_b_out, tensor_l_out, tensor_m]

    mgr = Manager()

    mgr.eval_tensor_create_def(params)

    # Record commands for efficient evaluation
    sq = mgr.create_sequence()
    sq.begin()
    sq.record_tensor_sync_device([tensor_w_in, tensor_b_in])
    sq.record_algo_data(params, compute_shader.to_spirv())
    sq.record_tensor_sync_local([tensor_w_out_i, tensor_w_out_j, tensor_b_out, tensor_l_out])
    sq.end()

    ITERATIONS = 100
    learning_rate = 0.1

    # Perform machine learning training and inference across all input X and Y
    for i_iter in range(ITERATIONS):
        sq.eval()

        # Calculate the parameters based on the respective derivatives calculated
        w_in_i_val = tensor_w_in.data()[0]
        w_in_j_val = tensor_w_in.data()[1]
        b_in_val = tensor_b_in.data()[0]

        for j_iter in range(tensor_b_out.size()):
            w_in_i_val -= learning_rate * tensor_w_out_i.data()[j_iter]
            w_in_j_val -= learning_rate * tensor_w_out_j.data()[j_iter]
            b_in_val -= learning_rate * tensor_b_out.data()[j_iter]

        # Update the parameters to process inference again
        tensor_w_in.set_data([w_in_i_val, w_in_j_val])
        tensor_b_in.set_data([b_in_val])

    assert tensor_w_in.data()[0] < 0.01
    assert tensor_w_in.data()[0] > 0.0
    assert tensor_w_in.data()[1] > 1.5
    assert tensor_b_in.data()[0] < 0.7

