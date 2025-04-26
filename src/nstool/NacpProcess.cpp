#include "NacpProcess.h"
#include "Output.hpp"

#include <pietendo/hac/ApplicationControlPropertyUtil.h>

nstool::NacpProcess::NacpProcess() :
	mModuleName("nstool::NacpProcess"),
	mFile(),
	mCliOutputMode(true, false, false, false, false),
	mVerify(false)
{
}

void nstool::NacpProcess::process()
{
    handleLog("Processing NACP file.");

	importNacp();

	if (mCliOutputMode.show_basic_info) {
        displayNacp();
    }
}

void nstool::NacpProcess::setInputFile(const std::shared_ptr<tc::io::IStream>& file)
{
	mFile = file;
}

void nstool::NacpProcess::setCliOutputMode(CliOutputMode type)
{
	mCliOutputMode = type;
}

void nstool::NacpProcess::setVerifyMode(bool verify)
{
	mVerify = verify;
}

const pie::hac::ApplicationControlProperty& nstool::NacpProcess::getApplicationControlProperty() const
{
	return mNacp;
}

void nstool::NacpProcess::importNacp()
{
	if (mFile == nullptr)
	{
		throw tc::Exception(mModuleName, "No file reader set.");
	}

	if (mFile->canRead() == false || mFile->canSeek() == false)
	{
		throw tc::NotSupportedException(mModuleName, "Input stream requires read/seek permissions.");
	}

	// check if file_size does matches expected size
	size_t file_size = tc::io::IOUtil::castInt64ToSize(mFile->length());
	if (file_size != sizeof(pie::hac::sApplicationControlProperty))
	{
		throw tc::Exception(mModuleName, "File was incorrect size.");
	}

	// read cnmt
	tc::ByteData scratch = tc::ByteData(file_size);
	mFile->seek(0, tc::io::SeekOrigin::Begin);
	mFile->read(scratch.data(), scratch.size());

	mNacp.fromBytes(scratch.data(), scratch.size());
}

