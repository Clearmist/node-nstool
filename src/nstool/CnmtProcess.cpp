#include "CnmtProcess.h"
#include "Output.hpp"

#include <pietendo/hac/ContentMetaUtil.h>

nstool::CnmtProcess::CnmtProcess() :
	mModuleName("nstool::CnmtProcess"),
	mFile(),
	mCliOutputMode(true, false, false, false, false),
	mVerify(false)
{
}

void nstool::CnmtProcess::process()
{
    handleLog("Processing CNMT file.");

	importCnmt();

	if (mCliOutputMode.show_basic_info) {
        displayCnmt();
    }
}

void nstool::CnmtProcess::setInputFile(const std::shared_ptr<tc::io::IStream>& file)
{
	mFile = file;
}

void nstool::CnmtProcess::setCliOutputMode(CliOutputMode type)
{
	mCliOutputMode = type;
}

void nstool::CnmtProcess::setVerifyMode(bool verify)
{
	mVerify = verify;
}

const pie::hac::ContentMeta& nstool::CnmtProcess::getContentMeta() const
{
	return mCnmt;
}

void nstool::CnmtProcess::importCnmt()
{
	if (mFile == nullptr) {
		throw tc::Exception(mModuleName, "No file reader set.");
	}

    if (mFile->canRead() == false || mFile->canSeek() == false) {
		throw tc::NotSupportedException(mModuleName, "Input stream requires read/seek permissions.");
	}

	// check if file_size is greater than 20MB, don't import.
	size_t cnmt_file_size = tc::io::IOUtil::castInt64ToSize(mFile->length());

    if (cnmt_file_size > (0x100000 * 20)) {
		throw tc::Exception(mModuleName, "File too large.");
	}

	// read cnmt
	tc::ByteData scratch = tc::ByteData(cnmt_file_size);
	mFile->seek(0, tc::io::SeekOrigin::Begin);
	mFile->read(scratch.data(), scratch.size());

	// parse cnmt
	mCnmt.fromBytes(scratch.data(), scratch.size());
}

