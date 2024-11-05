#pragma once

#include <QString>
#include "base/at_exit.h"
#include "base/basictypes.h"
#include "base/command_line.h"
#include "base/file_util.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "base/utf_string_conversions.h"
#include "courgette/courgette.h"
#include "courgette/streams.h"
#include "courgette/third_party/bsdiff.h"

class CourgetteImpl
{
public:
    CourgetteImpl(void);
    ~CourgetteImpl(void);

    // 生成补丁文件
    bool MakePatchForFile(QString const& oldFile, QString const& newFile, QString const& patchFile);

    // 通过补丁生成新文件
    bool MakeFileToPatch(QString const& oldFile, QString const& patchFile, QString const& newFile);

    // 二进制
    // 生成补丁文件
    bool MakeBSDPatchForFile(QString const& oldFile, QString const& newFile, QString const& patchFile);

    // 通过补丁生成新文件
    bool MakeBSDFileToPatch(QString const& oldFile, QString const& patchFile, QString const& newFile);

private:
    bool GenerateEnsemblePatch(const base::FilePath& old_file,
        const base::FilePath& new_file,
        const base::FilePath& patch_file);

    bool ApplyEnsemblePatch(const base::FilePath& old_file,
        const base::FilePath& patch_file,
        const base::FilePath& new_file);

    bool GenerateBSDiffPatch(const base::FilePath& old_file,
        const base::FilePath& new_file,
        const base::FilePath& patch_file);

    bool ApplyBSDiffPatch(const base::FilePath& old_file,
        const base::FilePath& patch_file,
        const base::FilePath& new_file);

    bool ReadOrFail(const base::FilePath& file_name, std::string& buf);

    bool CourgetteImpl::WriteSinkToFile(const courgette::SinkStream *sink, const base::FilePath& output_file);

private:
    QString m_strErrorMsg;
};