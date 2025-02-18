//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUReconstructionOCL1.cxx
/// \author David Rohr

#define GPUCA_GPUTYPE_OPENCL
#define __OPENCL_HOST__

#include "GPUReconstructionOCL1.h"
#include "GPUReconstructionOCL1Internals.h"
#include "GPUReconstructionIncludes.h"

using namespace GPUCA_NAMESPACE::gpu;

#include <cstring>
#include <unistd.h>
#include <typeinfo>
#include <cstdlib>

#include "utils/opencl_obtain_program.h"
#include "utils/qGetLdBinarySymbols.h"
QGET_LD_BINARY_SYMBOLS(GPUReconstructionOCL1Code_bin);

GPUReconstruction* GPUReconstruction_Create_OCL(const GPUSettingsDeviceBackend& cfg) { return new GPUReconstructionOCL1(cfg); }

GPUReconstructionOCL1Backend::GPUReconstructionOCL1Backend(const GPUSettingsDeviceBackend& cfg) : GPUReconstructionOCL(cfg)
{
}

template <class T, int I, typename... Args>
int GPUReconstructionOCL1Backend::runKernelBackend(krnlSetup& _xyz, const Args&... args)
{
  cl_kernel k = _xyz.y.num > 1 ? getKernelObject<cl_kernel, T, I, true>() : getKernelObject<cl_kernel, T, I, false>();
  return runKernelBackendCommon(_xyz, k, args...);
}

template <class S, class T, int I, bool MULTI>
S& GPUReconstructionOCL1Backend::getKernelObject()
{
  static unsigned int krnl = FindKernel<T, I>(MULTI ? 2 : 1);
  return mInternals->kernels[krnl].first;
}

int GPUReconstructionOCL1Backend::GetOCLPrograms()
{
  cl_uint count;
  if (GPUFailedMsgI(clGetDeviceIDs(mInternals->platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &count))) {
    GPUError("Error getting OPENCL Device Count");
    return (1);
  }

  if (_makefiles_opencl_obtain_program_helper(mInternals->context, count, mInternals->devices.get(), &mInternals->program, _binary_GPUReconstructionOCL1Code_bin_start)) {
    clReleaseContext(mInternals->context);
    GPUError("Could not obtain OpenCL progarm");
    return 1;
  }

#define GPUCA_OPENCL1
#define GPUCA_KRNL(x_class, x_attributes, x_arguments, x_forward) \
  GPUCA_KRNL_WRAP(GPUCA_KRNL_LOAD_, x_class, x_attributes, x_arguments, x_forward)
#define GPUCA_KRNL_LOAD_single(x_class, x_attributes, x_arguments, x_forward) \
  if (AddKernel<GPUCA_M_KRNL_TEMPLATE(x_class)>(false)) {                     \
    return 1;                                                                 \
  }
#define GPUCA_KRNL_LOAD_multi(x_class, x_attributes, x_arguments, x_forward) \
  if (AddKernel<GPUCA_M_KRNL_TEMPLATE(x_class)>(true)) {                     \
    return 1;                                                                \
  }
#include "GPUReconstructionKernels.h"
#undef GPUCA_KRNL
#undef GPUCA_OPENCL1
#undef GPUCA_KRNL_LOAD_single
#undef GPUCA_KRNL_LOAD_multi

  return 0;
}

bool GPUReconstructionOCL1Backend::CheckPlatform(unsigned int i)
{
  char platform_version[64] = {}, platform_vendor[64] = {};
  clGetPlatformInfo(mInternals->platforms[i], CL_PLATFORM_VERSION, sizeof(platform_version), platform_version, nullptr);
  clGetPlatformInfo(mInternals->platforms[i], CL_PLATFORM_VENDOR, sizeof(platform_vendor), platform_vendor, nullptr);
  if (strcmp(platform_vendor, "Advanced Micro Devices, Inc.") == 0 && strstr(platform_version, "OpenCL 2.0 AMD-APP (") != nullptr) {
    float ver = 0;
    sscanf(platform_version, "OpenCL 2.0 AMD-APP (%f)", &ver);
    if (ver < 2000.f) {
      if (mProcessingSettings.debugLevel >= 2) {
        GPUInfo("AMD APP OpenCL Platform found");
      }
      return true;
    }
  }
  return false;
}