void nstool::CnmtProcess::displayCnmt()
{
	const pie::hac::sContentMetaHeader* cnmt_hdr = (const pie::hac::sContentMetaHeader*)mCnmt.getBytes().data();

    handlePrint("[ContentMeta]\n");

    if (outputJSON) {
        output["archive"]["titleId"]["int"] = mCnmt.getTitleId();
        output["archive"]["titleId"]["hex"] = fmt::format("0x{:016x}", mCnmt.getTitleId());
        output["archive"]["titleId"]["string"] = fmt::format("{:016x}", mCnmt.getTitleId());
        output["archive"]["version"]["string"] = pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getTitleVersion());
        output["archive"]["version"]["int"] = mCnmt.getTitleVersion();
        output["archive"]["type"]["string"] = pie::hac::ContentMetaUtil::getContentMetaTypeAsString(mCnmt.getContentMetaType());
        output["archive"]["type"]["int"] = (uint32_t)mCnmt.getContentMetaType();
        output["archive"]["attributes"] = fmt::format("0x{:x}", *((byte_t*)&cnmt_hdr->attributes));
    } else {
        fmt::print("  TitleId:               0x{:016x}\n", mCnmt.getTitleId());
        fmt::print("  Version:               {:s} (v{:d})\n", pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getTitleVersion()), mCnmt.getTitleVersion());
        fmt::print("  Type:                  {:s} ({:d})\n", pie::hac::ContentMetaUtil::getContentMetaTypeAsString(mCnmt.getContentMetaType()), (uint32_t)mCnmt.getContentMetaType());
        fmt::print("  Attributes:            0x{:x}", *((byte_t*)&cnmt_hdr->attributes));
    }

    if (mCnmt.getAttribute().size()) {
		std::vector<std::string> attribute_list;

		for (auto itr = mCnmt.getAttribute().begin(); itr != mCnmt.getAttribute().end(); itr++) {
			attribute_list.push_back(pie::hac::ContentMetaUtil::getContentMetaAttributeFlagAsString(pie::hac::cnmt::ContentMetaAttributeFlag(*itr)));
		}

		handlePrint(" [");

        for (auto itr = attribute_list.begin(); itr != attribute_list.end(); itr++) {
            if (outputJSON) {
                output["archive"]["attributeList"].push_back(*itr);
            } else {
                fmt::print("{:s}",*itr);

                if ((itr + 1) != attribute_list.end()) {
                    fmt::print(", ");
                }
            }
		}

		handlePrint("]");
	}

    handlePrint("\n");

    if (outputJSON) {
        output["archive"]["storageId"]["string"] = pie::hac::ContentMetaUtil::getStorageIdAsString(mCnmt.getStorageId());
        output["archive"]["storageId"]["int"] = (uint32_t)mCnmt.getStorageId();
        output["archive"]["contentInstallType"]["string"] = pie::hac::ContentMetaUtil::getContentInstallTypeAsString(mCnmt.getContentInstallType());
        output["archive"]["contentInstallType"]["int"] = (uint32_t)mCnmt.getContentInstallType();
        output["archive"]["requiredDownloadSystemVersion"]["string"] = pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getRequiredDownloadSystemVersion());
        output["archive"]["requiredDownloadSystemVersion"]["int"] = mCnmt.getRequiredDownloadSystemVersion();
    } else {
        fmt::print("  StorageId:             {:s} ({:d})\n", pie::hac::ContentMetaUtil::getStorageIdAsString(mCnmt.getStorageId()), (uint32_t)mCnmt.getStorageId());
        fmt::print("  ContentInstallType:    {:s} ({:d})\n", pie::hac::ContentMetaUtil::getContentInstallTypeAsString(mCnmt.getContentInstallType()),(uint32_t)mCnmt.getContentInstallType());
        fmt::print("  RequiredDownloadSystemVersion: {:s} (v{:d})\n", pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getRequiredDownloadSystemVersion()), mCnmt.getRequiredDownloadSystemVersion());
    }

    switch (mCnmt.getContentMetaType()) {
		case (pie::hac::cnmt::ContentMetaType_Application):
            if (outputJSON) {
                output["archive"]["extendedHeader"]["type"] = "application";
                output["archive"]["extendedHeader"]["requiredApplicationVersion"]["string"] = pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getApplicationMetaExtendedHeader().getRequiredApplicationVersion());
                output["archive"]["extendedHeader"]["requiredApplicationVersion"]["int"] = mCnmt.getApplicationMetaExtendedHeader().getRequiredApplicationVersion();
                output["archive"]["extendedHeader"]["requiredSystemVersion"]["string"] = pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getApplicationMetaExtendedHeader().getRequiredSystemVersion());
                output["archive"]["extendedHeader"]["requiredSystemVersion"]["int"] = mCnmt.getApplicationMetaExtendedHeader().getRequiredSystemVersion();
                output["archive"]["extendedHeader"]["patchId"]["hex"] = fmt::format("0x{:016x}", mCnmt.getApplicationMetaExtendedHeader().getPatchId());
                output["archive"]["extendedHeader"]["patchId"]["string"] = fmt::format("{:016x}", mCnmt.getApplicationMetaExtendedHeader().getPatchId());
                output["archive"]["extendedHeader"]["patchId"]["int"] = mCnmt.getApplicationMetaExtendedHeader().getPatchId();
            } else {
                fmt::print("  ApplicationExtendedHeader:\n");
                fmt::print("    RequiredApplicationVersion: {:s} (v{:d})\n", pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getApplicationMetaExtendedHeader().getRequiredApplicationVersion()), mCnmt.getApplicationMetaExtendedHeader().getRequiredApplicationVersion());
                fmt::print("    RequiredSystemVersion:      {:s} (v{:d})\n", pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getApplicationMetaExtendedHeader().getRequiredSystemVersion()), mCnmt.getApplicationMetaExtendedHeader().getRequiredSystemVersion());
                fmt::print("    PatchId:                    0x{:016x}\n", mCnmt.getApplicationMetaExtendedHeader().getPatchId());
            }
			break;
		case (pie::hac::cnmt::ContentMetaType_Patch):
            if (outputJSON) {
                output["archive"]["extendedHeader"]["type"] = "patch meta";
                output["archive"]["extendedHeader"]["requiredSystemVersion"]["string"] = pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getPatchMetaExtendedHeader().getRequiredSystemVersion());
                output["archive"]["extendedHeader"]["requiredSystemVersion"]["int"] = mCnmt.getPatchMetaExtendedHeader().getRequiredSystemVersion();
                output["archive"]["extendedHeader"]["applicationId"]["hex"] = fmt::format("0x{:016x}", mCnmt.getPatchMetaExtendedHeader().getApplicationId());
                output["archive"]["extendedHeader"]["applicationId"]["string"] = fmt::format("{:016x}", mCnmt.getPatchMetaExtendedHeader().getApplicationId());
                output["archive"]["extendedHeader"]["applicationId"]["int"] = mCnmt.getPatchMetaExtendedHeader().getApplicationId();
            } else {
                fmt::print("  PatchMetaExtendedHeader:\n");
                fmt::print("    RequiredSystemVersion: {:s} (v{:d})\n", pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getPatchMetaExtendedHeader().getRequiredSystemVersion()), mCnmt.getPatchMetaExtendedHeader().getRequiredSystemVersion());
                fmt::print("    ApplicationId:         0x{:016x}\n", mCnmt.getPatchMetaExtendedHeader().getApplicationId());
            }
			break;
		case (pie::hac::cnmt::ContentMetaType_AddOnContent):
            if (outputJSON) {
                output["archive"]["extendedHeader"]["type"] = "addon content meta";
                output["archive"]["extendedHeader"]["requiredSystemVersion"]["string"] = pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getAddOnContentMetaExtendedHeader().getRequiredApplicationVersion());
                output["archive"]["extendedHeader"]["requiredSystemVersion"]["int"] = mCnmt.getAddOnContentMetaExtendedHeader().getRequiredApplicationVersion();
                output["archive"]["extendedHeader"]["applicationId"]["hex"] = fmt::format("0x{:016x}", mCnmt.getAddOnContentMetaExtendedHeader().getApplicationId());
                output["archive"]["extendedHeader"]["applicationId"]["string"] = fmt::format("{:016x}", mCnmt.getAddOnContentMetaExtendedHeader().getApplicationId());
                output["archive"]["extendedHeader"]["applicationId"]["int"] = mCnmt.getAddOnContentMetaExtendedHeader().getApplicationId();
            } else {
                fmt::print("  AddOnContentMetaExtendedHeader:\n");
                fmt::print("    RequiredApplicationVersion: {:s} (v{:d})\n", pie::hac::ContentMetaUtil::getVersionAsString(mCnmt.getAddOnContentMetaExtendedHeader().getRequiredApplicationVersion()), mCnmt.getAddOnContentMetaExtendedHeader().getRequiredApplicationVersion());
                fmt::print("    ApplicationId:         0x{:016x}\n", mCnmt.getAddOnContentMetaExtendedHeader().getApplicationId());
            }
			break;
		case (pie::hac::cnmt::ContentMetaType_Delta):
            if (outputJSON) {
                output["archive"]["extendedHeader"]["type"] = "delta meta";
                output["archive"]["extendedHeader"]["applicationId"]["hex"] = fmt::format("0x{:016x}", mCnmt.getDeltaMetaExtendedHeader().getApplicationId());
                output["archive"]["extendedHeader"]["applicationId"]["string"] = fmt::format("{:016x}", mCnmt.getDeltaMetaExtendedHeader().getApplicationId());
                output["archive"]["extendedHeader"]["applicationId"]["int"] = mCnmt.getDeltaMetaExtendedHeader().getApplicationId();
            } else {
                fmt::print("  DeltaMetaExtendedHeader:\n");
                fmt::print("    ApplicationId:         0x{:016x}\n", mCnmt.getDeltaMetaExtendedHeader().getApplicationId());
            }
			break;
		default:
			break;
	}

    if (mCnmt.getContentInfo().size() > 0) {
		handlePrint("  ContentInfo:\n");

        for (size_t i = 0; i < mCnmt.getContentInfo().size(); i++) {
			const pie::hac::ContentInfo& info = mCnmt.getContentInfo()[i];

            if (outputJSON) {
                output["archive"]["contentInfo"][i]["type"]["string"] = pie::hac::ContentMetaUtil::getContentTypeAsString(info.getContentType());
                output["archive"]["contentInfo"][i]["type"]["int"] = (uint32_t)info.getContentType();
                output["archive"]["contentInfo"][i]["id"] = tc::cli::FormatUtil::formatBytesAsString(info.getContentId().data(), info.getContentId().size(), false, "");
                output["archive"]["contentInfo"][i]["size"] = fmt::format("0x{:x}", info.getContentSize());
                output["archive"]["contentInfo"][i]["hash"] = tc::cli::FormatUtil::formatBytesAsString(info.getContentHash().data(), info.getContentHash().size(), false, "");
            } else {
                fmt::print("    {:d}\n", i);
                fmt::print("      Type:         {:s} ({:d})\n", pie::hac::ContentMetaUtil::getContentTypeAsString(info.getContentType()), (uint32_t)info.getContentType());
                fmt::print("      Id:           {:s}\n", tc::cli::FormatUtil::formatBytesAsString(info.getContentId().data(), info.getContentId().size(), false, ""));
                fmt::print("      Size:         0x{:x}\n", info.getContentSize());
                fmt::print("      Hash:         {:s}\n", tc::cli::FormatUtil::formatBytesAsString(info.getContentHash().data(), info.getContentHash().size(), false, ""));
            }
		}
	}

    if (mCnmt.getContentMetaInfo().size() > 0) {
		handlePrint("  ContentMetaInfo:\n");
		displayContentMetaInfoList(mCnmt.getContentMetaInfo(), "    ");
	}

	// print extended data
	if (mCnmt.getContentMetaType() == pie::hac::cnmt::ContentMetaType_Patch && mCnmt.getPatchMetaExtendedHeader().getExtendedDataSize() != 0) {
		// this is stubbed because the raw output is for development purposes
        if (outputJSON) {
            output["archive"]["extendedData"]["type"] = "patch meta";
        } else {
            //fmt::print("  PatchMetaExtendedData:\n");
		    //tc::cli::FormatUtil::formatBytesAsHxdHexString(mCnmt.getPatchMetaExtendedData().data(), mCnmt.getPatchMetaExtendedData().size());
        }
	} else if (mCnmt.getContentMetaType() == pie::hac::cnmt::ContentMetaType_Delta && mCnmt.getDeltaMetaExtendedHeader().getExtendedDataSize() != 0) {
		// this is stubbed because the raw output is for development purposes
        if (outputJSON) {
            output["archive"]["extendedData"]["type"] = "delta meta";
        } else {
            //fmt::print("  DeltaMetaExtendedData:\n");
            //tc::cli::FormatUtil::formatBytesAsHxdHexString(mCnmt.getDeltaMetaExtendedData().data(), mCnmt.getDeltaMetaExtendedData().size());
        }
	} else if (mCnmt.getContentMetaType() == pie::hac::cnmt::ContentMetaType_SystemUpdate && mCnmt.getSystemUpdateMetaExtendedHeader().getExtendedDataSize() != 0) {
		if (outputJSON) {
            output["archive"]["extendedData"]["type"] = "system update meta";
            output["archive"]["extendedData"]["formatVersion"] = mCnmt.getSystemUpdateMetaExtendedData().getFormatVersion();
        } else {
            fmt::print("  SystemUpdateMetaExtendedData:\n");
            fmt::print("    FormatVersion:         {:d}\n", mCnmt.getSystemUpdateMetaExtendedData().getFormatVersion());
            fmt::print("    FirmwareVariation:\n");
        }

		auto variation_info = mCnmt.getSystemUpdateMetaExtendedData().getFirmwareVariationInfo();

        for (size_t i = 0; i < mCnmt.getSystemUpdateMetaExtendedData().getFirmwareVariationInfo().size(); i++)
		{
            if (outputJSON) {
                output["archive"]["firmwareVariation"][i]["index"] = i;
                output["archive"]["firmwareVariation"][i]["id"] = fmt::format("0x{:x}", variation_info[i].variation_id);
            } else {
                fmt::print("      {:d}\n", i);
                fmt::print("        FirmwareVariationId:  0x{:x}\n", variation_info[i].variation_id);
            }

            if (mCnmt.getSystemUpdateMetaExtendedData().getFormatVersion() == 2) {
                if (outputJSON) {
                    output["archive"]["firmwareVariation"][i]["referToBase"] = variation_info[i].meta.empty();
                } else {
                    fmt::print("        ReferToBase:          {}\n", variation_info[i].meta.empty());
                }

                if (variation_info[i].meta.empty() == false) {
					fmt::print("        ContentMeta:\n");
					displayContentMetaInfoList(variation_info[i].meta, "          ");
				}
			}
		}
	}

    if (outputJSON) {
        output["archive"]["digest"] = tc::cli::FormatUtil::formatBytesAsString(mCnmt.getDigest().data(), mCnmt.getDigest().size(), false, "");
    } else {
        fmt::print("  Digest:   {:s}\n", tc::cli::FormatUtil::formatBytesAsString(mCnmt.getDigest().data(), mCnmt.getDigest().size(), false, ""));
    }
}

