// This file is part of Noggit3, licensed under GNU General Public License
// (version 3).

#pragma once

QDebug operator<<(QDebug dbg, const std::string& message);
QDebug operator<<(QDebug dbg, const std::filesystem::path& message);
QDebug operator<<(QDebug dbg, const std::error_code& message);

QDebug& _LogError(const char* pFile, int pLine);
QDebug& _LogDebug(const char* pFile, int pLine);
QDebug& _Log(const char* pFile, int pLine);

#define LogError _LogError(__FILE__, __LINE__)
#define LogDebug _LogDebug(__FILE__, __LINE__)
#define Log _Log(__FILE__, __LINE__)

void InitLogging();
