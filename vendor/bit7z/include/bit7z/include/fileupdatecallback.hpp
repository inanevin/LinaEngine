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

#ifndef UPDATECALLBACK_HPP
#define UPDATECALLBACK_HPP

#include "../include/bitinputarchive.hpp"
#include "../include/bitarchiveitem.hpp"
#include "../include/updatecallback.hpp"
#include "../include/fsitem.hpp"
#include "../include/bitarchivecreator.hpp"

#include <vector>

namespace bit7z {
    using namespace filesystem;
    using std::pair;
    using std::vector;
    using std::wstring;

    class FileUpdateCallback : public UpdateCallback {
        public:
            explicit FileUpdateCallback( const BitArchiveCreator& creator, const vector< FSItem >& new_items );

            virtual ~FileUpdateCallback() override;

            // CompressCallback
            uint32_t itemsCount() const override;
            wstring getErrorMessage() const override;

            // IArchiveUpdateCallback2
            STDMETHOD( GetProperty )( UInt32 index, PROPID propID, PROPVARIANT* value );
            STDMETHOD( GetStream )( UInt32 index, ISequentialInStream** inStream );
            STDMETHOD( GetVolumeSize )( UInt32 index, UInt64* size );
            STDMETHOD( GetVolumeStream )( UInt32 index, ISequentialOutStream** volumeStream );

        private:
            const vector< FSItem >& mNewItems;

            uint64_t mVolSize;
            wstring mVolName;

            vector< pair< wstring, HRESULT > > mFailedFiles;
    };
}
#endif // UPDATECALLBACK_HPP