void nstool::CnmtProcess::displayContentMetaInfo(const pie::hac::ContentMetaInfo& content_meta_info, const std::string& prefix)
{
	const pie::hac::sContentMetaInfo* content_meta_info_raw = (const pie::hac::sContentMetaInfo*)content_meta_info.getBytes().data();
	fmt::print("{:s}Id:           0x{:016x}\n", prefix, content_meta_info.getTitleId());
	fmt::print("{:s}Version:      {:s} (v{:d})\n", prefix, pie::hac::ContentMetaUtil::getVersionAsString(content_meta_info.getTitleVersion()), content_meta_info.getTitleVersion());
	fmt::print("{:s}Type:         {:s} ({:d})\n", prefix, pie::hac::ContentMetaUtil::getContentMetaTypeAsString(content_meta_info.getContentMetaType()), (uint32_t)content_meta_info.getContentMetaType());
	fmt::print("{:s}Attributes:   0x{:x}", prefix, *((byte_t*)&content_meta_info_raw->attributes) );

    if (content_meta_info.getAttribute().size()) {
		std::vector<std::string> attribute_list;

		for (auto itr = content_meta_info.getAttribute().begin(); itr != content_meta_info.getAttribute().end(); itr++) {
			attribute_list.push_back(pie::hac::ContentMetaUtil::getContentMetaAttributeFlagAsString(pie::hac::cnmt::ContentMetaAttributeFlag(*itr)));
		}

		fmt::print(" [");

        for (auto itr = attribute_list.begin(); itr != attribute_list.end(); itr++) {
			fmt::print("{:s}",*itr);

            if ((itr + 1) != attribute_list.end()) {
				fmt::print(", ");
			}
		}

		fmt::print("]");
	}

	fmt::print("\n");
}

void nstool::CnmtProcess::displayContentMetaInfoList(const std::vector<pie::hac::ContentMetaInfo>& content_meta_info_list, const std::string& prefix)
{
	for (size_t i = 0; i < content_meta_info_list.size(); i++) {
		const pie::hac::ContentMetaInfo& info = mCnmt.getContentMetaInfo()[i];
        fmt::print("{:d}\n", i);
		displayContentMetaInfo(info, prefix + "  ");
	}
}
