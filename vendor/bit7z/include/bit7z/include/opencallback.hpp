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

#ifndef OPENCALLBACK_HPP
#define OPENCALLBACK_HPP

#include <string>

#include "7zip/Archive/IArchive.h"
#include "7zip/IPassword.h"
#include "Common/MyCom.h"

#include "../include/callback.hpp"
#include "../include/fsitem.hpp"
#include "../include/bitarchivehandler.hpp"

namespace bit7z {
    using filesystem::FSItem;

    class OpenCallback : public IArchiveOpenCallback,
                         public IArchiveOpenVolumeCallback,
                         public IArchiveOpenSetSubArchiveName,
                         public ICryptoGetTextPassword,
                         public Callback {
        public:
            explicit OpenCallback( const BitArchiveHandler& handler, const std::wstring& filename = L"." );

            virtual ~OpenCallback() override;

            MY_UNKNOWN_IMP3( IArchiveOpenVolumeCallback, IArchiveOpenSetSubArchiveName, ICryptoGetTextPassword )

            //IArchiveOpenCallback
            STDMETHOD( SetTotal )( const UInt64* files, const UInt64* bytes );
            STDMETHOD( SetCompleted )( const UInt64* files, const UInt64* bytes );

            //IArchiveOpenVolumeCallback
            STDMETHOD( GetProperty )( PROPID propID, PROPVARIANT* value );
            STDMETHOD( GetStream )( const wchar_t* name, IInStream** inStream );

            //IArchiveOpenSetSubArchiveName
            STDMETHOD( SetSubArchiveName )( const wchar_t* name );

            //ICryptoGetTextPassword
            STDMETHOD( CryptoGetTextPassword )( BSTR* password );

        private:
            bool mSubArchiveMode;
            wstring mSubArchiveName;
            FSItem mFileItem;
    };
}
#endif // OPENCALLBACK_HPP
