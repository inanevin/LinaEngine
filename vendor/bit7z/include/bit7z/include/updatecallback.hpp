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

#ifndef COMPRESSCALLBACK_HPP
#define COMPRESSCALLBACK_HPP

#include "7zip/Archive/IArchive.h"
#include "7zip/ICoder.h"
#include "7zip/IPassword.h"

#include "../include/callback.hpp"
#include "../include/bitarchivecreator.hpp"
#include "../include/bitinputarchive.hpp"

namespace bit7z {
    CONSTEXPR auto kUnsupportedOperation = "Unsupported operation!";
    CONSTEXPR auto kUnsupportedInMemoryFormat = "Unsupported format for in-memory compression!";
    CONSTEXPR auto kCannotOverwriteBuffer = "Cannot overwrite or update a non empty buffer";

    class UpdateCallback : public Callback,
                           public IArchiveUpdateCallback2,
                           public ICompressProgressInfo,
                           protected ICryptoGetTextPassword2 {
        public:
            virtual ~UpdateCallback() override;
            virtual uint32_t itemsCount() const = 0;

            MY_UNKNOWN_IMP3( IArchiveUpdateCallback2, ICompressProgressInfo, ICryptoGetTextPassword2 )

            void setOldArc( const BitInputArchive* old_arc );

            HRESULT Finilize();

            // IProgress from IArchiveUpdateCallback2
            STDMETHOD( SetTotal )( UInt64 size );
            STDMETHOD( SetCompleted )( const UInt64* completeValue );

            // ICompressProgressInfo
            STDMETHOD( SetRatioInfo )( const UInt64* inSize, const UInt64* outSize );

            // IArchiveUpdateCallback2
            STDMETHOD( EnumProperties )( IEnumSTATPROPSTG** enumerator );
            STDMETHOD( GetUpdateItemInfo )( UInt32 index,
                                            Int32* newData,
                                            Int32* newProperties,
                                            UInt32* indexInArchive );
            STDMETHOD( SetOperationResult )( Int32 operationResult );

            //ICryptoGetTextPassword2
            STDMETHOD( CryptoGetTextPassword2 )( Int32* passwordIsDefined, BSTR* password );

        protected:
            const BitInputArchive* mOldArc;
            uint32_t mOldArcItemsCount;

            bool mAskPassword;
            bool mNeedBeClosed;

            explicit UpdateCallback( const BitArchiveCreator& creator );
    };
}

#endif // COMPRESSCALLBACK_HPP
