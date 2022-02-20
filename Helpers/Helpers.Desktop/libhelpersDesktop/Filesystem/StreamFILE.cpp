#include "StreamFILE.h"

#include <cassert>
#include <stdexcept>
#include <libhelpers/unique_ptr_extensions.h>
#include <libhelpers/UtfConvert.h>

namespace Filesystem {
    StreamFILE::StreamFILE(std::wstring path, StreamFILEMode mode)
        : path(std::move(path)), mode(mode)
    {
        errno_t err = 0;
        const wchar_t *mode1 = nullptr;
        const wchar_t *mode2 = nullptr;

        switch (this->mode) {
        case StreamFILEMode::ReadOnly: mode1 = L"rb"; break;
        case StreamFILEMode::ReadWriteOpenExisting: mode1 = L"r+b"; break;
        case StreamFILEMode::ReadWriteCreateIfNotExist: mode1 = L"r+b"; mode2 = L"w+b"; break;
        case StreamFILEMode::ReadWriteTruncate: mode1 = L"w+b"; break;
        default:
            throw std::runtime_error("StreamFILE: invalid mode value.");
            break;
        }

        err = _wfopen_s(GetAddressOf(this->file), this->path.c_str(), mode1);

        if (mode2 && err) {
            err = _wfopen_s(GetAddressOf(this->file), this->path.c_str(), mode2);
        }

        if (err) {
            throw std::runtime_error("StreamFILE: _wfopen_s failed.");
        }
    }

    StreamFILE::StreamFILE(const wchar_t *path, StreamFILEMode mode)
        : StreamFILE(std::wstring(path), mode)
    {}

    /*StreamFILE::StreamFILE(const std::string &pathUtf8, StreamFILEMode mode)
        : StreamFILE(ConvertToUtf16(pathUtf8), mode)
    {}

    StreamFILE::StreamFILE(const char *pathUtf8, StreamFILEMode mode)
        : StreamFILE(ConvertToUtf16(pathUtf8), mode)
    {}*/

    bool StreamFILE::CanRead() const {
        return true;
    }

    bool StreamFILE::CanWrite() const {
        bool canWrite = this->mode != StreamFILEMode::ReadOnly;
        return canWrite;
    }

    uint64_t StreamFILE::GetSize() const {
        struct _stat64 stats;

        int err = _fstati64(_fileno(this->file.get()), &stats);
        if (err) {
            throw std::runtime_error("StreamFILE::GetSize failed");
        }

        return (uint64_t)stats.st_size;
    }

    uint64_t StreamFILE::GetPosition() const {
        int64_t pos = _ftelli64(this->file.get());
        return (uint64_t)pos;
    }

    uint32_t StreamFILE::Read(void *buf, uint32_t length) {
        size_t read = fread(buf, 1, length, this->file.get());
        return (uint32_t)read;
    }

    uint32_t StreamFILE::Write(const void *buf, uint32_t length) {
        size_t written = fwrite(buf, 1, length, this->file.get());
        return (uint32_t)written;
    }

    void StreamFILE::Seek(SeekOrigin origin, int64_t offset) {
        int originTmp;

        switch (origin) {
        case Filesystem::SeekOrigin::Begin:
            originTmp = SEEK_SET;
            break;
        case Filesystem::SeekOrigin::Current:
            originTmp = SEEK_CUR;
            break;
        case Filesystem::SeekOrigin::End:
            originTmp = SEEK_END;
            break;
        default:
            throw std::runtime_error("StreamFILE::Seek invalid origin");
            break;
        }

        int err = _fseeki64(this->file.get(), offset, originTmp);
        if (err) {
            throw std::runtime_error("StreamFILE::Seek _fseeki64 failed");
        }
    }

    var_ptr<IStream> StreamFILE::CloneI(var_ptr_type ptrType) {
        auto clone = make_var_ptr<StreamFILE>(ptrType, this->path, this->mode);
        return clone;
    }




    void StreamFILE::FILEDeleter::operator()(FILE *file) {
        fclose(file);
    }
}