#include "FsProcess.h"
#include "util.h"
#include "Output.hpp"

#include <memory>
#include <tc/io/FileNotFoundException.h>
#include <tc/io/DirectoryNotFoundException.h>

nstool::FsProcess::FsProcess() :
	mModuleLabel("nstool::FsProcess"),
	mInputFs(),
	mFsFormatName(),
	mShowFsInfo(false),
	mProperties(),
	mShowFsTree(false),
    mOutputFile(),
	mFsRootLabel(),
	mExtractJobs(),
	mDataCache(0x10000)
{

}

void nstool::FsProcess::process()
{
	if (mInputFs == nullptr) {
		throw tc::InvalidOperationException(mModuleLabel, "No input filesystem");
	}

	if (mShowFsInfo) {
		handleLog(fmt::format("[{:s}]\n", mFsFormatName.isSet() ? mFsFormatName.get() : "FileSystem/Info"));

        for (auto itr = mProperties.begin(); itr != mProperties.end(); itr++) {
            handleLog(fmt::format("  {:s}\n", *itr));
		}
	}

	if (mShowFsTree) {
		printFs();
	}

	if (mExtractJobs.empty() == false) {
		extractFs();
	}
}

void nstool::FsProcess::setInputFileSystem(const std::shared_ptr<tc::io::IFileSystem>& input_fs)
{
	mInputFs = input_fs;
}

void nstool::FsProcess::setFsFormatName(const std::string& fs_format_name)
{
	mFsFormatName = fs_format_name;
}

void nstool::FsProcess::setShowFsInfo(bool show_fs_info)
{
	mShowFsInfo = show_fs_info;
}

void nstool::FsProcess::setFsProperties(const std::vector<std::string>& properties)
{
	mProperties = properties;
}

void nstool::FsProcess::setShowFsTree(bool show_fs_tree)
{
	mShowFsTree = show_fs_tree;
}

void nstool::FsProcess::setFsRootLabel(const std::string& root_label)
{
	mFsRootLabel = root_label;
}

void nstool::FsProcess::setExtractJobs(const std::vector<nstool::ExtractJob>& extract_jobs)
{
	mExtractJobs = extract_jobs;
}

void nstool::FsProcess::setExtractFile(std::string outputFile)
{
	mOutputFile = outputFile;
}

void nstool::FsProcess::printFs()
{
	handleLog(fmt::format("[{:s}/Tree]\n", (mFsFormatName.isSet() ? mFsFormatName.get() : "FileSystem")));
	visitDir(tc::io::Path("/"), tc::io::Path("/"), false, true);
}

void nstool::FsProcess::extractFs()
{
    handleLog(fmt::format("[{:s}/Extract]\n", (mFsFormatName.isSet() ? mFsFormatName.get() : "FileSystem")));

	for (auto itr = mExtractJobs.begin(); itr != mExtractJobs.end(); itr++)
	{
		// check if root path (legacy case)
		if (itr->virtual_path == tc::io::Path("/"))
		{
			visitDir(tc::io::Path("/"), itr->extract_path, true, false);

			handleLog(fmt::format("Root directory virtual path: \"{:s}\"\n", itr->virtual_path.to_string()));

			// root directory extract successful, continue to next job
			continue;
		}

		// otherwise determine if this is a file or subdirectory
		try {
			std::shared_ptr<tc::io::IStream> file_stream;
			mInputFs->openFile(itr->virtual_path, tc::io::FileMode::Open, tc::io::FileAccess::Read, file_stream);

			handleLog(fmt::format("Valid file path: \"{:s}\"\n", itr->virtual_path.to_string()));

			// the output path for this file will depend on the user specified extract path
			std::shared_ptr<tc::io::IFileSystem> local_fs = std::make_shared<tc::io::LocalFileSystem>(tc::io::LocalFileSystem());

			// case: the extract_path is a valid path to an existing directory
			// behaviour: extract the file, preserving the original filename, to the specified directory
			// method: try getDirectoryListing(itr->extract_path), if this is does not throw, then we can be sure this is a valid path to a directory, file_extract_path = itr->extract_path + itr->virtual_path.back()

			try {
				tc::io::sDirectoryListing dir_listing;
				local_fs->getDirectoryListing(itr->extract_path, dir_listing);

				tc::io::Path file_extract_path = itr->extract_path + itr->virtual_path.back();

				handleLog(fmt::format("Extracting file to {:s}\n", file_extract_path.to_string()));

				writeStreamToFile(file_stream, itr->extract_path + itr->virtual_path.back(), mDataCache);

				continue;
			} catch (tc::io::DirectoryNotFoundException&) {
				// acceptable exception, just means directory didn't exist
			}

			// case: the extract_path up until the last element is a valid path to an existing directory, but the full path specifies neither a directory or a file
			// behaviour: treat extract_path as the intended location to write the extracted file (the original filename is not preserved, instead specified by the user in the final element of the extract path)
			// method: since this checks n-1 elements, it implies a path with more than one element, so that must be accounted for, as relative paths are valid and single element paths aren't always root

			try {
				// get path to parent directory
				tc::io::Path parent_dir_path = itr->extract_path;

				// replace final path element with the current directory alias
				parent_dir_path.pop_back(); // remove filename
				parent_dir_path.push_back("."); // replace with the current dir name alias

				// test parent directory exists
				tc::io::sDirectoryListing dir_listing;
				local_fs->getDirectoryListing(parent_dir_path, dir_listing);

				handleLog(fmt::format("Saving {:s} as {:s}\n", itr->virtual_path.to_string(), itr->extract_path.to_string()));

				writeStreamToFile(file_stream, itr->extract_path, mDataCache);

				continue;
			} catch (tc::io::DirectoryNotFoundException&) {
				// acceptable exception, just means the parent directory didn't exist
			}

			// extract path could not be determined, inform the user and skip this job
			handleLog(fmt::format("[WARNING] Extract path was invalid, and was skipped: {:s}\n", itr->extract_path.to_string()), "warn");
			continue;
		} catch (tc::io::FileNotFoundException&) {
			// acceptable exception, just means file didn't exist
		}

		// not a file, attempt to process this as a directory
		try {
			tc::io::sDirectoryListing dir_listing;
			mInputFs->getDirectoryListing(itr->virtual_path, dir_listing);

			visitDir(itr->virtual_path, itr->extract_path, true, false);

			handleLog(fmt::format("Valid Directory Path: \"{:s}\"\n", itr->virtual_path.to_string()));

			// directory extract successful, continue to next job
			continue;

		} catch (tc::io::DirectoryNotFoundException&) {
			// acceptable exception, just means directory didn't exist
		}

		handleLog(fmt::format("Failed to extract virtual path: \"{:s}\"\n", itr->virtual_path.to_string()), "warn");
	}

}

