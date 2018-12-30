#pragma once

#include <cstdio>
#include <memory>
#include <string>
#include <libhelpers/Filesystem/IStreamFS.h>

namespace Filesystem {
    enum class StreamFILEMode {
        ReadOnly,
        ReadWriteOpenExisting,
        ReadWriteCreateIfNotExist,
        ReadWriteTruncate
    };

    class StreamFILE : public IStream {
    public:
        StreamFILE(std::wstring path, StreamFILEMode mode);
        StreamFILE(const wchar_t *path, StreamFILEMode mode);
        StreamFILE(const std::string &pathUtf8, StreamFILEMode mode);
        StreamFILE(const char *pathUtf8, StreamFILEMode mode);

        bool CanRead() const override;
        bool CanWrite() const override;

        uint64_t GetSize() const override;
        uint64_t GetPosition() const override;

        uint32_t Read(void *buf, uint32_t length) override;
        uint32_t Write(const void *buf, uint32_t length) override;
        void Seek(SeekOrigin origin, int64_t offset) override;

    private:
        struct FILEDeleter {
            void operator()(FILE *file);
        };

        std::wstring path;
        StreamFILEMode mode;
        std::unique_ptr<FILE, FILEDeleter> file;

        var_ptr<IStream> CloneI(var_ptr_type ptrType) override;
    };
}