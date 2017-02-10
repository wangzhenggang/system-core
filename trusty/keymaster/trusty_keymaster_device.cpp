/*
 * Copyright 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "TrustyKeymaster"

#include <assert.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <type_traits>

#include <hardware/keymaster2.h>
#include <keymaster/authorization_set.h>
#include <log/log.h>

#include "keymaster_ipc.h"
#include "trusty_keymaster_device.h"
#include "trusty_keymaster_ipc.h"

const uint32_t RECV_BUF_SIZE = PAGE_SIZE;
const uint32_t SEND_BUF_SIZE = (PAGE_SIZE - sizeof(struct keymaster_message) - 16 /* tipc header */);

namespace keymaster {

static keymaster_error_t translate_error(int err) {
    switch (err) {
        case 0:
            return KM_ERROR_OK;
        case -EPERM:
        case -EACCES:
            return KM_ERROR_SECURE_HW_ACCESS_DENIED;

        case -ECANCELED:
            return KM_ERROR_OPERATION_CANCELLED;

        case -ENODEV:
            return KM_ERROR_UNIMPLEMENTED;

        case -ENOMEM:
            return KM_ERROR_MEMORY_ALLOCATION_FAILED;

        case -EBUSY:
            return KM_ERROR_SECURE_HW_BUSY;

        case -EIO:
            return KM_ERROR_SECURE_HW_COMMUNICATION_FAILED;

        case -EOVERFLOW:
            return KM_ERROR_INVALID_INPUT_LENGTH;

        default:
            return KM_ERROR_UNKNOWN_ERROR;
    }
}

TrustyKeymasterDevice::TrustyKeymasterDevice(const hw_module_t* module) {
    static_assert(std::is_standard_layout<TrustyKeymasterDevice>::value,
                  "TrustyKeymasterDevice must be standard layout");
    static_assert(offsetof(TrustyKeymasterDevice, device_) == 0,
                  "device_ must be the first member of TrustyKeymasterDevice");
    static_assert(offsetof(TrustyKeymasterDevice, device_.common) == 0,
                  "common must be the first member of keymaster2_device");

    ALOGI("Creating device");
    ALOGD("Device address: %p", this);

    device_ = {};

    device_.common.tag = HARDWARE_DEVICE_TAG;
    device_.common.version = 1;
    device_.common.module = const_cast<hw_module_t*>(module);
    device_.common.close = close_device;

    device_.flags = KEYMASTER_SUPPORTS_EC;

    device_.configure = configure;
    device_.add_rng_entropy = add_rng_entropy;
    device_.generate_key = generate_key;
    device_.get_key_characteristics = get_key_characteristics;
    device_.import_key = import_key;
    device_.export_key = export_key;
    device_.attest_key = attest_key;
    device_.upgrade_key = upgrade_key;
    device_.delete_key = nullptr;
    device_.delete_all_keys = nullptr;
    device_.begin = begin;
    device_.update = update;
    device_.finish = finish;
    device_.abort = abort;

    int rc = trusty_keymaster_connect();
    error_ = translate_error(rc);
    if (rc < 0) {
        ALOGE("failed to connect to keymaster (%d)", rc);
        return;
    }

    GetVersionRequest version_request;
    GetVersionResponse version_response;
    error_ = Send(KM_GET_VERSION, version_request, &version_response);
    if (error_ == KM_ERROR_INVALID_ARGUMENT || error_ == KM_ERROR_UNIMPLEMENTED) {
        ALOGE("\"Bad parameters\" error on GetVersion call.  Version 0 is not supported.");
        error_ = KM_ERROR_VERSION_MISMATCH;
        return;
    }
    message_version_ = MessageVersion(version_response.major_ver, version_response.minor_ver,
                                      version_response.subminor_ver);
    if (message_version_ < 0) {
        // Can't translate version?  Keymaster implementation must be newer.
        ALOGE("Keymaster version %d.%d.%d not supported.", version_response.major_ver,
              version_response.minor_ver, version_response.subminor_ver);
        error_ = KM_ERROR_VERSION_MISMATCH;
    }
}

TrustyKeymasterDevice::~TrustyKeymasterDevice() {
    trusty_keymaster_disconnect();
}

keymaster_error_t TrustyKeymasterDevice::configure(const keymaster_key_param_set_t* params) {
    ALOGD("Device received configure\n");

    if (error_ != KM_ERROR_OK) {
        return error_;
    }
    if (!params) {
        return KM_ERROR_UNEXPECTED_NULL_POINTER;
    }

    AuthorizationSet params_copy(*params);
    ConfigureRequest request;
    if (!params_copy.GetTagValue(TAG_OS_VERSION, &request.os_version) ||
        !params_copy.GetTagValue(TAG_OS_PATCHLEVEL, &request.os_patchlevel)) {
        ALOGD("Configuration parameters must contain OS version and patch level");
        return KM_ERROR_INVALID_ARGUMENT;
    }

    ConfigureResponse response;
    keymaster_error_t err = Send(KM_CONFIGURE, request, &response);
    if (err != KM_ERROR_OK) {
        return err;
    }

    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::add_rng_entropy(const uint8_t* data, size_t data_length) {
    ALOGD("Device received add_rng_entropy");
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::generate_key(
    const keymaster_key_param_set_t* params, keymaster_key_blob_t* key_blob,
    keymaster_key_characteristics_t* characteristics) {
    ALOGD("Device received generate_key");
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::get_key_characteristics(
    const keymaster_key_blob_t* key_blob, const keymaster_blob_t* client_id,
    const keymaster_blob_t* app_data, keymaster_key_characteristics_t* characteristics) {
    ALOGD("Device received get_key_characteristics");
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::import_key(
    const keymaster_key_param_set_t* params, keymaster_key_format_t key_format,
    const keymaster_blob_t* key_data, keymaster_key_blob_t* key_blob,
    keymaster_key_characteristics_t* characteristics) {
    ALOGD("Device received import_key");
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::export_key(keymaster_key_format_t export_format,
                                                    const keymaster_key_blob_t* key_to_export,
                                                    const keymaster_blob_t* client_id,
                                                    const keymaster_blob_t* app_data,
                                                    keymaster_blob_t* export_data) {
    ALOGD("Device received export_key");
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::attest_key(const keymaster_key_blob_t* key_to_attest,
                                                    const keymaster_key_param_set_t* attest_params,
                                                    keymaster_cert_chain_t* cert_chain) {
    ALOGD("Device received attest_key");
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::upgrade_key(const keymaster_key_blob_t* key_to_upgrade,
                                                     const keymaster_key_param_set_t* upgrade_params,
                                                     keymaster_key_blob_t* upgraded_key) {
    ALOGD("Device received upgrade_key");
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::begin(keymaster_purpose_t purpose,
                                               const keymaster_key_blob_t* key,
                                               const keymaster_key_param_set_t* in_params,
                                               keymaster_key_param_set_t* out_params,
                                               keymaster_operation_handle_t* operation_handle) {
    ALOGD("Device received begin");
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::update(keymaster_operation_handle_t operation_handle,
                                                const keymaster_key_param_set_t* in_params,
                                                const keymaster_blob_t* input,
                                                size_t* input_consumed,
                                                keymaster_key_param_set_t* out_params,
                                                keymaster_blob_t* output) {
    ALOGD("Device received update");
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::finish(keymaster_operation_handle_t operation_handle,
                                                const keymaster_key_param_set_t* in_params,
                                                const keymaster_blob_t* input,
                                                const keymaster_blob_t* signature,
                                                keymaster_key_param_set_t* out_params,
                                                keymaster_blob_t* output) {
    ALOGD("Device received finish");
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterDevice::abort(keymaster_operation_handle_t operation_handle) {
    ALOGD("Device received abort");
    return KM_ERROR_OK;
}

hw_device_t* TrustyKeymasterDevice::hw_device() {
    return &device_.common;
}

static inline TrustyKeymasterDevice* convert_device(const keymaster2_device_t* dev) {
    return reinterpret_cast<TrustyKeymasterDevice*>(const_cast<keymaster2_device_t*>(dev));
}

/* static */
int TrustyKeymasterDevice::close_device(hw_device_t* dev) {
    delete reinterpret_cast<TrustyKeymasterDevice*>(dev);
    return 0;
}

/* static */
keymaster_error_t TrustyKeymasterDevice::configure(const keymaster2_device_t* dev,
                                                   const keymaster_key_param_set_t* params) {
    return convert_device(dev)->configure(params);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::add_rng_entropy(const keymaster2_device_t* dev,
                                                         const uint8_t* data, size_t data_length) {
    return convert_device(dev)->add_rng_entropy(data, data_length);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::generate_key(
    const keymaster2_device_t* dev, const keymaster_key_param_set_t* params,
    keymaster_key_blob_t* key_blob, keymaster_key_characteristics_t* characteristics) {
    return convert_device(dev)->generate_key(params, key_blob, characteristics);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::get_key_characteristics(
    const keymaster2_device_t* dev, const keymaster_key_blob_t* key_blob,
    const keymaster_blob_t* client_id, const keymaster_blob_t* app_data,
    keymaster_key_characteristics_t* characteristics) {
    return convert_device(dev)->get_key_characteristics(key_blob, client_id, app_data,
                                                        characteristics);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::import_key(
    const keymaster2_device_t* dev, const keymaster_key_param_set_t* params,
    keymaster_key_format_t key_format, const keymaster_blob_t* key_data,
    keymaster_key_blob_t* key_blob, keymaster_key_characteristics_t* characteristics) {
    return convert_device(dev)->import_key(params, key_format, key_data, key_blob, characteristics);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::export_key(const keymaster2_device_t* dev,
                                                    keymaster_key_format_t export_format,
                                                    const keymaster_key_blob_t* key_to_export,
                                                    const keymaster_blob_t* client_id,
                                                    const keymaster_blob_t* app_data,
                                                    keymaster_blob_t* export_data) {
    return convert_device(dev)->export_key(export_format, key_to_export, client_id, app_data,
                                           export_data);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::attest_key(const keymaster2_device_t* dev,
                                                    const keymaster_key_blob_t* key_to_attest,
                                                    const keymaster_key_param_set_t* attest_params,
                                                    keymaster_cert_chain_t* cert_chain) {
    return convert_device(dev)->attest_key(key_to_attest, attest_params, cert_chain);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::upgrade_key(const keymaster2_device_t* dev,
                                                     const keymaster_key_blob_t* key_to_upgrade,
                                                     const keymaster_key_param_set_t* upgrade_params,
                                                     keymaster_key_blob_t* upgraded_key) {
    return convert_device(dev)->upgrade_key(key_to_upgrade, upgrade_params, upgraded_key);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::begin(const keymaster2_device_t* dev,
                                               keymaster_purpose_t purpose,
                                               const keymaster_key_blob_t* key,
                                               const keymaster_key_param_set_t* in_params,
                                               keymaster_key_param_set_t* out_params,
                                               keymaster_operation_handle_t* operation_handle) {
    return convert_device(dev)->begin(purpose, key, in_params, out_params, operation_handle);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::update(
    const keymaster2_device_t* dev, keymaster_operation_handle_t operation_handle,
    const keymaster_key_param_set_t* in_params, const keymaster_blob_t* input,
    size_t* input_consumed, keymaster_key_param_set_t* out_params, keymaster_blob_t* output) {
    return convert_device(dev)->update(operation_handle, in_params, input, input_consumed,
                                       out_params, output);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::finish(const keymaster2_device_t* dev,
                                                keymaster_operation_handle_t operation_handle,
                                                const keymaster_key_param_set_t* in_params,
                                                const keymaster_blob_t* input,
                                                const keymaster_blob_t* signature,
                                                keymaster_key_param_set_t* out_params,
                                                keymaster_blob_t* output) {
    return convert_device(dev)->finish(operation_handle, in_params, input, signature, out_params,
                                       output);
}

/* static */
keymaster_error_t TrustyKeymasterDevice::abort(const keymaster2_device_t* dev,
                                               keymaster_operation_handle_t operation_handle) {
    return convert_device(dev)->abort(operation_handle);
}

keymaster_error_t TrustyKeymasterDevice::Send(uint32_t command, const Serializable& req,
                                              KeymasterResponse* rsp) {
    uint32_t req_size = req.SerializedSize();
    if (req_size > SEND_BUF_SIZE) {
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    uint8_t send_buf[SEND_BUF_SIZE];
    Eraser send_buf_eraser(send_buf, SEND_BUF_SIZE);
    req.Serialize(send_buf, send_buf + req_size);

    // Send it
    uint8_t recv_buf[RECV_BUF_SIZE];
    Eraser recv_buf_eraser(recv_buf, RECV_BUF_SIZE);
    uint32_t rsp_size = RECV_BUF_SIZE;
    ALOGV("Sending %d byte request\n", (int)req.SerializedSize());
    int rc = trusty_keymaster_call(command, send_buf, req_size, recv_buf, &rsp_size);
    if (rc < 0) {
        ALOGE("tipc error: %d\n", rc);
        // TODO(swillden): Distinguish permanent from transient errors and set error_ appropriately.
        return translate_error(rc);
    } else {
        ALOGV("Received %d byte response\n", rsp_size);
    }

    const keymaster_message* msg = (keymaster_message*)recv_buf;
    const uint8_t* p = msg->payload;
    if (!rsp->Deserialize(&p, p + rsp_size)) {
        ALOGE("Error deserializing response of size %d\n", (int)rsp_size);
        return KM_ERROR_UNKNOWN_ERROR;
    } else if (rsp->error != KM_ERROR_OK) {
        ALOGE("Response of size %d contained error code %d\n", (int)rsp_size, (int)rsp->error);
        return rsp->error;
    }
    return rsp->error;
}

}  // namespace keymaster
