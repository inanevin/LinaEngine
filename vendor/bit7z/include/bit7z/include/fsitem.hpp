/*
 * bit7z - A C++ static library to interface with the 7-zip DLLs.
 * Copyright (c) 2014-2019  Riccardo Ostani - All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Bit7z is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bit7z; if not, see https://www.gnu.org/licenses/.
 */

#ifndef FSITEM_HPP
#define FSITEM_HPP

#include <string>
#include <cstdint>

#include <Windows.h>

namespace bit7z {
    namespace filesystem {
        typedef WIN32_FIND_DATA FSItemInfo;

        using std::wstring;

        class FSItem {
            public:
                explicit FSItem( const wstring& path, const wstring& inArchivePath = L"" );

                explicit FSItem( const wstring& dir, FSItemInfo data, const wstring& searchPath );

                bool isDots() const;

                bool isDir() const;

                uint64_t size() const;

                FILETIME creationTime() const;

                FILETIME lastAccessTime() const;

                FILETIME lastWriteTime() const;

                wstring name() const;

                wstring path() const;

                wstring inArchivePath() const;

                uint32_t attributes() const;

            private:
                wstring mPath;
                FSItemInfo mFileData;
                wstring mSearchPath;
                wstring mInArchivePath;
        };
    }
}
#endif // FSITEM_HPP
