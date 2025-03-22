﻿/**
 * @file Base.hpp
 * @author NaruseMioShirakana
 * @email shirakanamio@foxmail.com
 * @copyright Copyright (C) 2022-2025 NaruseMioShirakana (shirakanamio@foxmail.com)
 * @license GNU Affero General Public License v3.0
 * @attentions
 *  - This file is part of DragonianLib.
 *  - DragonianLib is free software: you can redistribute it and/or modify it under the terms of the
 *  - GNU Affero General Public License as published by the Free Software Foundation, either version 3
 *  - of the License, or any later version.
 *
 *  - DragonianLib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  - without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  - See the GNU Affero General Public License for more details.
 *
 *  - You should have received a copy of the GNU Affero General Public License along with Foobar.
 *  - If not, see <https://www.gnu.org/licenses/agpl-3.0.html>.
 * @brief Base class of SingingVoiceConversion
 * @changes
 *  > 2025/3/21 NaruseMioShirakana Created <
 *	> 2025/3/22 NaruseMioShirakana Add SingingVoiceConversionModule <
 */

#pragma once
#include "OnnxLibrary/SingingVoiceConversion/Util/Util.hpp"

_D_Dragonian_Lib_Lib_Singing_Voice_Conversion_Header

class SingingVoiceConversionModule
{
public:

	SingingVoiceConversionModule() = delete;
	SingingVoiceConversionModule(const HParams& Params);
	SingingVoiceConversionModule(const SingingVoiceConversionModule&) = default;
	SingingVoiceConversionModule(SingingVoiceConversionModule&&) noexcept = default;
	SingingVoiceConversionModule& operator=(const SingingVoiceConversionModule&) = default;
	SingingVoiceConversionModule& operator=(SingingVoiceConversionModule&&) noexcept = default;
	virtual ~SingingVoiceConversionModule() = default;

	Tensor<Float32, 4, Device::CPU> Inference(
		const Parameters& Params,
		const Tensor<Float32, 3, Device::CPU>& Audio,
		SizeType SourceSampleRate,
		const FeatureExtractor& UnitsEncoder,
		const PitchExtractor& F0Extractor,
		const PitchParameters& F0Params,
		std::optional<std::reference_wrapper<const Cluster>> UnitsCluster = std::nullopt,
		std::optional<std::reference_wrapper<const Tensor<Float32, 3, Device::CPU>>> AudioMask = std::nullopt
	) const;

	SliceDatas PreProcess(
		const Parameters& Params,
		const SliceDatas& InferenceDatas
	) const;

	/**
	 * @brief Inference
	 * @param Params Parameters
	 * @param InputDatas Input datas
	 * @return Tensor<Float32, 4, Device::CPU>
	 */
	virtual Tensor<Float32, 4, Device::CPU> Inference(
		const Parameters& Params,
		const SliceDatas& InputDatas
	) const = 0;

	/**
	 * @brief Check arguments and preprocess input datas for inference
	 * @param Params Inference parameters
	 * @param InputDatas 
	 * @return InputDatas
	 */
	virtual SliceDatas PreProcess(
		const Parameters& Params,
		SliceDatas&& InputDatas
	) const = 0;

protected:
	Int64 _MyOutputSamplingRate = 32000;
	Int64 _MyUnitsDim = 256;
	Int64 _MyHopSize = 512;
	Int64 _MySpeakerCount = 1;
	Float32 _MySpecMax = 2.f;
	Float32 _MySpecMin = -12.f;
	Int64 _MyF0Bin = 256;
	Float32 _MyF0Max = 1100.0;
	Float32 _MyF0Min = 50.0;
	Float32 _MyF0MelMax = 1127.f * log(1.f + _MyF0Max / 700.f);
	Float32 _MyF0MelMin = 1127.f * log(1.f + _MyF0Min / 700.f);
	bool _HasVolumeEmbedding = false;
	bool _HasSpeakerEmbedding = false;
	bool _HasSpeakerMixLayer = false;

public:
	static Tensor<Float32, 4, Device::CPU> NormSpec(
		const Tensor<Float32, 4, Device::CPU>& Spec,
		float SpecMax = 2.f,
		float SpecMin = -12.f
	);

	static Tensor<Float32, 4, Device::CPU> DenormSpec(
		const Tensor<Float32, 4, Device::CPU>& Spec,
		float SpecMax = 2.f,
		float SpecMin = -12.f
	);

	Tensor<Float32, 4, Device::CPU> NormSpec(
		const Tensor<Float32, 4, Device::CPU>& Spec
	) const;

	Tensor<Float32, 4, Device::CPU> DenormSpec(
		const Tensor<Float32, 4, Device::CPU>& Spec
	) const;

	static Tensor<Float32, 3, Device::CPU> ExtractVolume(
		const Tensor<Float32, 3, Device::CPU>& Audio,
		Int64 HopSize, Int64 WindowSize
	);

	static Tensor<Int64, 3, Device::CPU> GetF0Embed(
		const Tensor<Float32, 3, Device::CPU>& F0,
		Float32 F0Bin, Float32 F0MelMax, Float32 F0MelMin
	);

	static Tensor<Float32, 4, Device::CPU> GetGTSpec(
		const Tensor<Float32, 4, Device::CPU>& Mel,
		const Tensor<Float32, 4, Device::CPU>& Audio,
		Int64 HopSize, Int64 WindowSize
	);

protected:
	std::optional<ProgressCallback> _MyProgressCallback;

public:
	Int64 GetSamplingRate() const noexcept
	{
		return _MyOutputSamplingRate;
	}

	Int64 GetUnitsDim() const noexcept
	{
		return _MyUnitsDim;
	}

	Int64 GetHopSize() const noexcept
	{
		return _MyHopSize;
	}

	Int64 GetSpeakerCount() const noexcept
	{
		return _MySpeakerCount;
	}

	Float32 GetSpecMax() const noexcept
	{
		return _MySpecMax;
	}

	Float32 GetSpecMin() const noexcept
	{
		return _MySpecMin;
	}

	Int64 GetF0Bin() const noexcept
	{
		return _MyF0Bin;
	}

	Float32 GetF0Max() const noexcept
	{
		return _MyF0Max;
	}

	Float32 GetF0Min() const noexcept
	{
		return _MyF0Min;
	}

	Float32 GetF0MelMax() const noexcept
	{
		return _MyF0MelMax;
	}

	Float32 GetF0MelMin() const noexcept
	{
		return _MyF0MelMin;
	}

	bool HasVolumeEmbedding() const noexcept
	{
		return _HasVolumeEmbedding;
	}

	bool HasSpeakerEmbedding() const noexcept
	{
		return _HasSpeakerEmbedding;
	}

	bool HasSpeakerMixLayer() const noexcept
	{
		return _HasSpeakerMixLayer;
	}
};

_D_Dragonian_Lib_Lib_Singing_Voice_Conversion_End