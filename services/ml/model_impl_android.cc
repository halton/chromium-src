// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/ml/model_impl_android.h"
#include "services/ml/compilation_impl_android.h"
#include "base/strings/string_number_conversions.h"

namespace ml {

ModelImplAndroid::ModelImplAndroid() {
  int32_t result = ANeuralNetworksModel_create(&nn_model_);
  DLOG(INFO) << "ANeuralNetworksModel_create: " << result;
}

ModelImplAndroid::~ModelImplAndroid() {
  ANeuralNetworksModel_free(nn_model_);
  DLOG(INFO) << "ANeuralNetworksModel_free";
}

int32_t ModelImplAndroid::AddOperand(int32_t type, const std::vector<uint32_t>& dimensions, float scale, int32_t zeroPoint) {
  // Logging
  DLOG(INFO) << "  ModelImplAndroid::AddOperand";
  DLOG(INFO) << "    " << "type: " << type;
  DLOG(INFO) << "    " << "dimensions(" << dimensions.size() << "): " << VectorToString(dimensions.data(), dimensions.size());
  DLOG(INFO) << "    " << "scale: " << scale;
  DLOG(INFO) << "    " << "zeroPoint: " << zeroPoint;
  Operand operand;
  operand.type = type;
  operand.dimensions = dimensions;
  operand.scale = scale;
  operand.zeroPoint = zeroPoint;
  operands_.push_back(operand);

  // Implementation
  ANeuralNetworksOperandType operand_type;
  // TODO: convert from blink operand type to NN API type.
  operand_type.type = type;
  operand_type.dimensionCount = dimensions.size();
  operand_type.dimensions = dimensions.data();
  operand_type.scale = scale;
  operand_type.zeroPoint = zeroPoint;
  int32_t result = ANeuralNetworksModel_addOperand(nn_model_, &operand_type);
  DLOG(INFO) << "ANeuralNetworksModel_addOperand: " << result;

  return result;
}

int32_t ModelImplAndroid::SetOperandValue(uint32_t index, const void* buffer, uint32_t length) {
  // Logging
  DLOG(INFO) << "  ModelImplAndroid::SetOperandValue";
  DLOG(INFO) << "    " << "index: " << index;
  DLOG(INFO) << "    " << "length: " << length;
  if (index > operands_.size()) {
    return mojom::BAD_DATA;
  }
  auto operand = operands_[index];
  if (operand.type == mojom::TENSOR_FLOAT32 || operand.type == mojom::FLOAT32) {
    const float* value = static_cast<const float*>(buffer);
    uint32_t size = length / 4;
    DLOG(INFO) << "    " << "buffer(" << size << "): " << VectorToString(value, size);
  } else if (operand.type == mojom::TENSOR_INT32 || operand.type == mojom::INT32) {
    const int32_t* value = static_cast<const int32_t*>(buffer);
    uint32_t size = length / 4;
    DLOG(INFO) << "    " << "buffer(" << size << "): " << VectorToString(value, size);
  } else if (operand.type == mojom::TENSOR_QUANT8_ASYMM) {
    const int8_t* value = static_cast<const int8_t*>(buffer);
    uint32_t size = length;
    DLOG(INFO) << "    " << "buffer(" << size << "): " << VectorToString(value, size);
  } else if (operand.type == mojom::UINT32) {
    const uint32_t* value = static_cast<const uint32_t*>(buffer);
    uint32_t size = length;
    DLOG(INFO) << "    " << "buffer(" << size << "): " << VectorToString(value, size);
  }

  // Implementation
  // TODO: optimize the memory copies.
  int32_t result = ANeuralNetworksModel_setOperandValue(
      nn_model_, index, buffer, length);
  DLOG(INFO) << "ANeuralNetworksModel_setOperandValue: " << result;

  return result;
}

int32_t ModelImplAndroid::AddOperation(int32_t type, const std::vector<uint32_t>& inputs, const std::vector<uint32_t>& outputs) {
  // Logging  
  DLOG(INFO) << "  ModelImplAndroid::AddOperation";
  DLOG(INFO) << "    " << "type: " << type;
  DLOG(INFO) << "    " << "inputs(" << inputs.size() << "): " << VectorToString(inputs.data(), inputs.size());
  DLOG(INFO) << "    " << "outputs(" << outputs.size() << "): " << VectorToString(outputs.data(), outputs.size());
  Operation operation;
  operation.type = type;
  operation.inputs = inputs;
  operation.outputs = outputs;
  operations_.push_back(operation);

  // Implementation
  // TODO: convert blink operation type to NN API type.
  int32_t result = ANeuralNetworksModel_addOperation(
    nn_model_, type, inputs.size(), inputs.data(), outputs.size(), outputs.data());
  DLOG(INFO) << "ANeuralNetworksModel_addOperation: " << result;

  return result;
}

int32_t ModelImplAndroid::IdentifyInputsAndOutputs(const std::vector<uint32_t>& inputs, const std::vector<uint32_t>& outputs) {
  DLOG(INFO) << "  ModelImplAndroid::IdentifyInputsAndOutputs";
  DLOG(INFO) << "    " << "inputs(" << inputs.size() << "): " << VectorToString(inputs.data(), inputs.size());
  DLOG(INFO) << "    " << "outputs(" << outputs.size() << "): " << VectorToString(outputs.data(), outputs.size());
  inputs_ = inputs;
  outputs_ = outputs;

  int32_t result = ANeuralNetworksModel_identifyInputsAndOutputs(
      nn_model_, inputs.size(), inputs.data(), outputs.size(), outputs.data());
  DLOG(INFO) << "ANeuralNetworksModel_identifyInputsAndOutputs: " << result;

  return result;
}

void ModelImplAndroid::finish(mojom::ModelInfoPtr model_info, finishCallback callback) {
  DLOG(INFO) << "ModelImplAndroid::finish";
  DLOG(INFO) << "operands(" << model_info->operands.size() << ")";
  int32_t result;
  for (size_t i = 0; i < model_info->operands.size(); ++i ) {
    DLOG(INFO) << "  operand[" << i << "]";
    const mojom::OperandPtr& operand = model_info->operands[i];
    result = AddOperand(operand->type, operand->dimensions, operand->scale, operand->zeroPoint);
    if (result != ANEURALNETWORKS_NO_ERROR) {
      std::move(callback).Run(result);
      return;
    }
  }
  DLOG(INFO) << "operations(" << model_info->operations.size() << ")";
  for (size_t i = 0; i < model_info->operations.size(); ++i ) {
    DLOG(INFO) << "  operation[" << i << "]";
    const mojom::OperationPtr& operation = model_info->operations[i];
    result = AddOperation(operation->type, operation->inputs, operation->outputs);
    if (result != ANEURALNETWORKS_NO_ERROR) {
      std::move(callback).Run(result);
      return;
    }
  }
  DLOG(INFO) << "values(" << model_info->values.size() << ")";
  auto mapping = model_info->memory->Map(model_info->memory_size);
  const int8_t* base = static_cast<const int8_t*>(mapping.get());
  for (size_t i = 0; i < model_info->values.size(); ++i ) {
    DLOG(INFO) << "  values[" << i << "]";
    const mojom::OperandValueInfoPtr& value_info = model_info->values[i];
    SetOperandValue(value_info->index,
                    static_cast<const void*>(base + value_info->offset),
                    value_info->length);
  }
  DLOG(INFO) << "inputs(" << model_info->inputs.size() << ")";
  DLOG(INFO) << "outputs(" << model_info->outputs.size() << ")";
  result = IdentifyInputsAndOutputs(model_info->inputs, model_info->outputs);
  if (result != ANEURALNETWORKS_NO_ERROR) {
    std::move(callback).Run(result);
    return;
  }

  result = ANeuralNetworksModel_finish(nn_model_);
  DLOG(INFO) << "ANeuralNetworksModel_finish: " << result;

  std::move(callback).Run(result);
}

void ModelImplAndroid::createCompilation(createCompilationCallback callback) {
  DLOG(INFO) << "ModelImplAndroid::createCompilation";
  auto init_params = mojom::CompilationInitParams::New();

  auto impl = std::make_unique<CompilationImplAndroid>(this);

  mojom::CompilationPtrInfo ptr_info;
  mojo::MakeStrongBinding(std::move(impl),
                          mojo::MakeRequest(&ptr_info));
  init_params->compilation = std::move(ptr_info);
  
  std::move(callback).Run(mojom::NO_ERROR,
                          std::move(init_params));
}

}  // namespace ml
