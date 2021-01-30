
Python Examples
========

This section contains simple and advanced examples using the Python Kompute class. For an overview of the module check `Python Package Overview <python-package.html>`_, for a deep dive into functions check the `Python Class Reference Section <python-reference.html>`_.

You will be able to run the examples below by installing the dependencies in `python/test/requirements-dev.txt`

Python Example (Simple)
^^^^^

Then you can interact with it from your interpreter. Below is the same sample as above "Your First Kompute (Simple Version)" but in Python:

.. code-block:: python
   :linenos:

   from kp import Manager, Tensor
   from pyshader import python2shader, ivec3, f32, Array

   mgr = Manager()

   # Can be initialized with List[] or np.Array
   tensor_in_a = Tensor([2, 2, 2])
   tensor_in_b = Tensor([1, 2, 3])
   tensor_out = Tensor([0, 0, 0])

   mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

   # Define the function via PyShader or directly as glsl string or spirv bytes
   @python2shader
   def compute_shader_multiply(index=("input", "GlobalInvocationId", ivec3),
                               data1=("buffer", 0, Array(f32)),
                               data2=("buffer", 1, Array(f32)),
                               data3=("buffer", 2, Array(f32))):
       i = index.x
       data3[i] = data1[i] * data2[i]

   # Run shader operation synchronously
   mgr.eval_algo_data_def(
       [tensor_in_a, tensor_in_b, tensor_out], compute_shader_multiply.to_spirv())

   mgr.eval_await_def()

   mgr.eval_tensor_sync_local_def([tensor_out])

   assert tensor_out.data() == [2.0, 4.0, 6.0]


Python Example (Extended)
^^^^^

Similarly you can find the same extended example as above:

.. code-block:: python
   :linenos:

    from kp import Manager, Tensor
    from pyshader import python2shader, ivec3, f32, Array

    mgr = Manager(0, [2])

    # Can be initialized with List[] or np.Array
    tensor_in_a = Tensor([2, 2, 2])
    tensor_in_b = Tensor([1, 2, 3])
    tensor_out = Tensor([0, 0, 0])

    mgr.eval_tensor_create_def([tensor_in_a, tensor_in_b, tensor_out])

    seq = mgr.create_sequence("op")

    # Define the function via PyShader or directly as glsl string or spirv bytes
    @python2shader
    def compute_shader_multiply(index=("input", "GlobalInvocationId", ivec3),
                                data1=("buffer", 0, Array(f32)),
                                data2=("buffer", 1, Array(f32)),
                                data3=("buffer", 2, Array(f32))):
        i = index.x
        data3[i] = data1[i] * data2[i]

    # Run shader operation asynchronously and then await
    mgr.eval_async_algo_data_def(
        [tensor_in_a, tensor_in_b, tensor_out], compute_shader_multiply.to_spirv())
    mgr.eval_await_def()

    seq.begin()
    seq.record_tensor_sync_local([tensor_in_a])
    seq.record_tensor_sync_local([tensor_in_b])
    seq.record_tensor_sync_local([tensor_out])
    seq.end()

    seq.eval()

    assert tensor_out.data() == [2.0, 4.0, 6.0]

Kompute Operation Capabilities
^^^^^

Handling multiple capabilites of processing can be done by compute shaders being loaded into separate sequences. The example below shows how this can be done:

.. code-block:: python
   :linenos:

    from kp import Manager

    # We'll assume we have the shader data available
    from my_spv_shader_data import mult_shader, sum_shader

    mgr = Manager()

    t1 = mgr.build_tensor([2,2,2])
    t2 = mgr.build_tensor([1,2,3])
    t3 = mgr.build_tensor([1,2,3])

    # Create multiple separate sequences
    sq_mult = mgr.create_sequence("SQ_MULT")
    sq_sum = mgr.create_sequence("SQ_SUM")
    sq_sync = mgr.create_sequence("SQ_SYNC")

    # Initialize sq_mult
    sq_mult.begin()
    sq_mult.record_algo_data([t1, t2, t3], add_shader)
    sq_mult.end()

    sq_sum.begin()
    sq_sum.record_algo_data([t3, t2, t1], sum_shader)
    sq_sum.end()

    sq_sync.begin()
    sq_sync.record_tensor_sync_local([t1, t3])
    sq_sync.end()

    # Run multiple iterations
    for i in range(10):
        sq_mult.eval()
        sq_sum.eval()

    sq_sync.eval()

    print(t1.data(), t2.data(), t3.data())

Machine Learning Logistic Regression Implementation
^^^^^^

Similar to the logistic regression implementation in the C++ examples section, below you can find the Python implementation of the Logistic Regression algorithm.

.. code-block:: python
   :linenos:

    from kp import Manager, Tensor
    from pyshader import python2shader, ivec3, f32, Array

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

    # Print outputs
    print(tensor_w_in.data())
    print(tensor_b_in.data())

