
Reference
========

This section provides a breakdown of the cpp classes and what each of their functions provide. It is partially generated and augomented from the Doxygen autodoc content. You can also go directly to the `raw doxygen docs <../doxygen/annotated.html>`_.

Below is a diagram that provides insights on the relationship between Vulkan Kompute objects and Vulkan resources, which primarily encompass ownership of either CPU and/or GPU memory.

.. image:: ../images/kompute-vulkan-architecture.jpg
   :width: 100%

Manager
-------

.. doxygenclass:: kp::Manager
   :members:

.. image:: ../images/kompute-vulkan-architecture-manager.jpg
   :width: 100%

Sequence
-------

.. doxygenclass:: kp::Sequence
   :members:

.. image:: ../images/kompute-vulkan-architecture-sequence.jpg
   :width: 100%

Tensor
-------

.. doxygenclass:: kp::Tensor
   :members:

.. image:: ../images/kompute-vulkan-architecture-tensor.jpg
   :width: 100%

Algorithm
-------

.. doxygenclass:: kp::Algorithm
   :members:

.. image:: ../images/kompute-vulkan-architecture-algorithm.jpg
   :width: 100%

OpBase
-------

.. doxygenclass:: kp::OpBase
   :members:

.. image:: ../images/kompute-vulkan-architecture-operations.jpg
   :width: 100%

OpAlgoBase
-------

.. doxygenclass:: kp::OpAlgoBase
   :members:

.. image:: ../images/kompute-vulkan-architecture-opmult.jpg
   :width: 100%

OpMult
-------

.. doxygenclass:: kp::OpMult
   :members:

.. image:: ../images/kompute-vulkan-architecture-opmult.jpg
   :width: 100%

OpTensorCreate
-------

.. doxygenclass:: kp::OpTensorCreate
   :members:

.. image:: ../images/kompute-vulkan-architecture-opcreatetensor.jpg
   :width: 100%

OpTensorCreate
-------

.. doxygenclass:: kp::OpTensorCopy
   :members:


