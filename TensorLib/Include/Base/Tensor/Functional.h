﻿/**
 * @file Functional.h
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
 * @brief Functional
 * @changes
 *  > 2025/3/22 NaruseMioShirakana Refactored <
 */

#pragma once
#include "Tensor.h"
#include "Libraries/NumpySupport/NumpyFileFormat.h"
#include <ostream>

_D_Dragonian_Lib_Space_Begin

template <typename ..._TIndices>
_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) IDim(_TIndices&& ..._Indices)
{
	return Dimensions<sizeof...(_TIndices)>({ std::forward<_TIndices>(_Indices)... });
}

template <typename ..._TIndices>
_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) ISize(_TIndices&& ..._Indices)
{
	return Dimensions<sizeof...(_TIndices)>({ std::forward<_TIndices>(_Indices)... });
}

template <typename ..._TIndices>
_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) IScale(_TIndices&& ..._Indices)
{
	return IDLArray<double, sizeof...(_TIndices)>({ std::forward<_TIndices>(_Indices)... });
}

template <typename ..._ValueTypes>
_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) IArray(_ValueTypes&& ..._Values)
{
	return IDLArray<TypeTraits::GetVaListTypeAtType<0, _ValueTypes...>, sizeof...(_ValueTypes)>({ std::forward<_ValueTypes>(_Values)... });
}

template <typename ..._ArgTypes>
_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) IRanges(_ArgTypes&& ..._Args)
{
	return VRanges<sizeof...(_ArgTypes)>({ std::forward<_ArgTypes>(_Args)... });
}

namespace Functional
{
	class FunctionalImpl
	{
	public:
		FunctionalImpl() = delete;
		template <typename _MyValueType, size_t _NRankA, size_t _NRankB, Device _MyDevice, typename = std::enable_if_t<Operators::BinaryOperators::MaxBinary::HasOperatorValue<_MyValueType>>>
		static decltype(auto) Max(const Tensor<_MyValueType, _NRankA, _MyDevice>& _TensorA, const Tensor<_MyValueType, _NRankB, _MyDevice>& _TensorB)
		{
			_TensorA.WaitingAsArgument();
			_TensorB.WaitingAsArgument();
			auto BroadCasted = Tensor<_MyValueType, _NRankA, _MyDevice>::BroadCast(_TensorA, _TensorB);
			auto Ret = Tensor<_MyValueType, MaxOf(_NRankA, _NRankB), _MyDevice>::New(BroadCasted.first.Shape());
			Ret.WaitingAsResult();
			Operators::OperatorsBase<_MyValueType, _MyDevice>::ImplMaxTensor(
				Ret.Data(),
				Ret.GetDefaultOperatorParameter(),
				BroadCasted.first.Data(),
				BroadCasted.first.GetDefaultOperatorParameter(),
				BroadCasted.second.Data(),
				BroadCasted.second.GetDefaultOperatorParameter(),
				!BroadCasted.first.IsBroadCasted() && BroadCasted.first.IsContinuous() &&
				!BroadCasted.second.IsBroadCasted() && BroadCasted.second.IsContinuous()
			);
			return Ret;
		}

		template <typename _MyValueType, size_t _NRankA, size_t _NRankB, Device _MyDevice, typename = std::enable_if_t<Operators::BinaryOperators::MinBinary::HasOperatorValue<_MyValueType>>>
		static decltype(auto) Min(const Tensor<_MyValueType, _NRankA, _MyDevice>& _TensorA, const Tensor<_MyValueType, _NRankB, _MyDevice>& _TensorB)
		{
			_TensorA.WaitingAsArgument();
			_TensorB.WaitingAsArgument();
			auto BroadCasted = Tensor<_MyValueType, _NRankA, _MyDevice>::BroadCast(_TensorA, _TensorB);
			auto Ret = Tensor<_MyValueType, MaxOf(_NRankA, _NRankB), _MyDevice>::New(BroadCasted.first.Shape());
			Ret.WaitingAsResult();
			Operators::OperatorsBase<_MyValueType, _MyDevice>::ImplMinTensor(
				Ret.Data(),
				Ret.GetDefaultOperatorParameter(),
				BroadCasted.first.Data(),
				BroadCasted.first.GetDefaultOperatorParameter(),
				BroadCasted.second.Data(),
				BroadCasted.second.GetDefaultOperatorParameter(),
				!BroadCasted.first.IsBroadCasted() && BroadCasted.first.IsContinuous() &&
				!BroadCasted.second.IsBroadCasted() && BroadCasted.second.IsContinuous()
			);
			return Ret;
		}

	};

	namespace FunctionalTraits
	{
		template <typename>
		struct IsTensorType : std::false_type {};
		template <typename _ValueType, size_t _NRank, Device _MyDevice>
		struct IsTensorType<Tensor<_ValueType, _NRank, _MyDevice>> : std::true_type {};
		template <typename _Type>
		constexpr static bool IsTensorTypeValue = IsTensorType<TypeTraits::RemoveARPCVType<_Type>>::value;

		template <typename _Header, typename ..._Rest>
		struct IntegerArgumentCount
		{
			constexpr static size_t Value = IntegerArgumentCount<_Rest...>::Value + (TypeTraits::IsIntegerValue<_Header> ? 1 : 0);
		};
		template <typename _Header>
		struct IntegerArgumentCount<_Header>
		{
			constexpr static size_t Value = TypeTraits::IsIntegerValue<_Header> ? 1 : 0;
		};

		template <typename _Header, typename ..._Rest>
		struct TensorArgumentCount
		{
			constexpr static size_t Value = TensorArgumentCount<_Rest...>::Value + (IsTensorTypeValue<_Header> ? 1 : 0);
		};
		template <typename _Header>
		struct TensorArgumentCount<_Header>
		{
			constexpr static size_t Value = IsTensorTypeValue<_Header> ? 1 : 0;
		};

		template <typename _Header, typename ..._Rest>
		struct FindFirstTensorArgument
		{
			constexpr static size_t Value = IsTensorTypeValue<_Header> ? 0 : 1 + FindFirstTensorArgument<_Rest...>::Value;
		};
		template <typename _Header>
		struct FindFirstTensorArgument<_Header>
		{
			constexpr static size_t Value = IsTensorTypeValue<_Header> ? 0 : 1;
		};

		template <typename _Header, typename ..._Rest>
		struct CheckArgumentIsAllTensorOrInteger
		{
			constexpr static bool Value = (IsTensorTypeValue<_Header> || TypeTraits::IsIntegerValue<_Header>) && CheckArgumentIsAllTensorOrInteger<_Rest...>::Value;
		};
		template <typename _Header>
		struct CheckArgumentIsAllTensorOrInteger<_Header>
		{
			constexpr static bool Value = IsTensorTypeValue<_Header> || TypeTraits::IsIntegerValue<_Header>;
		};

		template <typename _TensorType, typename _Header, typename ..._Rest>
		struct AllTensorTypeIsSame
		{
			constexpr static bool Value = (TypeTraits::IsIntegerValue<_Header> || TypeTraits::IsSameTypeValue<TypeTraits::RemoveARPCVType<_TensorType>, TypeTraits::RemoveARPCVType<_Header>>) && AllTensorTypeIsSame<_TensorType, _Rest...>::Value && !TypeTraits::IsPointerValue<_Header>;
		};
		template <typename _TensorType, typename _Header>
		struct AllTensorTypeIsSame<_TensorType, _Header>
		{
			constexpr static bool Value = TypeTraits::IsIntegerValue<_Header> || TypeTraits::IsSameTypeValue<TypeTraits::RemoveARPCVType<_TensorType>, TypeTraits::RemoveARPCVType<_Header>> && !TypeTraits::IsPointerValue<_Header>;
		};

		template <typename ..._ArgTypes>
		struct StackCatTraits
		{
			template <size_t>
			static void _StackCat() {}
			template <size_t _Index, typename _First, typename ..._Rest>
			decltype(auto) _StackCat(_First&& _FirstArg, _Rest&& ..._Args)
			{
				if constexpr (TypeTraits::IsIntegerValue<_First>)
				{
					if (_Axis == 0)
						_Axis = _FirstArg;
					_StackCat<_Index>(std::forward<_Rest>(_Args)...);
				}
				else if constexpr (IsTensorTypeValue<_First>)
				{
					_Tensors[_Index] = std::forward<_First>(_FirstArg).View();
					_StackCat<_Index + 1>(std::forward<_Rest>(_Args)...);
				}
				else
					_StackCat<_Index>(std::forward<_Rest>(_Args)...);
			}
			StackCatTraits() = delete;
			StackCatTraits(_ArgTypes&& ..._Args)
			{
				_StackCat<0>(std::forward<_ArgTypes>(_Args)...);
			}

			static constexpr auto _MyArgumentCount = sizeof...(_ArgTypes);
			static constexpr auto _MyIntegerArgumentCount = IntegerArgumentCount<_ArgTypes...>::Value;
			static constexpr auto _MyTensorArgumentCount = TensorArgumentCount<_ArgTypes...>::Value;

			static_assert(_MyTensorArgumentCount > 0, "At least one tensor argument is required.");

			static constexpr bool _HasTensorArgument = _MyTensorArgumentCount > 0;
			static constexpr bool _HasOnlyOneIntegerArgument = _MyIntegerArgumentCount < 2;

			static constexpr auto _MyFirstTensorArgumentIndex = FindFirstTensorArgument<_ArgTypes...>::Value;

			using _MyFirstTensorArgumentType = TypeTraits::ConditionalType<
				_HasTensorArgument,
				TypeTraits::GetVaListTypeAtType<MinOf(_MyArgumentCount - 1, _MyFirstTensorArgumentIndex), _ArgTypes...>,
				Tensor<void, 1, Device::CUSTOM>
			>;
			static constexpr bool _MyFirstTensorTypeIsNotPointer = !TypeTraits::IsPointerValue<_MyFirstTensorArgumentType>;
			using _MyFirstTensorArgumentTypeUnref = TypeTraits::RemoveARPCVType<_MyFirstTensorArgumentType>;
			static constexpr bool _AllTensorTypeIsSame = AllTensorTypeIsSame<_MyFirstTensorArgumentTypeUnref, _ArgTypes...>::Value;

			static constexpr auto _MyFirstTensorArgumentRank = _MyFirstTensorArgumentTypeUnref::Rank();
			using _MyFirstTensorArgumentValueType = typename _MyFirstTensorArgumentTypeUnref::ValueType;
			static constexpr auto _MyFirstTensorArgumentDevice = _MyFirstTensorArgumentTypeUnref::_Device;

			static constexpr bool _IsDefaultConstructible = std::is_default_constructible_v<_MyFirstTensorArgumentValueType>;

			static constexpr auto _Enabled = _HasOnlyOneIntegerArgument && _HasTensorArgument && _MyFirstTensorTypeIsNotPointer && _AllTensorTypeIsSame && _IsDefaultConstructible;

			_MyFirstTensorArgumentTypeUnref _Tensors[_MyTensorArgumentCount];
			SizeType _Axis = 0;
		};
		template <>
		struct StackCatTraits<>
		{
			StackCatTraits() = delete;
			static constexpr auto _Enabled = false;
		};

		template <typename ..._ArgTypes>
		StackCatTraits(_ArgTypes&& ...) -> StackCatTraits<_ArgTypes...>;
	}

	template <typename _MyValueType>
	void SimpleDrawVector(
		const TemplateLibrary::ConstantRanges<_MyValueType>& _MyData,
		std::ostream& _Stream, const int _GraphHeight = 10, const int _GraphWidth = 70
	) {
		if (!_MyData.Size()) {
			_Stream << "No data to display.\n";
			return;
		}
		auto [MinIt, MaxIt] = std::minmax_element(_MyData.Begin(), _MyData.End());
		double MinValue = static_cast<double>(*MinIt); double MaxValue = static_cast<double>(*MaxIt);
		TemplateLibrary::Vector<double> GraphData(_GraphWidth);
		TemplateLibrary::Resample(
			_MyData.Data(),
			_MyData.Size(),
			GraphData.Data(),
			static_cast<size_t>(_GraphWidth)
		);
		for (auto& Value : GraphData)
			Value = std::round((Value - MinValue) / (MaxValue - MinValue) * (_GraphHeight - 1));

		for (size_t x = 0; x < _GraphWidth + 2; ++x)
			_Stream << "-";
		_Stream << "\n";
		for (int y = _GraphHeight - 1; y >= 0; --y) {
			_Stream << "|";
			for (auto& Value : GraphData) {
				if (static_cast<int>(Value) == y)
					_Stream << "*";
				else
					_Stream << " ";
			}
			_Stream << "|\n";
		}
		for (size_t x = 0; x < _GraphWidth + 2; ++x)
			_Stream << "-";
		_Stream << "\n";
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice>
	void NumpySave(
		const std::wstring& _Path,
		const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor
	)
	{
		NumpyFileFormat::SaveNumpyFile(_Path, _Tensor.Shape(), _Tensor.Data(), _Tensor.TotalSize());
	}

	template <typename _MyValueType = Float32, size_t _NRank, Device _MyDevice = Device::CPU>
	Tensor<_MyValueType, _NRank, _MyDevice> NumpyLoad(const std::wstring& _Path)
	{
		auto [VecShape, VecData] = NumpyFileFormat::LoadNumpyFile(_Path);

		Dimensions<_NRank> Shape;
		if (VecShape.Size() != _NRank)
			_D_Dragonian_Lib_Throw_Exception("The rank of the tensor is not compatible with the numpy file.");
		Shape.Assign(VecShape.Data());
		auto Alloc = VecData.GetAllocator();
		auto Ret = VecData.Release();
		Ret.second /= sizeof(_MyValueType);
		if (Ret.second != static_cast<size_t>(Shape.Multiply()))
			_D_Dragonian_Lib_Throw_Exception("The data size of the tensor is not compatible with the numpy file.");
		return Tensor<_MyValueType, _NRank, _MyDevice>::FromBuffer(Shape, (_MyValueType*)Ret.first, Ret.second, Alloc);
	}

	template <typename _MyValueType = Float32, size_t _NRank, Device _MyDevice = Device::CPU>
	Tensor<_MyValueType, _NRank, _MyDevice> FromShared(const Dimensions<_NRank>& MyShape, const std::shared_ptr<void>& Buffer, size_t BufferSize)
	{
		return Tensor<_MyValueType, _NRank, _MyDevice>::New(MyShape, Buffer, BufferSize);
	}

	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU>
	Tensor<_MyValueType, 1, _MyDevice> FromVector(TemplateLibrary::Vector<_MyValueType, _MyDevice>& Buffer)
	{
		return Tensor<_MyValueType, 1, _MyDevice>::FromBuffer(
			IDim(static_cast<SizeType>(Buffer.Size())),
			Buffer.Data(),
			Buffer.Size()
		);
	}

	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU>
	Tensor<_MyValueType, 1, _MyDevice> FromVector(TemplateLibrary::Vector<_MyValueType>&& Buffer)
	{
		auto Allocator = Buffer.GetAllocator();
		auto [Data, Size] = Buffer.Release();
		auto Shape = IDim(static_cast<SizeType>(Size));
		return Tensor<_MyValueType, 1, _MyDevice>::FromBuffer(Shape, Data, Size, Allocator);
	}

	/**
	 * @brief Create a new tensor with the specified shape.
	 * @param MyShape The shape of the tensor.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<std::is_trivially_copy_assignable_v<_MyValueType> || std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) Empty(
		const Dimensions<_NRank>& MyShape
	)
	{
		return Tensor<_MyValueType, _NRank, _MyDevice>::New(MyShape);
	}

	/**
	 * @brief Create a new tensor with the specified shape and initialize it with the specified args.
	 * @param MyShape The shape of the tensor.
	 * @param Arg0 The first value.
	 * @param Args The rest values.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename _First, typename ...Rest, typename = std::enable_if_t<std::is_constructible_v<_MyValueType, _First, Rest...>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) ConstructTensor(
		const Dimensions<_NRank>& MyShape,
		_First&& Arg0, Rest&& ...Args
	)
	{
		return Tensor<_MyValueType, _NRank, _MyDevice>::New(MyShape, std::forward<_First>(Arg0), std::forward<Rest>(Args)...);
	}

	/**
	 * @brief Create an empty new tensor.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<std::is_trivially_copy_assignable_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) EmptyTensor()
	{
		return Tensor<_MyValueType, _NRank, _MyDevice>::New();
	}

	/**
	 * @brief Create a new scalar tensor with the specified value.
	 * @param _Val The value of the tensor.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) NewScalar(
		const _MyValueType& _Val
	)
	{
		return Tensor<_MyValueType, 1, _MyDevice>::New({ 1 }, _Val);
	}

	/**
	 * @brief Create a new tensor with the specified shape, and fix the tensor with ones.
	 * @param _Shape The shape of the tensor.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>&& std::is_constructible_v<_MyValueType, decltype(1)>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) Ones(
		const Dimensions<_NRank>& _Shape
	)
	{
		return Tensor<_MyValueType, _NRank, _MyDevice>::Ones(_Shape);
	}

	/**
	 * @brief Create a new tensor with the specified shape, and fix the tensor with zeros.
	 * @param _Shape The shape of the tensor.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>&& std::is_constructible_v<_MyValueType, decltype(0)>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) Zeros(
		const Dimensions<_NRank>& _Shape
	)
	{
		return Tensor<_MyValueType, _NRank, _MyDevice>::Zeros(_Shape);
	}

	/**
	 * @brief Create a new tensor with the specified shape, and fix the tensor with a constant value.
	 * @param _Shape The shape of the tensor.
	 * @param _Val The constant value to fix the tensor.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) ConstantOf(
		const Dimensions<_NRank>& _Shape,
		const _MyValueType& _Val
	)
	{
		return Tensor<_MyValueType, _NRank, _MyDevice>::ConstantOf(_Shape, _Val);
	}

	/**
	 * @brief Create a new tensor with the specified shape, and fix the tensor with random values.
	 * @param _Shape The shape of the tensor.
	 * @param Min The minimum value of the random values.
	 * @param Max The maximum value of the random values.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<TypeTraits::IsArithmeticValue<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) Rand(
		const Dimensions<_NRank>& _Shape,
		const _MyValueType& Min,
		const _MyValueType& Max
	)
	{
		return Tensor<_MyValueType, _NRank, _MyDevice>::Rand(_Shape, Min, Max);
	}

	/**
	 * @brief Create a new tensor with the specified shape, and fix the tensor with random values.
	 * @param _Shape The shape of the tensor.
	 * @param _Mean The mean value of the random values.
	 * @param _Sigma The sigma value of the random values.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<TypeTraits::IsArithmeticValue<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) Randn(
		const Dimensions<_NRank>& _Shape,
		double _Mean = 0.,
		double _Sigma = 1.
	)
	{
		return Tensor<_MyValueType, _NRank, _MyDevice>::Randn(_Shape, _Mean, _Sigma);
	}

	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, typename = std::enable_if_t<Operators::BinaryOperators::AddBinary::HasOperatorValue<_MyValueType>&& Operators::BinaryOperators::MulBinary::HasOperatorValue<_MyValueType>&& std::is_move_assignable_v<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) Arange(
		_MyValueType _Begin,
		_MyValueType _End,
		_MyValueType _Step
	)
	{
		return Tensor<_MyValueType, 1, _MyDevice>::Arange(_Begin, _End, _Step);
	}

	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, typename = std::enable_if_t<Operators::BinaryOperators::AddBinary::HasOperatorValue<_MyValueType>&& Operators::BinaryOperators::MulBinary::HasOperatorValue<_MyValueType>&& std::is_move_assignable_v<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) Linspace(
		_MyValueType _Begin,
		_MyValueType _End,
		size_t _Count,
		bool _EndPoint = false
	)
	{
		return Tensor<_MyValueType, 1, _MyDevice>::Linspace(_Begin, _End, _Count, _EndPoint);
	}

	/**
	 * @brief Create a new tensor with the same shape as the specified tensor.
	 * @param _ShapeReference The tensor to reference the shape.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<std::is_trivially_copy_assignable_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) EmptyLike(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _ShapeReference
	)
	{
		return Empty<_MyValueType, _MyDevice, _NRank>(_ShapeReference.Shape());
	}

	/**
	 * @brief Create a new tensor with the same shape as the specified tensor, and fix the tensor with ones.
	 * @param _ShapeReference The tensor to reference the shape.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>&& std::is_constructible_v<_MyValueType, decltype(1)>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) OnesLike(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _ShapeReference
	)
	{
		return Ones<_MyValueType, _MyDevice, _NRank>(_ShapeReference.Shape());
	}

	/**
	 * @brief Create a new tensor with the same shape as the specified tensor, and fix the tensor with zeros.
	 * @param _ShapeReference The tensor to reference the shape.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>&& std::is_constructible_v<_MyValueType, decltype(0)>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) ZerosLike(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _ShapeReference
	)
	{
		return Zeros<_MyValueType, _MyDevice, _NRank>(_ShapeReference.Shape());
	}

	/**
	 * @brief Create a new tensor with the same shape as the specified tensor, and fix the tensor with a constant value.
	 * @param _ShapeReference The tensor to reference the shape.
	 * @param _Val The constant value to fix the tensor.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) ConstantLike(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _ShapeReference,
		const _MyValueType& _Val
	)
	{
		return ConstantOf<_MyValueType, _MyDevice, _NRank>(_ShapeReference.Shape(), _Val);
	}

	/**
	 * @brief Create a new tensor with the same shape as the specified tensor, and fix the tensor with random values.
	 * @param _ShapeReference The tensor to reference the shape.
	 * @param Min The minimum value of the random values.
	 * @param Max The maximum value of the random values.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<TypeTraits::IsArithmeticValue<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) RandLike(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _ShapeReference,
		const _MyValueType& Min,
		const _MyValueType& Max
	)
	{
		return Rand<_MyValueType, _MyDevice, _NRank>(_ShapeReference.Shape(), Min, Max);
	}

	/**
	 * @brief Create a new tensor with the same shape as the specified tensor, and fix the tensor with random values.
	 * @param _ShapeReference The tensor to reference the shape.
	 * @param _Mean The mean value of the random values.
	 * @param _Sigma The sigma value of the random values.
	 * @return The new tensor.
	 */
	template <typename _MyValueType = Float32, Device _MyDevice = Device::CPU, size_t _NRank, typename = std::enable_if_t<TypeTraits::IsArithmeticValue<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	_D_Dragonian_Lib_Constexpr_Force_Inline decltype(auto) RandnLike(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _ShapeReference,
		double _Mean = 0.,
		double _Sigma = 1.
	)
	{
		return Randn<_MyValueType, _MyDevice, _NRank>(_ShapeReference.Shape(), _Mean, _Sigma);
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	decltype(auto) Copy(const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor)
	{
		return _Tensor.Clone();
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	decltype(auto) Clone(const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor)
	{
		return _Tensor.Clone();
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	decltype(auto) MakeContinuous(Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor)
	{
		return _Tensor.MakeContinuous();
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice, typename = std::enable_if_t<std::is_copy_assignable_v<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	decltype(auto) Continuous(const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor)
	{
		return _Tensor.Continuous();
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice, size_t _TRank>
	decltype(auto) View(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor,
		const Dimensions<_TRank>& _ViewShape
	)
	{
		return _Tensor.View(_ViewShape);
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice, typename... _Args>
	decltype(auto) View(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor,
		_Args... _ViewShape
	)
	{
		if constexpr (sizeof...(_Args) == 0)
			return _Tensor.View();
		else
			return _Tensor.View(_ViewShape...);
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice>
	decltype(auto) UnSqueeze(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor,
		size_t _Axis
	)
	{
		return _Tensor.UnSqueeze(_Axis);
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice>
	decltype(auto) Squeeze(const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor, size_t _Axis)
	{
		return _Tensor.Squeeze(_Axis);
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice, typename = std::enable_if_t<(_NRank > 1)>>
		decltype(auto) Transpose(
			const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor,
			SizeType _Axis1 = -1, SizeType _Axis2 = -2
		)
	{
		return _Tensor.Transpose(_Axis1, _Axis2);
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice>
	decltype(auto) Permute(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor,
		const Dimensions<_NRank>& _PremuteOrder
	)
	{
		return _Tensor.Permute(_PremuteOrder);
	}

	template <typename _MyValueType, size_t _NRank, Device _MyDevice, typename... _Args, typename = std::enable_if_t<sizeof...(_Args) == _NRank>>
	decltype(auto) Permute(
		const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor,
		_Args... _PremuteOrder
	)
	{
		return _Tensor.Permute(_PremuteOrder...);
	}

	template <typename _Type, typename _MyValueType, size_t _NRank, Device _MyDevice, typename = std::enable_if_t<TypeTraits::CouldBeConvertedFromValue<_Type, _MyValueType>&& TypeTraits::CouldBeConvertedFromValue<_Type, _Type>&& std::is_copy_assignable_v<_Type>&& std::is_default_constructible_v<_Type>>>
	decltype(auto) Cast(const Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor)
	{
		return _Tensor.template Cast<_Type>();
	}

	template <size_t _TRank, typename ValueType, size_t _NRank, Device _MyDevice, typename = std::enable_if_t<_TRank <= _NRank && std::is_copy_assignable_v<ValueType>&& std::is_default_constructible_v<ValueType>>>
	decltype(auto) Padding(
		const Tensor<ValueType, _NRank, _MyDevice>& _Tensor,
		const VRanges<_TRank>& _Padding,
		PaddingType _PaddingType = PaddingType::Zero,
		std::optional<ValueType> _ConstantValue = std::nullopt
	)
	{
		return _Tensor.Padding(_Padding, _PaddingType, std::move(_ConstantValue));
	}

	template <size_t _TRank, typename ValueType, size_t _NRank, Device _MyDevice, typename = std::enable_if_t<_TRank <= _NRank && std::is_copy_assignable_v<ValueType>&& std::is_default_constructible_v<ValueType>>>
	decltype(auto) Pad(
		const Tensor<ValueType, _NRank, _MyDevice>& _Tensor,
		const VRanges<_TRank>& _Padding,
		PaddingType _PaddingType = PaddingType::Zero,
		std::optional<ValueType> _ConstantValue = std::nullopt
	)
	{
		return _Tensor.Pad(_Padding, _PaddingType, std::move(_ConstantValue));
	}

	template <typename ValueType, size_t _NRank, Device _MyDevice, typename = std::enable_if_t<std::is_copy_assignable_v<ValueType>&& std::is_default_constructible_v<ValueType>>>
	decltype(auto) Repeat(
		const Tensor<ValueType, _NRank, _MyDevice>& _Tensor,
		const IDLArray<SizeType, _NRank>& _Repeats
	)
	{
		return _Tensor.Repeat(_Repeats);
	}

	template <typename ValueType, size_t _NRank, Device _MyDevice, typename = std::enable_if_t<std::is_copy_assignable_v<ValueType>&& std::is_default_constructible_v<ValueType>>>
	decltype(auto) Repeat(
		const Tensor<ValueType, _NRank, _MyDevice>& _Tensor,
		SizeType _Axis,
		SizeType _Repeat
	)
	{
		return _Tensor.Repeat(_Axis, _Repeat);
	}

	template <typename ..._ArgTypes, typename = std::enable_if_t<FunctionalTraits::StackCatTraits<_ArgTypes...>::_Enabled>>
	decltype(auto) Stack(
		_ArgTypes&& ..._Args
	)
	{
		using _MyTraits = FunctionalTraits::StackCatTraits<_ArgTypes...>;
		using _MyTensorType = TypeTraits::RemoveARPCVType<typename _MyTraits::_MyFirstTensorArgumentType>;
		using _MyValueType = typename _MyTraits::_MyFirstTensorArgumentValueType;
		constexpr auto _MyDevice = _MyTraits::_MyFirstTensorArgumentDevice;
		constexpr auto _MyRank = SizeType(_MyTraits::_MyFirstTensorArgumentRank);
		constexpr auto _MyTensorCount = SizeType(_MyTraits::_MyTensorArgumentCount);

		auto _MyTensors = FunctionalTraits::StackCatTraits(std::forward<_ArgTypes>(_Args)...);
		auto _Dim = _MyTensorType::CalcIterator(_MyTensors._Axis, _MyRank);
		const auto& _Inputs = _MyTensors._Tensors;

		if constexpr (_MyTensorCount == 1)
			return _Inputs[0].Clone().UnSqueeze(_Dim);
		else
		{
			const auto& _0Shape = _MyTensors._Tensors[0].Shape();
			for (SizeType i = 1; i < _MyTensorCount; ++i)
			{
				const auto& CurShape = _Inputs[i].Shape();
				for (SizeType j = 0; j < _MyRank; ++j)
					if (_0Shape[j] != CurShape[j])
						_D_Dragonian_Lib_Throw_Exception("Shape MisMatch!");
			}

			const auto Shape = _0Shape.Insert(_MyTensorCount, _Dim);
			auto Ret = Empty<_MyValueType, _MyDevice, _MyRank + 1>(Shape);
			Ret.WaitingAsResult();
			SliceOptions<_MyRank + 1> _MySliceOption;
			auto& CurSlice = _MySliceOption[_Dim];
			for (SizeType i = 0; i < _MyTensorCount; ++i)
			{
				CurSlice = { i , i + 1 };
				Ret[_MySliceOption].TensorAssign(_Inputs[i].UnSqueeze(_Dim));
			}
			return Ret;
		}
	}

	template <typename ..._ArgTypes, typename = std::enable_if_t<FunctionalTraits::StackCatTraits<_ArgTypes...>::_Enabled>>
	decltype(auto) Cat(
		_ArgTypes&& ..._Args
	)
	{
		using _MyTraits = FunctionalTraits::StackCatTraits<_ArgTypes...>;
		using _MyTensorType = TypeTraits::RemoveARPCVType<typename _MyTraits::_MyFirstTensorArgumentType>;
		using _MyValueType = typename _MyTraits::_MyFirstTensorArgumentValueType;
		constexpr auto _MyDevice = _MyTraits::_MyFirstTensorArgumentDevice;
		constexpr auto _MyRank = SizeType(_MyTraits::_MyFirstTensorArgumentRank);
		constexpr auto _MyTensorCount = SizeType(_MyTraits::_MyTensorArgumentCount);

		auto _MyTensors = FunctionalTraits::StackCatTraits(std::forward<_ArgTypes>(_Args)...);
		auto _Dim = _MyTensorType::CalcIterator(_MyTensors._Axis, _MyRank);
		const auto& _Inputs = _MyTensors._Tensors;

		if constexpr (_MyTensorCount == 1)
			return _Inputs[0].Clone();
		else
		{
			const auto& FShape = _Inputs[0].Shape();
			const auto NDims = _Inputs[0].Rank();
			auto Shape = FShape;

			for (size_t i = 1; i < _MyTensorCount; ++i)
			{
				const auto& CurShape = _Inputs[i].Shape();
				for (SizeType j = 0; j < NDims; ++j)
				{
					if (j == _Dim)
						Shape[j] += CurShape[j];
					else if (FShape[j] != CurShape[j])
						_D_Dragonian_Lib_Throw_Exception("Shape MisMatch!");
				}
			}

			auto Ret = Empty<_MyValueType, _MyDevice, _MyRank>(Shape);
			Ret.WaitingAsResult();

			SliceOptions<_MyRank> _MySliceOption;
			auto& CurSlice = _MySliceOption[_Dim];
			CurSlice = { 0, 0 };
			for (SizeType i = 0; i < _MyTensorCount; ++i)
			{
				const auto& __Shape = _Inputs[i].Shape();
				CurSlice = { CurSlice.End , CurSlice.End + __Shape[_Dim] };
				Ret[_MySliceOption].TensorAssign(_Inputs[i]);
			}
			return Ret;
		}
	}

	template <typename _MyValueType, size_t _NRankA, size_t _NRankB, Device _MyDevice, typename = std::enable_if_t<Operators::BinaryOperators::MaxBinary::HasOperatorValue<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	decltype(auto) Max(const Tensor<_MyValueType, _NRankA, _MyDevice>& _TensorA, const Tensor<_MyValueType, _NRankB, _MyDevice>& _TensorB)
	{
		return FunctionalImpl::Max(_TensorA, _TensorB);
	}

	template <typename _MyValueType, size_t _NRankA, size_t _NRankB, Device _MyDevice, typename = std::enable_if_t<Operators::BinaryOperators::MinBinary::HasOperatorValue<_MyValueType>&& std::is_default_constructible_v<_MyValueType>>>
	decltype(auto) Min(const Tensor<_MyValueType, _NRankA, _MyDevice>& _TensorA, const Tensor<_MyValueType, _NRankB, _MyDevice>& _TensorB)
	{
		return FunctionalImpl::Min(_TensorA, _TensorB);
	}
}

_D_Dragonian_Lib_Space_End

template <typename _MyValueType, size_t _NRank, DragonianLib::Device _MyDevice>
std::ostream& operator<<(std::ostream& _OStream, const DragonianLib::Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor)
{
	_OStream << _Tensor.CastToString();
	return _OStream;
}

template <typename _MyValueType, size_t _NRank, DragonianLib::Device _MyDevice>
std::wostream& operator<<(std::wostream& _OStream, const DragonianLib::Tensor<_MyValueType, _NRank, _MyDevice>& _Tensor)
{
	_OStream << _Tensor.CastToWideString();
	return _OStream;
}

template <typename _MyValueType, size_t _NRank>
std::ostream& operator<<(std::ostream& _OStream, const DragonianLib::TemplateLibrary::Array<_MyValueType, _NRank>& _Array)
{
	_OStream << _Array.ToString();
	return _OStream;
}

template <typename _MyValueType, size_t _NRank>
std::wostream& operator<<(std::wostream& _OStream, const DragonianLib::TemplateLibrary::Array<_MyValueType, _NRank>& _Array)
{
	_OStream << _Array.ToWString();
	return _OStream;
}