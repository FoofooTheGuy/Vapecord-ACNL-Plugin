#include <CTRPluginFramework.hpp>
#include "core/infrastructure/Address.hpp"
#include "Files.h"
#include <3ds.h>
#include <cstring>
#include <vector>
#include <string>

namespace CTRPluginFramework {

	static const char *AUTOBACKUP_PREFIX = "AutoBackup";

	static bool OpenSaveArchive(FS_Archive &archive) {
		FS_Path archivePath = fsMakePath(PATH_EMPTY, "");
		return R_SUCCEEDED(FSUSER_OpenArchive(&archive, ARCHIVE_SAVEDATA, archivePath));
	}

	struct SaveFileInfo {
		std::string name;
		u64 size;
	};

	static bool ListSaveFiles(FS_Archive archive, std::vector<SaveFileInfo> &files) {
		Handle dirHandle;
		FS_Path dirPath = fsMakePath(PATH_ASCII, "/");

		if (R_FAILED(FSUSER_OpenDirectory(&dirHandle, archive, dirPath))) {
            return false;
        }

		FS_DirectoryEntry entries[16];
		u32 entriesRead = 0;

		do {
			if (R_FAILED(FSDIR_Read(dirHandle, &entriesRead, 16, entries))) {
                break;
            }

			for (u32 i = 0; i < entriesRead; ++i) {
				if (entries[i].attributes & FS_ATTRIBUTE_DIRECTORY) {
                    continue;
                }

				std::string name;
				Utils::ConvertUTF16ToUTF8(name, reinterpret_cast<const char16_t *>(entries[i].name));
				files.push_back({ name, entries[i].fileSize });
			}
		} while (entriesRead > 0);

		FSDIR_Close(dirHandle);
		return !files.empty();
	}

	static bool CopySaveFileToSD(FS_Archive saveArchive, const SaveFileInfo &info, const std::string &destPath) {
		Handle srcHandle;
		std::string srcPath = "/" + info.name;
		FS_Path fsFilePath = fsMakePath(PATH_ASCII, srcPath.c_str());

		if (R_FAILED(FSUSER_OpenFile(&srcHandle, saveArchive, fsFilePath, FS_OPEN_READ, 0))) {
            return false;
        }

		std::string fullDest = destPath + "/" + info.name;
		File destFile;
		int res = File::Open(destFile, fullDest, File::RWC | File::TRUNCATE);
		if (res != 0) {
			FSFILE_Close(srcHandle);
			return false;
		}

		constexpr u32 CHUNK_SIZE = 0x8000;
		u8 *buffer = new u8[CHUNK_SIZE];
		u64 offset = 0;
		u64 remaining = info.size;
		bool success = true;

		while (remaining > 0) {
			u32 toRead = remaining > CHUNK_SIZE ? CHUNK_SIZE : static_cast<u32>(remaining);
			u32 bytesRead = 0;

			if (R_FAILED(FSFILE_Read(srcHandle, &bytesRead, offset, buffer, toRead)) || bytesRead == 0) {
				success = false;
				break;
			}

			if (destFile.Write(buffer, bytesRead) != 0) {
				success = false;
				break;
			}

			offset += bytesRead;
			remaining -= bytesRead;
		}

		delete[] buffer;
		destFile.Flush();
		destFile.Close();
		FSFILE_Close(srcHandle);
		return success;
	}

	static std::string GetCheckpointGameFolder(void) {
		u64 titleId = Process::GetTitleID();
		u32 uniqueId = static_cast<u32>((titleId >> 8) & 0xFFFFF);
		std::string hexPrefix = Utils::Format("0x%05X", uniqueId);

		Directory checkpointDir;
		if (Directory::Open(checkpointDir, "E:/3ds/Checkpoint/saves") == 0) {
			std::vector<std::string> dirs;
			checkpointDir.ListDirectories(dirs);
			checkpointDir.Close();

			for (const std::string &dir : dirs) {
				if (dir.compare(0, hexPrefix.size(), hexPrefix) == 0) {
                    return dir;
                }
			}
		}

		return hexPrefix + " Animal Crossing New Leaf";
	}

