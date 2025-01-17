/*
==============================================================================

BEGIN_JUCE_MODULE_DECLARATION

   ID:            chowdsp_dsp_utils
   vendor:        Chowdhury DSP
   version:       1.1.0
   name:          ChowDSP DSP Utilities
   description:   Commonly used DSP utilities for ChowDSP plugins
   dependencies:  chowdsp_filters

   website:       https://ccrma.stanford.edu/~jatin/chowdsp
   license:       GPLv3

   END_JUCE_MODULE_DECLARATION

==============================================================================
*/

#pragma once

/** Config: CHOWDSP_USE_LIBSAMPLERATE
           Enable libsamplerate wrappers for resampling processors.

           You must link against libsamplerate, and make sure that libsamplerate headers
           are in the include paths. You must respect the libsamplerate license when enabling this option.
 */
#ifndef CHOWDSP_USE_LIBSAMPLERATE
#define CHOWDSP_USE_LIBSAMPLERATE 0
#endif

// STL includes
#include <random>

// JUCE includes
#include <chowdsp_filters/chowdsp_filters.h>

#if CHOWDSP_USE_LIBSAMPLERATE
#include <samplerate.h>
#endif

// delay
#include "Delay/chowdsp_DelayInterpolation.h"
#include "Delay/chowdsp_DelayLine.h"
#include "Delay/chowdsp_PitchShift.h"
#include "Delay/BBD/chowdsp_BBDFilterBank.h"
#include "Delay/BBD/chowdsp_BBDDelayLine.h"
#include "Delay/BBD/chowdsp_BBDDelayWrapper.h"

// modal tools
#include "Modal/chowdsp_ModalFilter.h"
#include "Modal/chowdsp_ModalFilterBank.h"

// resamplers
#include "Resampling/chowdsp_Upsampler.h"
#include "Resampling/chowdsp_Downsampler.h"
#include "Resampling/chowdsp_BaseResampler.h"
#include "Resampling/chowdsp_LanczosResampler.h"
#include "Resampling/chowdsp_ResamplingProcessor.h"
#include "Resampling/chowdsp_SRCResampler.h"
#include "Resampling/chowdsp_ResampledProcess.h"

// some other useful processors
#include "Processors/chowdsp_AudioTimer.h"
#include "Processors/chowdsp_BypassProcessor.h"
#include "Processors/chowdsp_Gain.h"
#include "Processors/chowdsp_LevelDetector.h"
#include "Processors/chowdsp_Panner.h"
#include "Processors/chowdsp_TunerProcessor.h"

// sound sources
#include "Sources/chowdsp_SawtoothWave.h"
#include "Sources/chowdsp_SineWave.h"
#include "Sources/chowdsp_SquareWave.h"

#if CHOWDSP_USING_JUCE
#include <juce_audio_processors/juce_audio_processors.h>

#if JUCE_MODULE_AVAILABLE_juce_dsp
#include <juce_dsp/juce_dsp.h>

// convolution
#include "Convolution/chowdsp_ConvolutionEngine.h"
#include "Convolution/chowdsp_IRTransfer.h"
#include "Convolution/chowdsp_IRHelpers.h"

// resamplers
#include "Resampling/chowdsp_VariableOversampling.h"

// sound sources
#include "Sources/chowdsp_Noise.h"
#include "Sources/chowdsp_RepitchedSource.h"
#endif

// some other useful processors
#include "Processors/chowdsp_GainProcessor.h"

#endif
