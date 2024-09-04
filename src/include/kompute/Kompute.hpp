#pragma once

#include "Algorithm.hpp"
#include "Core.hpp"
#include "Image.hpp"
#include "Manager.hpp"
#include "Sequence.hpp"
#include "Tensor.hpp"

#include "operations/OpAlgoDispatch.hpp"
#include "operations/OpBase.hpp"
#include "operations/OpCopy.hpp"
#include "operations/OpMemoryBarrier.hpp"
#include "operations/OpMult.hpp"
#include "operations/OpSyncDevice.hpp"
#include "operations/OpSyncLocal.hpp"

// Will be build by CMake and placed inside the build directory
#include "ShaderLogisticRegression.hpp"
#include "ShaderOpMult.hpp"
