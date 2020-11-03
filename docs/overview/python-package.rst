
Python Package Overview
========

This section provides an overview of the Python Package from a functionality perspective. If you wish to see all the classes and their respective functions you can find that in the `Python Class Reference Section <python-reference>`_.

Below is a diagram that provides insights on the relationship between Vulkan Kompute objects and Vulkan resources, which primarily encompass ownership of either CPU and/or GPU memory.

.. image:: ../images/kompute-architecture.jpg
   :width: 70%

Python Components
^^^^^^^^

The Python package exposes three main classes:

* :class:`kp.Manager` - Manages all high level Vulkan and Kompute resources created
* :class:`kp.Sequence` - Contains a set of recorded operations that can be reused
* :class:`kp.Tensor` - Core data component to manage GPU and host data used in operations

One thing that you will notice is that the class :class:`kp::OpBase` and all its relevant operator subclasses are not exposed in Python.

This is primarily because the way to interact with the operations are through the respective :class:`kp.Manager` and :class:`kp.Sequence` functions.

More specifically, it can be through the following functions:

* mgr.eval_<opname> - Runs operation under an existing named sequence
* mgr.eval_<opname>_def - Runs operation under a new anonymous sequence
* mgr.eval_async_<opname> - Runs operation asynchronously under an existing named sequence
* mgr.eval_async_<opname>_def - Runs operation asynchronously under a new anonymous sequence
* seq.record_<opname> - Records operation in sequence (requires sequence to be in recording mode)

You can see these operations being used in the `Simple Python example <https://kompute.cc/index.html#python-example-simple>`_ and in the `Extended Python Example <https://kompute.cc/index.html#python-example-extended>`_.

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


Package Installation 
^^^^^^^^^

The package can be installed through the top level `setup.py` by running:

```
pip install .
```

You can configure log level with the function `kp.log_level` as outlined below.

The values are TRACE=0, DEBUG=1, INFO=2, WARN=3, ERROR=4. Kompute defaults to INFO.

```
import kp
kp.log_level(1)
```