void nstool::FsProcess::visitDir(const tc::io::Path& v_path, const tc::io::Path& l_path, bool extract_fs, bool print_fs)
{
	tc::io::LocalFileSystem local_fs;

	// get listing for directory
	tc::io::sDirectoryListing info;
	mInputFs->getDirectoryListing(v_path, info);

    const std::string directory = v_path.back();

	if (print_fs) {
        if (!outputJSON) {
            for (size_t i = 0; i < v_path.size(); i++) {
                fmt::print(" ");
            }

            fmt::print("{:s}/\n", ((v_path.size() == 1) ? (mFsRootLabel.isSet() ? (mFsRootLabel.get() + ":")  : "Root:") : directory));
        }
	}

	if (extract_fs) {
		// create local directory
		local_fs.createDirectory(l_path);
	}

	// iterate through child files
	size_t cache_read_len;
	tc::io::Path out_path;
	std::shared_ptr<tc::io::IStream> in_stream;
	std::shared_ptr<tc::io::IStream> out_stream;

	for (auto itr = info.file_list.begin(); itr != info.file_list.end(); itr++) {
        // build out path
        out_path = l_path + *itr;

		if (print_fs) {
            if (outputJSON) {
                const std::string rootLabbel = mFsRootLabel.isSet() ? mFsRootLabel.get() : "Root";

                nlohmann::json entry = {
                    {"rootLabel", rootLabbel},
                    {"directory", directory},
                    {"file", *itr},
                    {"fullPath", fmt::format("{:s}:{:s}", rootLabbel, out_path.to_string())}
                };

                output["archive"]["files"].push_back(entry);
            } else {
                for (size_t i = 0; i < v_path.size(); i++) {
                    fmt::print(" ");
                }

                fmt::print(" {:s}\n", *itr);
            }
		}

		if (extract_fs && (mOutputFile == "" || (mOutputFile == *itr))) {
			handleLog(fmt::format("Saving {:s}\n", out_path.to_string()));

			// begin export
			mInputFs->openFile(v_path + *itr, tc::io::FileMode::Open, tc::io::FileAccess::Read, in_stream);
			local_fs.openFile(out_path, tc::io::FileMode::OpenOrCreate, tc::io::FileAccess::Write, out_stream);

			in_stream->seek(0, tc::io::SeekOrigin::Begin);
			out_stream->seek(0, tc::io::SeekOrigin::Begin);

            for (int64_t remaining_data = in_stream->length(); remaining_data > 0;) {
				cache_read_len = in_stream->read(mDataCache.data(), mDataCache.size());

                if (cache_read_len == 0) {
					throw tc::io::IOException(mModuleLabel, fmt::format("Failed to read from {:s} file.", (mFsFormatName.isSet() ? (mFsFormatName.get() + " ") : "")));
				}

				out_stream->write(mDataCache.data(), cache_read_len);

				remaining_data -= int64_t(cache_read_len);
			}
		}
	}

	// iterate through child directories
	for (auto itr = info.dir_list.begin(); itr != info.dir_list.end(); itr++) {
        // When traversing each directory append the directory to the local path only if we're not looking to extract a single file.
        const tc::io::Path localPath = mOutputFile == "" ? l_path + *itr : l_path;

		visitDir(v_path + *itr, localPath, extract_fs, print_fs);
	}
}