void nstool::NacpProcess::displayNacp()
{
    if (!outputJSON) {
        fmt::print("[ApplicationControlProperty]\n");
    }

	// Title
	if (mNacp.getTitle().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getTitle().begin(); itr != mNacp.getTitle().end(); itr++) {
                nlohmann::json entry = {
                    {"language", pie::hac::ApplicationControlPropertyUtil::getLanguageAsString(itr->language)},
                    {"name", itr->name},
                    {"publisher", itr->publisher},
                };

                output["archive"]["title"].push_back(entry);
            }
        } else {
            fmt::print("  Title:\n");
            for (auto itr = mNacp.getTitle().begin(); itr != mNacp.getTitle().end(); itr++)
            {
                fmt::print("    {:s}:\n", pie::hac::ApplicationControlPropertyUtil::getLanguageAsString(itr->language));
                fmt::print("      Name:       {:s}\n", itr->name);
                fmt::print("      Publisher:  {:s}\n", itr->publisher);
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["title"] = {};
        } else {
            fmt::print("  Title:                                  None\n");
        }
	}

	// Isbn
	if (mNacp.getIsbn().empty() == false)
	{
        if (outputJSON) {
		    output["archive"]["isbn"] = mNacp.getIsbn();
        } else {
            fmt::print("  ISBN:                                   {:s}\n", mNacp.getIsbn());
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
		    output["archive"]["isbn"] = "";
        } else {
		    fmt::print("  ISBN:                                   (NotSet)\n");
        }
	}

	// StartupUserAccount
	if (mNacp.getStartupUserAccount() != pie::hac::nacp::StartupUserAccount_None || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
		    output["archive"]["startupUserAccount"] = pie::hac::ApplicationControlPropertyUtil::getStartupUserAccountAsString(mNacp.getStartupUserAccount());
        } else {
		    fmt::print("  StartupUserAccount:                     {:s}\n", pie::hac::ApplicationControlPropertyUtil::getStartupUserAccountAsString(mNacp.getStartupUserAccount()));
        }
	}

	// UserAccountSwitchLock
	if (mNacp.getUserAccountSwitchLock() != pie::hac::nacp::UserAccountSwitchLock_Disable || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["userAccountSwitchLock"] = pie::hac::ApplicationControlPropertyUtil::getUserAccountSwitchLockAsString(mNacp.getUserAccountSwitchLock());
        } else {
            fmt::print("  UserAccountSwitchLock:                  {:s}\n", pie::hac::ApplicationControlPropertyUtil::getUserAccountSwitchLockAsString(mNacp.getUserAccountSwitchLock()));
        }
	}

	// AddOnContentRegistrationType
	if (mNacp.getAddOnContentRegistrationType() != pie::hac::nacp::AddOnContentRegistrationType_AllOnLaunch || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["addOnContentRegistrationType"] = pie::hac::ApplicationControlPropertyUtil::getAddOnContentRegistrationTypeAsString(mNacp.getAddOnContentRegistrationType());
        } else {
            fmt::print("  AddOnContentRegistrationType:           {:s}\n", pie::hac::ApplicationControlPropertyUtil::getAddOnContentRegistrationTypeAsString(mNacp.getAddOnContentRegistrationType()));
        }
	}

	// Attribute
	if (mNacp.getAttribute().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getAttribute().begin(); itr != mNacp.getAttribute().end(); itr++) {
                output["archive"]["attributes"].push_back(pie::hac::ApplicationControlPropertyUtil::getAttributeFlagAsString(*itr));
            }
        } else {
            fmt::print("  Attribute:\n");
            for (auto itr = mNacp.getAttribute().begin(); itr != mNacp.getAttribute().end(); itr++)
            {
                fmt::print("    {:s}\n", pie::hac::ApplicationControlPropertyUtil::getAttributeFlagAsString(*itr));
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["attributes"] = {};
        } else {
            fmt::print("  Attribute:                              None\n");
        }
	}

	// SupportedLanguage
	if (mNacp.getSupportedLanguage().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getSupportedLanguage().begin(); itr != mNacp.getSupportedLanguage().end(); itr++) {
                output["archive"]["supportedLanguages"].push_back(pie::hac::ApplicationControlPropertyUtil::getLanguageAsString(*itr));
            }
        } else {
            fmt::print("  SupportedLanguage:\n");
            for (auto itr = mNacp.getSupportedLanguage().begin(); itr != mNacp.getSupportedLanguage().end(); itr++)
            {
                fmt::print("    {:s}\n", pie::hac::ApplicationControlPropertyUtil::getLanguageAsString(*itr));
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["supportedLanguages"] = {};
        } else {
            fmt::print("  SupportedLanguage:                      None\n");
        }
	}

	// ParentalControl
	if (mNacp.getParentalControl().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getParentalControl().begin(); itr != mNacp.getParentalControl().end(); itr++)
            {
                output["archive"]["parentalControl"].push_back(pie::hac::ApplicationControlPropertyUtil::getParentalControlFlagAsString(*itr));
            }
        } else {
            fmt::print("  ParentalControl:\n");
            for (auto itr = mNacp.getParentalControl().begin(); itr != mNacp.getParentalControl().end(); itr++)
            {
                fmt::print("    {:s}\n", pie::hac::ApplicationControlPropertyUtil::getParentalControlFlagAsString(*itr));
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["parentalControl"] = {};
        } else {
            fmt::print("  ParentalControl:                      None\n");
        }
	}

	// Screenshot
	if (mNacp.getScreenshot() != pie::hac::nacp::Screenshot_Allow || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["screenshot"] = pie::hac::ApplicationControlPropertyUtil::getScreenshotAsString(mNacp.getScreenshot());
        } else {
		    fmt::print("  Screenshot:                             {:s}\n", pie::hac::ApplicationControlPropertyUtil::getScreenshotAsString(mNacp.getScreenshot()));
        }
	}

	// VideoCapture
	if (mNacp.getVideoCapture() != pie::hac::nacp::VideoCapture_Disable || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["videoCapture"] = pie::hac::ApplicationControlPropertyUtil::getVideoCaptureAsString(mNacp.getVideoCapture());
        } else {
            fmt::print("  VideoCapture:                           {:s}\n", pie::hac::ApplicationControlPropertyUtil::getVideoCaptureAsString(mNacp.getVideoCapture()));
        }
	}

	// DataLossConfirmation
	if (mNacp.getDataLossConfirmation() != pie::hac::nacp::DataLossConfirmation_None || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["dataLossConfirmation"] = pie::hac::ApplicationControlPropertyUtil::getDataLossConfirmationAsString(mNacp.getDataLossConfirmation());
        } else {
            fmt::print("  DataLossConfirmation:                   {:s}\n", pie::hac::ApplicationControlPropertyUtil::getDataLossConfirmationAsString(mNacp.getDataLossConfirmation()));
        }
	}

	// PlayLogPolicy
	if (mNacp.getPlayLogPolicy() != pie::hac::nacp::PlayLogPolicy_All || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["playLogPolicy"] = pie::hac::ApplicationControlPropertyUtil::getPlayLogPolicyAsString(mNacp.getPlayLogPolicy());
        } else {
            fmt::print("  PlayLogPolicy:                          {:s}\n", pie::hac::ApplicationControlPropertyUtil::getPlayLogPolicyAsString(mNacp.getPlayLogPolicy()));
        }
	}

	// PresenceGroupId
	if (mNacp.getPresenceGroupId() != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["presenceGroupID"] = fmt::format("{:016x}", mNacp.getPresenceGroupId());
        } else {
            fmt::print("  PresenceGroupId:                        0x{:016x}\n", mNacp.getPresenceGroupId());
        }
	}

	// RatingAge
	if (mNacp.getRatingAge().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getRatingAge().begin(); itr != mNacp.getRatingAge().end(); itr++) {
                nlohmann::json entry = {
                    {"group", pie::hac::ApplicationControlPropertyUtil::getOrganisationAsString(itr->organisation)},
                    {"rating", itr->age},
                };

                output["archive"]["ageRatings"].push_back(entry);
            }
        } else {
            fmt::print("  RatingAge:\n");

            for (auto itr = mNacp.getRatingAge().begin(); itr != mNacp.getRatingAge().end(); itr++)
            {
                fmt::print("    {:s}:\n", pie::hac::ApplicationControlPropertyUtil::getOrganisationAsString(itr->organisation));
                fmt::print("      Age: {:d}\n", itr->age);
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["ageRatings"] = {};
        } else {
            fmt::print("  RatingAge:                              None\n");
        }
	}

	// DisplayVersion
	if (mNacp.getDisplayVersion().empty() == false)
	{
        if (outputJSON) {
            output["archive"]["displayVersion"] = mNacp.getDisplayVersion();
        } else {
            fmt::print("  DisplayVersion:                         {:s}\n", mNacp.getDisplayVersion());
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["displayVersion"] = "";
        } else {
            fmt::print("  DisplayVersion:                         (NotSet)\n");
        }
	}

	// AddOnContentBaseId
	if (mNacp.getAddOnContentBaseId() != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["addOnContentBaseID"] = fmt::format("{:016x}", mNacp.getAddOnContentBaseId());
        } else {
            fmt::print("  AddOnContentBaseId:                     0x{:016x}\n", mNacp.getAddOnContentBaseId());
        }
	}

	// SaveDataOwnerId
	if (mNacp.getSaveDataOwnerId() != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["saveDataOwnerID"] = fmt::format("{:016x}", mNacp.getSaveDataOwnerId());
        } else {
            fmt::print("  SaveDataOwnerId:                        0x{:016x}\n", mNacp.getSaveDataOwnerId());
        }
	}

	// UserAccountSaveDataSize
	if (mNacp.getUserAccountSaveDataSize().size != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["userAccountSaveDataSize"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getUserAccountSaveDataSize().size);
        } else {
            fmt::print("  UserAccountSaveDataSize:                {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getUserAccountSaveDataSize().size));
        }
	}

	// UserAccountSaveDataJournalSize
	if (mNacp.getUserAccountSaveDataSize().journal_size != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["userAccountSaveDataJournalSize"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getUserAccountSaveDataSize().journal_size);
        } else {
            fmt::print("  UserAccountSaveDataJournalSize:         {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getUserAccountSaveDataSize().journal_size));
        }
	}

	// DeviceSaveDataSize
	if (mNacp.getDeviceSaveDataSize().size != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["deviceSaveDataSize"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getDeviceSaveDataSize().size);
        } else {
            fmt::print("  DeviceSaveDataSize:                     {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getDeviceSaveDataSize().size));
        }
	}

	// DeviceSaveDataJournalSize
	if (mNacp.getDeviceSaveDataSize().journal_size != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["deviceSaveDataJournalSize"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getDeviceSaveDataSize().journal_size);
        } else {
            fmt::print("  DeviceSaveDataJournalSize:              {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getDeviceSaveDataSize().journal_size));
        }
	}

	// BcatDeliveryCacheStorageSize
	if (mNacp.getBcatDeliveryCacheStorageSize() != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["bcatDeliveryCacheStorageSize"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getBcatDeliveryCacheStorageSize());
        } else {
            fmt::print("  BcatDeliveryCacheStorageSize:           {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getBcatDeliveryCacheStorageSize()));
        }
	}

	// ApplicationErrorCodeCategory
	if (mNacp.getApplicationErrorCodeCategory().empty() == false)
	{
        if (outputJSON) {
            output["archive"]["applicationErrorCodeCategory"] = mNacp.getApplicationErrorCodeCategory();
        } else {
            fmt::print("  ApplicationErrorCodeCategory:           {:s}\n", mNacp.getApplicationErrorCodeCategory());
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["applicationErrorCodeCategory"] = "";
        } else {
            fmt::print("  ApplicationErrorCodeCategory:           (NotSet)\n");
        }
	}

	// LocalCommunicationId
	if (mNacp.getLocalCommunicationId().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getLocalCommunicationId().begin(); itr != mNacp.getLocalCommunicationId().end(); itr++) {
                output["archive"]["localCommunicationID"].push_back(fmt::format("{:016x}", *itr));
            }
        } else {
            fmt::print("  LocalCommunicationId:\n");
            for (auto itr = mNacp.getLocalCommunicationId().begin(); itr != mNacp.getLocalCommunicationId().end(); itr++)
            {
                fmt::print("    0x{:016x}\n", *itr);
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["localCommunicationID"] = {};
        } else {
            fmt::print("  LocalCommunicationId:                   None\n");
        }
	}

	// LogoType
    if (outputJSON) {
        output["archive"]["logoType"] = pie::hac::ApplicationControlPropertyUtil::getLogoTypeAsString(mNacp.getLogoType());
    } else {
        fmt::print("  LogoType:                               {:s}\n", pie::hac::ApplicationControlPropertyUtil::getLogoTypeAsString(mNacp.getLogoType()));
    }

	// LogoHandling
	if (mNacp.getLogoHandling() != pie::hac::nacp::LogoHandling_Auto || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["logoHandling"] = pie::hac::ApplicationControlPropertyUtil::getLogoHandlingAsString(mNacp.getLogoHandling());
        } else {
            fmt::print("  LogoHandling:                           {:s}\n", pie::hac::ApplicationControlPropertyUtil::getLogoHandlingAsString(mNacp.getLogoHandling()));
        }
	}

	// RuntimeAddOnContentInstall
	if (mNacp.getRuntimeAddOnContentInstall() != pie::hac::nacp::RuntimeAddOnContentInstall_Deny || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["runtimeAddOnContentInstall"] = pie::hac::ApplicationControlPropertyUtil::getRuntimeAddOnContentInstallAsString(mNacp.getRuntimeAddOnContentInstall());
        } else {
            fmt::print("  RuntimeAddOnContentInstall:             {:s}\n", pie::hac::ApplicationControlPropertyUtil::getRuntimeAddOnContentInstallAsString(mNacp.getRuntimeAddOnContentInstall()));
        }
	}

	// RuntimeParameterDelivery
	if (mNacp.getRuntimeParameterDelivery() != pie::hac::nacp::RuntimeParameterDelivery_Always || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["runtimeParameterDelivery"] = pie::hac::ApplicationControlPropertyUtil::getRuntimeParameterDeliveryAsString(mNacp.getRuntimeParameterDelivery());
        } else {
            fmt::print("  RuntimeParameterDelivery:               {:s}\n", pie::hac::ApplicationControlPropertyUtil::getRuntimeParameterDeliveryAsString(mNacp.getRuntimeParameterDelivery()));
        }
	}

	// CrashReport
	if (mNacp.getCrashReport() != pie::hac::nacp::CrashReport_Deny || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["crashReport"] = pie::hac::ApplicationControlPropertyUtil::getCrashReportAsString(mNacp.getCrashReport());
        } else {
            fmt::print("  CrashReport:                            {:s}\n", pie::hac::ApplicationControlPropertyUtil::getCrashReportAsString(mNacp.getCrashReport()));
        }
	}

	// Hdcp
	if (mNacp.getHdcp() != pie::hac::nacp::Hdcp_None || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["hdcp"] = pie::hac::ApplicationControlPropertyUtil::getHdcpAsString(mNacp.getHdcp());
        } else {
            fmt::print("  Hdcp:                                   {:s}\n", pie::hac::ApplicationControlPropertyUtil::getHdcpAsString(mNacp.getHdcp()));
        }
	}

	// SeedForPsuedoDeviceId
	if (mNacp.getSeedForPsuedoDeviceId() != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["seedForPsuedoDeviceId"] = fmt::format("{:016x}", mNacp.getSeedForPsuedoDeviceId());
        } else {
            fmt::print("  SeedForPsuedoDeviceId:                  0x{:016x}\n", mNacp.getSeedForPsuedoDeviceId());
        }
	}

	// BcatPassphase
	if (mNacp.getBcatPassphase().empty() == false)
	{
        if (outputJSON) {
            output["archive"]["bcatPassphase"] = mNacp.getBcatPassphase();
        } else {
            fmt::print("  BcatPassphase:                          {:s}\n", mNacp.getBcatPassphase());
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["bcatPassphase"] = "";
        } else {
            fmt::print("  BcatPassphase:                          (NotSet)\n");
        }
	}

	// StartupUserAccountOption
	if (mNacp.getStartupUserAccountOption().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getStartupUserAccountOption().begin(); itr != mNacp.getStartupUserAccountOption().end(); itr++) {
                output["archive"]["startupUserAccountOption"].push_back(pie::hac::ApplicationControlPropertyUtil::getStartupUserAccountOptionFlagAsString(*itr));
            }
        } else {
            fmt::print("  StartupUserAccountOption:\n");
            for (auto itr = mNacp.getStartupUserAccountOption().begin(); itr != mNacp.getStartupUserAccountOption().end(); itr++) {
                fmt::print("    {:s}\n", pie::hac::ApplicationControlPropertyUtil::getStartupUserAccountOptionFlagAsString(*itr));
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["startupUserAccountOption"] = "";
        } else {
            fmt::print("  StartupUserAccountOption:               None\n");
        }
	}

	// UserAccountSaveDataSizeMax
	if (mNacp.getUserAccountSaveDataMax().size != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["userAccountSaveDataSizeMax"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getUserAccountSaveDataMax().size);
        } else {
            fmt::print("  UserAccountSaveDataSizeMax:             {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getUserAccountSaveDataMax().size));
        }
	}

	// UserAccountSaveDataJournalSizeMax
	if (mNacp.getUserAccountSaveDataMax().journal_size != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["userAccountSaveDataJournalSizeMax"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getUserAccountSaveDataMax().journal_size);
        } else {
            fmt::print("  UserAccountSaveDataJournalSizeMax:      {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getUserAccountSaveDataMax().journal_size));
        }
	}

	// DeviceSaveDataSizeMax
	if (mNacp.getDeviceSaveDataMax().size != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["deviceSaveDataSizeMax"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getDeviceSaveDataMax().size);
        } else {
            fmt::print("  DeviceSaveDataSizeMax:                  {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getDeviceSaveDataMax().size));
        }
	}

	// DeviceSaveDataJournalSizeMax
	if (mNacp.getDeviceSaveDataMax().journal_size != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["deviceSaveDataJournalSizeMax"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getDeviceSaveDataMax().journal_size);
        } else {
            fmt::print("  DeviceSaveDataJournalSizeMax:           {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getDeviceSaveDataMax().journal_size));
        }
	}

	// TemporaryStorageSize
	if (mNacp.getTemporaryStorageSize() != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["temporaryStorageSize"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getTemporaryStorageSize());
        } else {
            fmt::print("  TemporaryStorageSize:                   {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getTemporaryStorageSize()));
        }
	}

	// CacheStorageSize
	if (mNacp.getCacheStorageSize().size != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["cacheStorageSize"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getCacheStorageSize().size);
        } else {
            fmt::print("  CacheStorageSize:                       {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getCacheStorageSize().size));
        }
	}

	// CacheStorageJournalSize
	if (mNacp.getCacheStorageSize().journal_size != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["cacheStorageJournalSize"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getCacheStorageSize().journal_size);
        } else {
            fmt::print("  CacheStorageJournalSize:                {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getCacheStorageSize().journal_size));
        }
	}

	// CacheStorageDataAndJournalSizeMax
	if (mNacp.getCacheStorageDataAndJournalSizeMax() != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["cacheStorageDataAndJournalSizeMax"] = pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getCacheStorageDataAndJournalSizeMax());
        } else {
            fmt::print("  CacheStorageDataAndJournalSizeMax:      {:s}\n", pie::hac::ApplicationControlPropertyUtil::getSaveDataSizeAsString(mNacp.getCacheStorageDataAndJournalSizeMax()));
        }
	}

	// CacheStorageIndexMax
	if (mNacp.getCacheStorageIndexMax() != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["cacheStorageIndexMax"] = fmt::format("{:004x}", mNacp.getCacheStorageIndexMax());
        } else {
            fmt::print("  CacheStorageIndexMax:                   0x{:04x}\n", mNacp.getCacheStorageIndexMax());
        }
	}

	// PlayLogQueryableApplicationId
	if (mNacp.getPlayLogQueryableApplicationId().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getPlayLogQueryableApplicationId().begin(); itr != mNacp.getPlayLogQueryableApplicationId().end(); itr++) {
                output["archive"]["playLogQueryableApplicationId"].push_back(fmt::format("{:016x}", *itr));
            }
        } else {
            fmt::print("  PlayLogQueryableApplicationId:\n");
            for (auto itr = mNacp.getPlayLogQueryableApplicationId().begin(); itr != mNacp.getPlayLogQueryableApplicationId().end(); itr++)
            {
                fmt::print("    0x{:016x}\n", *itr);
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["playLogQueryableApplicationId"] = {};
        } else {
            fmt::print("  PlayLogQueryableApplicationId:          None\n");
        }
	}

	// PlayLogQueryCapability
	if (mNacp.getPlayLogQueryCapability() != pie::hac::nacp::PlayLogQueryCapability_None || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["playLogQueryCapability"] = pie::hac::ApplicationControlPropertyUtil::getPlayLogQueryCapabilityAsString(mNacp.getPlayLogQueryCapability());
        } else {
            fmt::print("  PlayLogQueryCapability:                 {:s}\n", pie::hac::ApplicationControlPropertyUtil::getPlayLogQueryCapabilityAsString(mNacp.getPlayLogQueryCapability()));
        }
	}

	// Repair
	if (mNacp.getRepair().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getRepair().begin(); itr != mNacp.getRepair().end(); itr++) {
                output["archive"]["repair"].push_back(pie::hac::ApplicationControlPropertyUtil::getRepairFlagAsString(*itr));
            }
        } else {
            fmt::print("  Repair:\n");
            for (auto itr = mNacp.getRepair().begin(); itr != mNacp.getRepair().end(); itr++) {
                fmt::print("    {:s}\n", pie::hac::ApplicationControlPropertyUtil::getRepairFlagAsString(*itr));
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["repair"] = {};
        } else {
            fmt::print("  Repair:                                 None\n");
        }
	}

	// ProgramIndex
	if (mNacp.getProgramIndex() != 0 || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["programIndex"] = fmt::format("{:02x}", mNacp.getProgramIndex());
        } else {
            fmt::print("  ProgramIndex:                           0x{:02x}\n", mNacp.getProgramIndex());
        }
	}

	// RequiredNetworkServiceLicenseOnLaunch
	if (mNacp.getRequiredNetworkServiceLicenseOnLaunch().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getRequiredNetworkServiceLicenseOnLaunch().begin(); itr != mNacp.getRequiredNetworkServiceLicenseOnLaunch().end(); itr++) {
                output["archive"]["requiredNetworkServiceLicenseOnLaunch"].push_back(pie::hac::ApplicationControlPropertyUtil::getRequiredNetworkServiceLicenseOnLaunchFlagAsString(*itr));
            }
        } else {
            fmt::print("  RequiredNetworkServiceLicenseOnLaunch:\n");
            for (auto itr = mNacp.getRequiredNetworkServiceLicenseOnLaunch().begin(); itr != mNacp.getRequiredNetworkServiceLicenseOnLaunch().end(); itr++)
            {
                fmt::print("    {:s}\n", pie::hac::ApplicationControlPropertyUtil::getRequiredNetworkServiceLicenseOnLaunchFlagAsString(*itr));
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["requiredNetworkServiceLicenseOnLaunch"] = {};
        } else {
            fmt::print("  RequiredNetworkServiceLicenseOnLaunch:  None\n");
        }
	}

	// NeighborDetectionClientConfiguration
	auto detect_config = mNacp.getNeighborDetectionClientConfiguration();
	if (detect_config.countSendGroupConfig() > 0 || detect_config.countReceivableGroupConfig() > 0)
	{
        if (!outputJSON) {
            fmt::print("  NeighborDetectionClientConfiguration:\n");
        }

        if (detect_config.countSendGroupConfig() > 0)
        {
            if (outputJSON) {
                output["archive"]["neighborDetectionClientConfiguration"]["sendGroupConfig"]["groupID"] = fmt::format("{:016x}", detect_config.send_data_configuration.group_id);
                output["archive"]["neighborDetectionClientConfiguration"]["sendGroupConfig"]["key"] = tc::cli::FormatUtil::formatBytesAsString(detect_config.send_data_configuration.key.data(), detect_config.send_data_configuration.key.size(), false, "");
            } else {
                fmt::print("    SendGroupConfig:\n");
                fmt::print("      GroupId:  0x{:016x}\n", detect_config.send_data_configuration.group_id);
                fmt::print("        Key:    {:s}\n", tc::cli::FormatUtil::formatBytesAsString(detect_config.send_data_configuration.key.data(), detect_config.send_data_configuration.key.size(), false, ""));
            }
        }
        else if (mCliOutputMode.show_extended_info)
        {
            if (outputJSON) {
                output["archive"]["neighborDetectionClientConfiguration"]["sendGroupConfig"]["groupID"] = "";
                output["archive"]["neighborDetectionClientConfiguration"]["sendGroupConfig"]["key"] = "";
            } else {
                fmt::print("    SendGroupConfig: None\n");
            }
        }

		if (detect_config.countReceivableGroupConfig() > 0)
		{
            if (outputJSON) {
                for (size_t i = 0; i < pie::hac::nacp::kReceivableGroupConfigurationCount; i++)
                {
                    if (detect_config.receivable_data_configuration[i].isNull()) {
                        continue;
                    }

                    nlohmann::json entry = {
                        {"groupID", fmt::format("{:016x}", detect_config.receivable_data_configuration[i].group_id)},
                        {"key", tc::cli::FormatUtil::formatBytesAsString(detect_config.receivable_data_configuration[i].key.data(), detect_config.receivable_data_configuration[i].key.size(), false, "")},
                    };

                    output["archive"]["neighborDetectionClientConfiguration"]["receivableGroupConfig"].push_back(entry);
                }
            } else {
                fmt::print("    ReceivableGroupConfig:\n");
                for (size_t i = 0; i < pie::hac::nacp::kReceivableGroupConfigurationCount; i++)
                {
                    if (detect_config.receivable_data_configuration[i].isNull()) {
                        continue;
                    }

                    fmt::print("      GroupId:  0x{:016x}\n", detect_config.receivable_data_configuration[i].group_id);
                    fmt::print("        Key:    {:s}\n", tc::cli::FormatUtil::formatBytesAsString(detect_config.receivable_data_configuration[i].key.data(), detect_config.receivable_data_configuration[i].key.size(), false, ""));
                }
            }
		}
		else if (mCliOutputMode.show_extended_info)
		{
            if (outputJSON) {
                output["archive"]["neighborDetectionClientConfiguration"]["receivableGroupConfig"] = {};
            } else {
                fmt::print("    ReceivableGroupConfig: None\n");
            }
		}
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (!outputJSON) {
            fmt::print("  NeighborDetectionClientConfiguration:   None\n");
        }
	}

	// JitConfiguration
	if (mNacp.getJitConfiguration().is_enabled || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["jitConfiguration"]["isEnabled"] = mNacp.getJitConfiguration().is_enabled;
            output["archive"]["jitConfiguration"]["memorySize"] = fmt::format("{:016x}", mNacp.getJitConfiguration().memory_size);
        } else {
            fmt::print("  JitConfiguration:\n");
            fmt::print("    IsEnabled:  {}\n", mNacp.getJitConfiguration().is_enabled);
            fmt::print("    MemorySize: 0x{:016x}\n", mNacp.getJitConfiguration().memory_size);
        }
	}

	// PlayReportPermission
	if (mNacp.getPlayReportPermission() != pie::hac::nacp::PlayReportPermission_None || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["playReportPermission"] = pie::hac::ApplicationControlPropertyUtil::getPlayReportPermissionAsString(mNacp.getPlayReportPermission());
        } else {
            fmt::print("  PlayReportPermission:                   {:s}\n", pie::hac::ApplicationControlPropertyUtil::getPlayReportPermissionAsString(mNacp.getPlayReportPermission()));
        }
	}

	// CrashScreenshotForProd
	if (mNacp.getCrashScreenshotForProd() != pie::hac::nacp::CrashScreenshotForProd_Deny || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["crashScreenshotForProd"] = pie::hac::ApplicationControlPropertyUtil::getCrashScreenshotForProdAsString(mNacp.getCrashScreenshotForProd());
        } else {
            fmt::print("  CrashScreenshotForProd:                 {:s}\n", pie::hac::ApplicationControlPropertyUtil::getCrashScreenshotForProdAsString(mNacp.getCrashScreenshotForProd()));
        }
	}

	// CrashScreenshotForDev
	if (mNacp.getCrashScreenshotForDev() != pie::hac::nacp::CrashScreenshotForDev_Deny || mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["crashScreenshotForDev"] = pie::hac::ApplicationControlPropertyUtil::getCrashScreenshotForDevAsString(mNacp.getCrashScreenshotForDev());
        } else {
            fmt::print("  CrashScreenshotForDev:                  {:s}\n", pie::hac::ApplicationControlPropertyUtil::getCrashScreenshotForDevAsString(mNacp.getCrashScreenshotForDev()));
        }
	}

	// AccessibleLaunchRequiredVersion
	if (mNacp.getAccessibleLaunchRequiredVersionApplicationId().size() > 0)
	{
        if (outputJSON) {
            for (auto itr = mNacp.getAccessibleLaunchRequiredVersionApplicationId().begin(); itr != mNacp.getAccessibleLaunchRequiredVersionApplicationId().end(); itr++) {
                output["archive"]["accessibleLaunchRequiredVersion"]["applicationID"].push_back(fmt::format("{:016x}", *itr));
            }
        } else {
            fmt::print("  AccessibleLaunchRequiredVersion:\n");
            fmt::print("    ApplicationId:\n");
            for (auto itr = mNacp.getAccessibleLaunchRequiredVersionApplicationId().begin(); itr != mNacp.getAccessibleLaunchRequiredVersionApplicationId().end(); itr++)
            {
                fmt::print("      0x{:016x}\n", *itr);
            }
        }
	}
	else if (mCliOutputMode.show_extended_info)
	{
        if (outputJSON) {
            output["archive"]["accessibleLaunchRequiredVersion"]["applicationID"] = {};
        } else {
            fmt::print("  AccessibleLaunchRequiredVersion:        None\n");
        }
	}
}
