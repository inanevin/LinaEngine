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

#ifndef FSINDEXER_HPP
#define FSINDEXER_HPP

#include <string>
#include <vector>
#include <map>

#include "../include/fsitem.hpp"

namespace bit7z {
    namespace filesystem {
        using std::wstring;
        using std::vector;
        using std::map;

        class FSIndexer {
            public:
                static vector< FSItem > indexDirectory( const wstring& in_dir,
                                                        const wstring& filter = L"",
                                                        bool recursive = true );

                static vector< FSItem > indexPaths( const vector< wstring >& in_paths, bool ignore_dirs = false );

                static vector< FSItem > indexPathsMap( const map< wstring, wstring >& in_paths,
                                                       bool ignore_dirs = false );

            private:
                FSItem mDirItem;
                wstring mFilter;

                explicit FSIndexer( const FSItem& directory, const wstring& filter = L"" );

                void listDirectoryItems( vector< FSItem >& result, bool recursive, const wstring& prefix = L"" );

                static void indexItem( const FSItem& item, bool ignore_dirs, vector< FSItem >& result );
        };
    }
}
#endif // FSINDEXER_HPP
