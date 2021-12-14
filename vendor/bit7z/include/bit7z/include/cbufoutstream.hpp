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

#ifndef CBUFOUTSTREAM_HPP
#define CBUFOUTSTREAM_HPP

#include <vector>

#include "../include/bittypes.hpp"

#include "7zip/IStream.h"
#include "Common/MyCom.h"

namespace bit7z {
    using std::vector;

    class CBufOutStream : public ISequentialOutStream, public CMyUnknownImp {
        public:
            explicit CBufOutStream( vector< byte_t >& out_buffer );

            virtual ~CBufOutStream();

            MY_UNKNOWN_IMP1( ISequentialOutStream )

            // ISequentialOutStream
            STDMETHOD( Write )( const void* data, UInt32 size, UInt32* processedSize );

        private:
            vector< byte_t >& mBuffer;
    };
}
#endif // CBUFOUTSTREAM_HPP