	static void ListAutoBackups(const std::string &gamePath, std::vector<std::string> &backups) {
		Directory gameDir;
		if (Directory::Open(gameDir, gamePath) != 0) {
            return;
        }

		std::vector<std::string> dirs;
		gameDir.ListDirectories(dirs);
		gameDir.Close();

		for (const std::string &dir : dirs) {
			if (dir.compare(0, strlen(AUTOBACKUP_PREFIX), AUTOBACKUP_PREFIX) == 0) {
                backups.push_back(dir);
            }
		}
	}

	static std::string FindOldestBackup(const std::string &gamePath, const std::vector<std::string> &backups) {
		u64 oldestTime = UINT64_MAX;
		std::string oldest = backups[0];

		for (const std::string &backup : backups) {
			std::string markerPath = gamePath + "/" + backup + "/.timestamp";
			File marker;
			if (File::Open(marker, markerPath, File::READ) != 0) {
                return backup;
            }

			u64 timestamp = 0;
			marker.Read(&timestamp, sizeof(timestamp));
			marker.Close();

			if (timestamp < oldestTime) {
				oldestTime = timestamp;
				oldest = backup;
			}
		}

		return oldest;
	}

	static std::string FindNextBackupName(const std::vector<std::string> &backups) {
		for (u32 i = 1; ; ++i) {
			std::string name = Utils::Format("%s %d", AUTOBACKUP_PREFIX, i);
			bool found = false;
			for (const std::string &b : backups) {
				if (b == name) { 
                    found = true; 
                    break; 
                }
			}
			if (!found) {
                return name;
            }
		}
	}

	static void WriteTimestamp(const std::string &path) {
		std::string markerPath = path + "/.timestamp";
		File marker;
		if (File::Open(marker, markerPath, File::RWC | File::TRUNCATE) == 0) {
			u64 timestamp = svcGetSystemTick();
			marker.Write(&timestamp, sizeof(timestamp));
			marker.Flush();
			marker.Close();
		}
	}

	void PerformAutoSaveBackup(void) {
		FS_Archive saveArchive;
		if (!OpenSaveArchive(saveArchive)) {
			OSD::NotifySysFont("Auto-backup: failed to open save archive", Color::Red);
			return;
		}

		std::vector<SaveFileInfo> files;
		if (!ListSaveFiles(saveArchive, files)) {
			FSUSER_CloseArchive(saveArchive);
			OSD::NotifySysFont("Auto-backup: no save files found", Color::Orange);
			return;
		}

		const FwkSettings &settings = FwkSettings::Get();
		u32 maxSlots = settings.SaveBackupMaxSlots;
		if (maxSlots == 0) {
            maxSlots = 3;
        }

		// Ensure Checkpoint directory chain exists
		Directory::Create("E:/3ds");
		Directory::Create("E:/3ds/Checkpoint");
		Directory::Create("E:/3ds/Checkpoint/saves");

		std::string gameFolderName = GetCheckpointGameFolder();
		std::string gamePath = Utils::Format(PATH_AUTOBACKUP, gameFolderName.c_str());
		Directory::Create(gamePath);

		// Only count our auto-backups, ignore user-created Checkpoint backups
		std::vector<std::string> autoBackups;
		ListAutoBackups(gamePath, autoBackups);

		std::string targetFolder;
		if (autoBackups.size() < maxSlots) {
			targetFolder = FindNextBackupName(autoBackups);
		} else {
			targetFolder = FindOldestBackup(gamePath, autoBackups);
		}

		std::string targetPath = gamePath + "/" + targetFolder;
		Directory::Create(targetPath);

		bool allSuccess = true;
		u32 totalFiles = 0;

		for (const SaveFileInfo &info : files) {
			if (CopySaveFileToSD(saveArchive, info, targetPath)) {
                ++totalFiles;
            }
			else {
                allSuccess = false;
            }
		}

		WriteTimestamp(targetPath);
		FSUSER_CloseArchive(saveArchive);

		if (allSuccess) {
            OSD::NotifySysFont(Utils::Format("Auto-backup: %s (%d files)", targetFolder.c_str(), totalFiles), Color::LimeGreen);
        }
		else {
            OSD::NotifySysFont(Utils::Format("Auto-backup: %s (partial, %d files)", targetFolder.c_str(), totalFiles), Color::Orange);
        }
	}
}
