#pragma once
#include "../Api.hpp"
#include "../../../emu-core/src/debug/Debug.hpp"
#include "../../../sdk/hex.hpp"
#include <string>
#include <vector>

// BCryptOpenAlgorithmProvider(BCRYPT_ALG_HANDLE* phAlgorithm, LPCWSTR pszAlgId, LPCWSTR pszImplementation, ULONG dwFlags) -> NTSTATUS
class ApiBCryptOpenAlgorithmProvider : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t ph_alg = get_arg(cpu, 0);
        uint64_t alg_id = get_arg(cpu, 1);
        uint64_t impl   = get_arg(cpu, 2);
        uint32_t flags  = static_cast<uint32_t>(get_arg(cpu, 3));

        std::wstring alg_w = read_wide_string(cpu, alg_id);
        std::string alg_a(alg_w.begin(), alg_w.end());

        static uint64_t s_alg_handle = 0x700;
        uint64_t handle = s_alg_handle++;
        if (ph_alg != 0) {
            write_u64(cpu, ph_alg, handle);
        }

        Debug::debug_msg("[BCryptOpenAlgorithmProvider] alg=\"" + alg_a + "\" -> handle=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// BCryptCloseAlgorithmProvider(BCRYPT_ALG_HANDLE hAlgorithm, ULONG dwFlags) -> NTSTATUS
class ApiBCryptCloseAlgorithmProvider : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t h_alg = get_arg(cpu, 0);
        uint32_t flags = static_cast<uint32_t>(get_arg(cpu, 1));
        Debug::debug_msg("[BCryptCloseAlgorithmProvider] handle=" + hex64(h_alg), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// BCryptCreateHash(BCRYPT_ALG_HANDLE hAlgorithm, BCRYPT_HASH_HANDLE* phHash, PUCHAR pbHashObject, ULONG cbHashObject, PUCHAR pbSecret, ULONG cbSecret, ULONG dwFlags) -> NTSTATUS
class ApiBCryptCreateHash : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t h_alg   = get_arg(cpu, 0);
        uint64_t ph_hash = get_arg(cpu, 1);

        static uint64_t s_hash_handle = 0x704;
        uint64_t handle = s_hash_handle++;
        if (ph_hash != 0) {
            write_u64(cpu, ph_hash, handle);
        }

        Debug::debug_msg("[BCryptCreateHash] alg=" + hex64(h_alg) + " -> hash=" + hex64(handle), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// BCryptHashData(BCRYPT_HASH_HANDLE hHash, PUCHAR pbInput, ULONG cbInput, ULONG dwFlags) -> NTSTATUS
class ApiBCryptHashData : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t h_hash = get_arg(cpu, 0);
        uint64_t pb_in  = get_arg(cpu, 1);
        uint32_t cb_in  = static_cast<uint32_t>(get_arg(cpu, 2));

        Debug::debug_msg("[BCryptHashData] hash=" + hex64(h_hash) + " cbInput=" + std::to_string(cb_in), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// BCryptFinishHash(BCRYPT_HASH_HANDLE hHash, PUCHAR pbOutput, ULONG cbOutput, ULONG dwFlags) -> NTSTATUS
class ApiBCryptFinishHash : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t h_hash = get_arg(cpu, 0);
        uint64_t pb_out = get_arg(cpu, 1);
        uint32_t cb_out = static_cast<uint32_t>(get_arg(cpu, 2));

        if (pb_out != 0 && cb_out > 0) {
            std::vector<uint8_t> dummy_hash(cb_out, 0xAA);
            cpu->mem_write(pb_out, dummy_hash.data(), cb_out);
        }

        Debug::debug_msg("[BCryptFinishHash] hash=" + hex64(h_hash) + " cbOutput=" + std::to_string(cb_out), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};

// BCryptDestroyHash(BCRYPT_HASH_HANDLE hHash) -> NTSTATUS
class ApiBCryptDestroyHash : public Api {
public:
    uint64_t call(CPU* cpu) override {
        uint64_t h_hash = get_arg(cpu, 0);
        Debug::debug_msg("[BCryptDestroyHash] hash=" + hex64(h_hash), LOG_INFO);
        return 0; // STATUS_SUCCESS
    }
};
