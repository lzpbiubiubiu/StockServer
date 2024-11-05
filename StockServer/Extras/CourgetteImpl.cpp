#include "CourgetteImpl.h"
#include <QDir>

CourgetteImpl::CourgetteImpl(void)
{
}

CourgetteImpl::~CourgetteImpl(void)
{
}

bool CourgetteImpl::MakePatchForFile(QString const& oldFile, QString const& newFile, QString const& patchFile)
{
    base::FilePath oldF(oldFile.toStdWString());
    base::FilePath newF(newFile.toStdWString());
    base::FilePath patchF(patchFile.toStdWString());
    return GenerateEnsemblePatch(oldF, newF, patchF);
}

bool CourgetteImpl::MakeFileToPatch(QString const& oldFile, QString const& patchFile, QString const& newFile)
{
    base::FilePath oldF(oldFile.toStdWString());
    base::FilePath newF(newFile.toStdWString());
    base::FilePath patchF(patchFile.toStdWString());
    return ApplyEnsemblePatch(oldF, patchF, newF);
}

//二进制
bool CourgetteImpl::MakeBSDPatchForFile(QString const& oldFile, QString const& newFile, QString const& patchFile)
{
    base::FilePath oldF(oldFile.toStdWString());
    base::FilePath newF(newFile.toStdWString());
    base::FilePath patchF(patchFile.toStdWString());
    return GenerateBSDiffPatch(oldF, newF, patchF);
}

bool CourgetteImpl::MakeBSDFileToPatch(QString const& oldFile, QString const& patchFile, QString const& newFile)
{
    base::FilePath oldF(QDir::toNativeSeparators(oldFile).toStdWString());
    base::FilePath newF(QDir::toNativeSeparators(newFile).toStdWString());
    base::FilePath patchF(QDir::toNativeSeparators(patchFile).toStdWString());
    return ApplyBSDiffPatch(oldF, patchF, newF);
}

bool CourgetteImpl::GenerateEnsemblePatch(const base::FilePath& old_file,
    const base::FilePath& new_file,
    const base::FilePath& patch_file)
{
    std::string old_buffer;
    bool isok = ReadOrFail(old_file, old_buffer);
    if(!isok)
        return isok;

    std::string new_buffer;
    isok = ReadOrFail(new_file, new_buffer);
    if(!isok)
        return isok;

    courgette::SourceStream old_stream;
    courgette::SourceStream new_stream;
    old_stream.Init(old_buffer);
    new_stream.Init(new_buffer);

    courgette::SinkStream patch_stream;
    courgette::Status status =
        courgette::GenerateEnsemblePatch(&old_stream, &new_stream, &patch_stream);

    if (status != courgette::C_OK) 
        return false;

    return WriteSinkToFile(&patch_stream, patch_file);
}

bool CourgetteImpl::ApplyEnsemblePatch(const base::FilePath& old_file,
    const base::FilePath& patch_file,
    const base::FilePath& new_file)
{
    // We do things a little differently here in order to call the same Courgette
    // entry point as the installer.  That entry point point takes file names and
    // returns an status code but does not output any diagnostics.

    courgette::Status status =
        courgette::ApplyEnsemblePatch(old_file.value().c_str(),
        patch_file.value().c_str(),
        new_file.value().c_str());

    if (status == courgette::C_OK)
        return true;

    // Diagnose the error.
    switch (status) {
    case courgette::C_BAD_ENSEMBLE_MAGIC:
        m_strErrorMsg = "Not a courgette patch";
        return false;

    case courgette::C_BAD_ENSEMBLE_VERSION:
        m_strErrorMsg = "Wrong version patch";
        return false;

    case courgette::C_BAD_ENSEMBLE_HEADER:
        m_strErrorMsg = "Corrupt patch";
        return false;

    case courgette::C_DISASSEMBLY_FAILED:
        m_strErrorMsg = "Disassembly failed (could be because of memory issues)";
        return false;

    case courgette::C_STREAM_ERROR:
        m_strErrorMsg = "Stream error (likely out of memory or disk space)";
        return false;

    default:
        break;
    }

    //  If we failed due to a missing input file, this will
    // print the message.
    std::string old_buffer;
    bool isok = ReadOrFail(old_file, old_buffer);
    old_buffer.clear();
    std::string patch_buffer;
    isok = ReadOrFail(patch_file, patch_buffer);
    patch_buffer.clear();

    // Non-input related errors:
    if (status == courgette::C_WRITE_OPEN_ERROR)
        m_strErrorMsg = "Can't open output";
    if (status == courgette::C_WRITE_ERROR)
        m_strErrorMsg = "Can't write output";

    return false;
}

bool CourgetteImpl::GenerateBSDiffPatch(const base::FilePath& old_file,
                         const base::FilePath& new_file,
                         const base::FilePath& patch_file)
{
    std::string old_buffer;
    bool isok = ReadOrFail(old_file, old_buffer);
    if(!isok)
        return isok;

    std::string new_buffer;
    isok = ReadOrFail(new_file, new_buffer);
    if(!isok)
        return isok;

    courgette::SourceStream old_stream;
    courgette::SourceStream new_stream;
    old_stream.Init(old_buffer);
    new_stream.Init(new_buffer);

    courgette::SinkStream patch_stream;
    courgette::BSDiffStatus status =
        courgette::CreateBinaryPatch(&old_stream, &new_stream, &patch_stream);

    if (status != courgette::OK) 
        return false;

    return WriteSinkToFile(&patch_stream, patch_file);
}

bool CourgetteImpl::ApplyBSDiffPatch(const base::FilePath& old_file,
                      const base::FilePath& patch_file,
                      const base::FilePath& new_file)
{
    std::string old_buffer;
    bool isok = ReadOrFail(old_file, old_buffer);
    if(!isok)
        return false;

    std::string patch_buffer;
    isok= ReadOrFail(patch_file, patch_buffer);
    if(!isok)
        return false;

    courgette::SourceStream old_stream;
    courgette::SourceStream patch_stream;
    old_stream.Init(old_buffer);
    patch_stream.Init(patch_buffer);

    courgette::SinkStream new_stream;
    courgette::BSDiffStatus status =
        courgette::ApplyBinaryPatch(&old_stream, &patch_stream, &new_stream);

    if (status != courgette::OK) 
        return false;

    return WriteSinkToFile(&new_stream, new_file);
}

bool CourgetteImpl::ReadOrFail(const base::FilePath& file_name, std::string& buffer)
{
    int64 file_size = 0;
    if (!file_util::GetFileSize(file_name, &file_size))
        return false;

    buffer.reserve(static_cast<size_t>(file_size));

    if (!file_util::ReadFileToString(file_name, &buffer))
        return false;

    return true;
}

bool CourgetteImpl::WriteSinkToFile(const courgette::SinkStream *sink, const base::FilePath& output_file)
{
    int count =
        file_util::WriteFile(output_file,
        reinterpret_cast<const char*>(sink->Buffer()),
        static_cast<int>(sink->Length()));
    if (count == -1)
        return false;

    if (static_cast<size_t>(count) != sink->Length())
        return false;

    return true;
}